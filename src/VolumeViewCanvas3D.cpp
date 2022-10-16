#include "VolumeViewCanvas3D.h"

VolumeViewCanvas3D::VolumeViewCanvas3D(
	wxWindow* parent, const std::shared_ptr<VolumeModel> model) :
	VolumeViewCanvas(parent, model)
{
	Bind(wxEVT_PAINT	 , [=](wxPaintEvent& e) { Render(e);            } , GetId());
	Bind(wxEVT_LEFT_DOWN , [=](wxMouseEvent& e) { HandleLeftClick(e);   } , GetId());
	Bind(wxEVT_LEFT_UP   , [=](wxMouseEvent& e) { HandleLeftRelease(e); } , GetId());
	Bind(wxEVT_MOTION    , [=](wxMouseEvent& e) { HandleMouseMove(e);   } , GetId());
	Bind(wxEVT_MOUSEWHEEL, [=](wxMouseEvent& e) { HandleMouseScroll(e);   } , GetId());

	Init();
}

void VolumeViewCanvas3D::HandleMouseScroll(wxMouseEvent& evt)
{
	I32 rotation   = evt.GetWheelRotation();
	auto dataSize  = m_volumeModel->m_dataset->DataSize();


	const R64 relSpaceX = 1.0f;
	const R64 relSpaceY = m_volumeModel->m_dataset->DataSpacing()[1] / m_volumeModel->m_dataset->DataSpacing()[0];
	const R64 relSpaceZ = m_volumeModel->m_dataset->DataSpacing()[2] / m_volumeModel->m_dataset->DataSpacing()[0];

	const float SPEED    = 5.0f;
	const float MIN_DIST = std::min(std::min(dataSize[0], dataSize[1]), dataSize[2]);
	const float MAX_DIST = std::max(std::max(dataSize[0], dataSize[1]), dataSize[2]) + 350.0f;

	m_cameraPos.z += (m_volumeModel->m_dataset->DataSize()[2] / std::max(SPEED, 1.0f)) * (rotation > 0 ? -1.0f : 1.0f);
	m_cameraPos.z  = std::max(MIN_DIST, std::min(m_cameraPos.z, MAX_DIST));

	Refresh();
}

void VolumeViewCanvas3D::Init()
{
	if(m_volumeModel->m_sharedContext == nullptr) return;

	SetCurrent(*m_volumeModel->m_sharedContext);

	if (m_volumeModel->m_dataset == nullptr) return;
	
	if (m_hasPreviousData)
	{
		Deallocate();
		m_cameraPos = glm::vec3(0.0f);
		m_mouseHemi0 = glm::vec3(0.0f);
		m_mouseHemi1 = glm::vec3(0.0f);
		m_rotateAxis = glm::vec3(1.0f);
		m_previousRotation = glm::mat4(1.0f);
		m_currentRotation  = glm::mat4(1.0f);
		m_rotateAngle = 0.0f;
		m_hasPreviousData = false;
	}

	m_volumeBounds = ConstructVolumeBoundsFromDataset(m_volumeModel->m_dataset.get());

	/**
	* Loading the OpenGL data for slices
	**/
	float vertexData[8 * 3];
	for (U32 i = 0; i < 8; ++i)
	{
		vertexData[i * 3 + 0] = m_volumeBounds.vertices[i].x;
		vertexData[i * 3 + 1] = m_volumeBounds.vertices[i].y;
		vertexData[i * 3 + 2] = m_volumeBounds.vertices[i].z;
	}

	glGenVertexArrays(1, &m_volBoundsVao);
	glGenBuffers(1, &m_volBoundsVbo);
	glGenBuffers(1, &m_volBoundsEbo);

	glBindVertexArray(m_volBoundsVao);
	glBindBuffer(GL_ARRAY_BUFFER, m_volBoundsVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_volBoundsEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_volumeBounds.edgeIndices), m_volumeBounds.edgeIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	/**
	* Shaders
	**/
	m_volumeShader		 = Shader("shaders/volume_raycast_vert.glsl", "shaders/volume_raycast_frag.glsl");
	m_volumeBoundsShader = Shader("shaders/volume_raycast_vert.glsl", "shaders/volume_raycast_border_frag.glsl");

	// Initialize camera position
	m_cameraPos = glm::vec3(0.0f, 0.0f, m_volumeModel->m_dataset->DataSize()[2] + 350.0f);

	m_hasPreviousData = true;

	Refresh();
}

void VolumeViewCanvas3D::Render(wxPaintEvent& evt)
{
	SetCurrent(*m_volumeModel->m_sharedContext);

	wxRect clientRect = GetClientRect();
	glViewport(clientRect.x, clientRect.y, clientRect.width, clientRect.height);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	if (m_volumeModel->m_dataset != nullptr)
	{
		const float viewRadius = 512.0f;

		const R64 relSpaceX = 1.0f;
		const R64 relSpaceY = m_volumeModel->m_dataset->DataSpacing()[1] / m_volumeModel->m_dataset->DataSpacing()[0];
		const R64 relSpaceZ = m_volumeModel->m_dataset->DataSpacing()[2] / m_volumeModel->m_dataset->DataSpacing()[0];
		
		const float aspectRatio = (float) clientRect.width / (float) clientRect.height;

		glm::vec3 volumeSize = glm::vec3( (float) relSpaceX * m_volumeModel->m_dataset->DataSize()[0], 
										  (float) relSpaceY * m_volumeModel->m_dataset->DataSize()[1],
										  (float) relSpaceZ * m_volumeModel->m_dataset->DataSize()[2]);
		glm::vec3 volumeOrigin = 0.5f * -volumeSize;

		glm::mat4 viewMatrix = glm::lookAt(m_cameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projMatrix = glm::perspective(glm::radians(50.0f), aspectRatio, 0.1f, 3072.0f);
		glm::mat4 translate  = glm::mat4(1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, m_volumeModel->m_texture.GetTextureID());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_volumeModel->m_transferFunction.m_cmapTex);

		glBindVertexArray(m_volBoundsVao);

		if (m_isBoxVisible)
		{
			m_volumeBoundsShader.UseProgram();
			m_volumeBoundsShader.SetMatrix4x4("model", translate);
			m_volumeBoundsShader.SetMatrix4x4("rotation", m_currentRotation);
			m_volumeBoundsShader.SetMatrix4x4("view", viewMatrix);
			m_volumeBoundsShader.SetMatrix4x4("projection", projMatrix);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		
		m_volumeShader.UseProgram();

		switch (m_projectionMethod)
		{
			case ProjectionMethod::MAX_INTENSITY:
			{
				m_volumeShader.SetUint("projectionMethod", 0);
			} break;
			case ProjectionMethod::AVERAGE:
			{
				m_volumeShader.SetUint("projectionMethod", 1);
			} break;
			case ProjectionMethod::FIRST_HIT:
			{
				m_volumeShader.SetUint("projectionMethod", 2);
				m_volumeShader.SetFloat("firstHitThreshold", (R32) m_firstHitThreshold);
			} break;
		}

		m_volumeShader.SetFloat("maxValue", m_volumeModel->m_dataset->GetMaxInDoubleRange());
		m_volumeShader.SetFloat("minValue", m_volumeModel->m_dataset->GetMinInDoubleRange());
		m_volumeShader.SetInt("texture3d", 0);
		m_volumeShader.SetInt("textureColor", 1);
		m_volumeShader.SetVector3("cameraPosition", m_cameraPos);
		m_volumeShader.SetVector3("volumeOrigin", volumeOrigin);
		m_volumeShader.SetVector3("volumeSize", volumeSize);

		m_volumeShader.SetMatrix4x4("model", translate);
		m_volumeShader.SetMatrix4x4("rotation", m_currentRotation);
		m_volumeShader.SetMatrix4x4("view", viewMatrix);
		m_volumeShader.SetMatrix4x4("projection", projMatrix);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	glFlush();
	SwapBuffers();
}

void VolumeViewCanvas3D::Deallocate()
{
	SetCurrent(*m_volumeModel->m_sharedContext);
	glDeleteVertexArrays(1, &m_volBoundsVao);
	glDeleteBuffers(1, &m_volBoundsVbo);
	glDeleteBuffers(1, &m_volBoundsEbo);
}

void VolumeViewCanvas3D::HandleLeftClick(wxMouseEvent& event)
{
	wxRect clientRect = GetClientRect();

	U32 xPos = event.GetX();
	U32 yPos = event.GetY();

	isMouseDown = true;

	m_mouseHemi0.x =  (2.0 * xPos - clientRect.width ) / (R32) clientRect.width;
	m_mouseHemi0.y = -(2.0 * yPos - clientRect.height) / (R32) clientRect.height;

	const R32 lengthSquared = (m_mouseHemi0.x * m_mouseHemi0.x) + (m_mouseHemi0.y * m_mouseHemi0.y);

	if (lengthSquared <= 1.0f)
		m_mouseHemi0.z = std::sqrt(1 - lengthSquared);
	else
		m_mouseHemi0 = glm::normalize(m_mouseHemi0);

	Refresh();
}

void VolumeViewCanvas3D::HandleLeftRelease(wxMouseEvent& event)
{
	isMouseDown   = false;
	m_previousRotation = m_currentRotation;
	Refresh();
}

void VolumeViewCanvas3D::HandleMouseMove(wxMouseEvent& event)
{
	wxRect clientRect = GetClientRect();

	if (isMouseDown)
	{
		U32 xPos = event.GetX();
		U32 yPos = event.GetY();

		m_mouseHemi1.x =  (2.0 * xPos - clientRect.width ) / (R32) clientRect.width;
		m_mouseHemi1.y = -(2.0 * yPos - clientRect.height) / (R32) clientRect.height;

		const R32 lengthSquared = (m_mouseHemi1.x * m_mouseHemi1.x) + (m_mouseHemi1.y * m_mouseHemi1.y);

		if (lengthSquared <= 1.0f)
			m_mouseHemi1.z = std::sqrt(1 - lengthSquared);
		else
			m_mouseHemi1 = glm::normalize(m_mouseHemi1);

		const float rotateSpeed = 75.0f;

		m_rotateAxis  = glm::normalize(glm::cross(m_mouseHemi0, m_mouseHemi1));
		m_rotateAngle = glm::acos(glm::dot(m_mouseHemi0, m_mouseHemi1)) * rotateSpeed;

		m_currentRotation = glm::rotate(m_previousRotation, 
			glm::radians(m_rotateAngle), 
			glm::vec3(glm::transpose(m_previousRotation) * glm::vec4(m_rotateAxis, 1.0f)));

	}
	Refresh();
}

VolumeBounds ConstructVolumeBoundsFromDataset(const VolumeDataset* data)
{
	VolumeBounds v = {};

	const R64 relSpaceX = 1.0f;
	const R64 relSpaceY = data->DataSpacing()[1] / data->DataSpacing()[0];
	const R64 relSpaceZ = data->DataSpacing()[2] / data->DataSpacing()[0];

	const float halfSizeX = relSpaceX * data->DataSize()[0] / 2.0f;
	const float halfSizeY = relSpaceY * data->DataSize()[1] / 2.0f;
	const float halfSizeZ = relSpaceZ * data->DataSize()[2] / 2.0f;

	v.vertices[0] = glm::vec3(-halfSizeX, -halfSizeY, -halfSizeZ); // back lower left
	v.vertices[1] = glm::vec3( halfSizeX, -halfSizeY, -halfSizeZ); // back lower right
	v.vertices[2] = glm::vec3(-halfSizeX,  halfSizeY, -halfSizeZ); // back upper left
	v.vertices[3] = glm::vec3( halfSizeX,  halfSizeY, -halfSizeZ); // back upper right
	v.vertices[4] = glm::vec3(-halfSizeX, -halfSizeY,  halfSizeZ); // front lower left
	v.vertices[5] = glm::vec3( halfSizeX, -halfSizeY,  halfSizeZ); // front lower right
	v.vertices[6] = glm::vec3(-halfSizeX,  halfSizeY,  halfSizeZ); // front upper left
	v.vertices[7] = glm::vec3( halfSizeX,  halfSizeY,  halfSizeZ); // front upper right

	U32 edgeIndices[36] = {
		0, 1, 3, 0, 3, 2, // Back face
		4, 0, 2, 4, 2, 6, // West face,
		5, 1, 3, 5, 3, 7, // East face
		4, 5, 1, 4, 1, 0, // Bottom
		6, 7, 3, 6, 3, 2, // Top face
		4, 5, 7, 4, 7, 6, // Front face
	};

	std::copy(std::begin(edgeIndices), std::end(edgeIndices), std::begin(v.edgeIndices));

	return v;
}
