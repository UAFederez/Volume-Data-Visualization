#include "VolumeViewCanvas2D.h"


VolumeViewCanvas2D::VolumeViewCanvas2D(
	wxWindow* parent    ,  VolumeDataset* dataset,
	AnatomicalAxis axis ,  std::shared_ptr<wxGLContext> context) :
	VolumeViewCanvas(parent, dataset, context),
	m_axis(axis)
{
	Bind(wxEVT_PAINT	, [=](wxPaintEvent& e) { Render(e);            } , GetId());
	Bind(wxEVT_LEFT_DOWN, [=](wxMouseEvent& e) { HandleLeftClick(e);   } , GetId());
	Bind(wxEVT_LEFT_UP  , [=](wxMouseEvent& e) { HandleLeftRelease(e); } , GetId());
	Bind(wxEVT_MOTION   , [=](wxMouseEvent& e) { HandleMouseMove(e);   } , GetId());
	Bind(wxEVT_MOUSEWHEEL   , [=](wxMouseEvent& e) { HandleMouseScroll(e);   } , GetId());
	Init();
}

void VolumeViewCanvas2D::HandleMouseScroll(wxMouseEvent& evt)
{
	I32 rotation = evt.GetWheelRotation();
	U32 extent   = 0;

	switch (m_axis)
	{
		case AnatomicalAxis::CORONAL:
		{
			extent = m_dataset->DataSize()[2];
		}
		break;
		case AnatomicalAxis::SAGITTAL:
		{
			extent = m_dataset->DataSize()[0];
		}
		break;
		case AnatomicalAxis::HORIZONTAL:
		{
			extent = m_dataset->DataSize()[1];
		}
		break;
	}

	m_sliceOffset += (1.0f / (float) extent) * (rotation > 0 ? 1.0f : -1.0f);
	m_sliceOffset  = std::min((float) 1.0f, std::max(0.0f, m_sliceOffset));
	Refresh();
}

void VolumeViewCanvas2D::Init()
{
	if(m_glContext == nullptr) return;

	SetCurrent(*m_glContext);

	if (m_dataset == nullptr) return;

	float quadVertices[] = {
		0.0f, 0.0f, 0.0f, 0.0f, // lower left
		1.0f, 0.0f, 1.0f, 0.0f, // lower right
		1.0f, 1.0f, 1.0f, 1.0f, // upper right
		0.0f, 1.0f, 0.0f, 1.0f, // upper left
	};
	
	unsigned int indices[] = {
		0, 1, 2, 0, 2, 3
	};

	glGenVertexArrays(1, &m_imageVao);
	glGenBuffers(1, &m_imageVbo);
	glGenBuffers(1, &m_imageEbo);

	glBindVertexArray(m_imageVao);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_imageVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_imageEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	m_imageShader = Shader("shaders/volume_cross_section_vert.glsl", "shaders/volume_cross_section_frag.glsl");

	Refresh();
}

void VolumeViewCanvas2D::Render(wxPaintEvent& evt)
{
	SetCurrent(*m_glContext);

	wxRect clientRect = GetClientRect();

	glViewport(clientRect.x, clientRect.y, clientRect.width, clientRect.height);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glm::vec2 dimensions = glm::vec2(0.0f);
	glm::vec3 x_perm_col = glm::vec3(0.0f);
	glm::vec3 y_perm_col = glm::vec3(0.0f);
	glm::vec3 z_perm_col = glm::vec3(0.0f);

	switch(m_axis)
	{
		case AnatomicalAxis::CORONAL:
		{
			dimensions = glm::vec2(m_dataset->DataSize()[0], m_dataset->DataSize()[1]);

			// [ x, y, z ] -> [ x, y, z ]
			x_perm_col = glm::vec3(1.0f, 0.0f, 0.0f);
			y_perm_col = glm::vec3(0.0f, 1.0f, 0.0f);
			z_perm_col = glm::vec3(0.0f, 0.0f, 1.0f);
		}
		break;
		case AnatomicalAxis::SAGITTAL:
		{
			dimensions = glm::vec2(m_dataset->DataSize()[2], m_dataset->DataSize()[1]);

			// [ x, y, z ] -> [ z, y, x ]
			x_perm_col = glm::vec3(0.0f, 0.0f, 1.0f);
			y_perm_col = glm::vec3(0.0f, 1.0f, 0.0f);
			z_perm_col = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		break;
		case AnatomicalAxis::HORIZONTAL:
		{
			dimensions = glm::vec2(m_dataset->DataSize()[0], m_dataset->DataSize()[2]);

			// [ x, y, z ] -> [ x, z, y ]
			x_perm_col = glm::vec3(1.0f, 0.0f, 0.0f);
			y_perm_col = glm::vec3(0.0f, 0.0f, 1.0f);
			z_perm_col = glm::vec3(0.0f, 1.0f, 0.0f);
		}
		break;
	}
	glm::mat3 permutationMatrix = glm::mat3(x_perm_col, y_perm_col, z_perm_col);
	const U32 padding = 10;
	glDisable(GL_DEPTH_TEST);

	if (m_dataset != nullptr)
	{
		const float scaleFactor = dimensions.x > dimensions.y ? 
								  ((float) (clientRect.width - padding) / dimensions.x) :
								  ((float) (clientRect.height - padding) / dimensions.y);
		
		const float minDimension = std::min(clientRect.width - padding, clientRect.height - padding);
		const glm::vec3 scaleVec = glm::vec3(dimensions.x * scaleFactor, dimensions.y * scaleFactor, 1.0f);

		glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), scaleVec);
		glm::mat4 viewMatrix  = glm::translate(
			glm::mat4(1.0f),
			glm::vec3(
				(float) ((clientRect.width  - padding) - scaleVec.x) / 2.0f,
				(float) ((clientRect.height - padding) - scaleVec.y) / 2.0f,
				0.0f
			)
		);


		glm::mat4 projMatrix  = glm::ortho(0.0f, (float) clientRect.width, (float) clientRect.height, 0.0f, -1.0f, 1.0f);

		m_imageShader.UseProgram();
		m_imageShader.SetMatrix4x4("model", modelMatrix);
		m_imageShader.SetMatrix4x4("view", viewMatrix);
		m_imageShader.SetMatrix4x4("projection", projMatrix);

		m_imageShader.SetMatrix3x3("uvPermutation", permutationMatrix);
		m_imageShader.SetFloat("sliceOffset", m_sliceOffset);

		glBindTexture(GL_TEXTURE_3D, m_textureId);
		glBindVertexArray(m_imageVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	}
	SwapBuffers();
}

void VolumeViewCanvas2D::Deallocate()
{
	SetCurrent(*m_glContext);
	glDeleteVertexArrays(1, &m_imageVao);
	glDeleteBuffers(1, &m_imageVbo);
	glDeleteBuffers(1, &m_imageEbo);
}

void VolumeViewCanvas2D::HandleLeftClick(wxMouseEvent& evt)
{

}

void VolumeViewCanvas2D::HandleLeftRelease(wxMouseEvent& evt)
{

}

void VolumeViewCanvas2D::HandleMouseMove(wxMouseEvent& evt)
{

}