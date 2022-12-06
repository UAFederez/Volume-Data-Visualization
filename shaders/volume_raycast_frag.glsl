#version 450 core

in  vec3 PosInWorld;
in  vec3 Origin;
out vec4 FragmentColor;

/**
 * 0 - Maximum intensity projecition
 * 2 - Average intensity projection
 * 1 - First-hit 
**/
uniform uint   projMethod = 0u;
uniform float  firstHitThreshold = 1.0f;

uniform float minValue = 0.0f;
uniform float maxValue = 1.0f;

uniform mat4 rotation;
uniform vec3 volumeSize;
uniform vec3 cameraPosition;

uniform sampler3D texture3d;
uniform sampler2D transferTexture;


/**
* 
* @param r0	Ray origin
* @param rd	Ray direction
* @param p0	Plane origin
* @param n		Plane normal
* 
* Returns a vec2 wherein:
* x - t-value for the ray if there is an intersection
* y - 0 if there is no intersection, 1 if there is
**/
vec2 TestRayIntersection(vec3 r0, vec3 rd, vec3 p0, vec3 n)
{
	float denom = dot(n, rd);
	if (abs(denom) < 1e-16) {
		return vec2(1e8);
	}
	return vec2(dot(n, normalize(p0 - r0)) / denom, 1.0f);
}

vec3 CalculateTexturePos(vec3 currPos)
{
	vec3 texturePos = vec3(transpose(rotation) * vec4(currPos - Origin, 1.0f));
	texturePos = vec3(round(texturePos.x), round(texturePos.y), round(texturePos.z)) / volumeSize;

	return texturePos;
}

void main()
{
	vec3 rayDir  = normalize(PosInWorld - cameraPosition);
	vec3 currPos = PosInWorld;

	float currIntensity  = (texture(texture3d, currPos).r - minValue) / (maxValue - minValue);
	float maxIntensity   = currIntensity;
	float totalIntensity = currIntensity;

	vec3 texturePos  = vec3(0.5f);
	vec3 posAtMax    = currPos;
	vec4 outputColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	const vec3 dx = vec3(1.0f, 0.0f, 0.0f);
	const vec3 dy = vec3(0.0f, 1.0f, 0.0f);
	const vec3 dz = vec3(0.0f, 0.0f, 1.0f);

	float contribution    = 1.0f;
	float numTotalSamples = 1.0f;

	while(true)
	{
		// The (currPos - rotatedOrigin) vector needs to be transformed with the inverse
		// of the rotation applied to it in order to get valid coordinates within the space
		// of the 3d texture.
		texturePos = vec3(transpose(rotation) * vec4(currPos - Origin, 1.0f));
		texturePos = vec3(round(texturePos.x), round(texturePos.y), round(texturePos.z)) / volumeSize;
	
		// Check if the point to sample the 3d texture is still within the bounds of the actual 
		// texture
		bool isOutsideX = (texturePos.x < 0.0 || texturePos.x > 1.0f);
		bool isOutsideY = (texturePos.y < 0.0 || texturePos.y > 1.0f);
		bool isOutsideZ = (texturePos.z < 0.0 || texturePos.z > 1.0f);
	
		if (isOutsideX || isOutsideY || isOutsideZ)
			break;
	
		currIntensity = (texture(texture3d, texturePos).r - minValue) / (maxValue - minValue);	
		posAtMax	  = currIntensity > maxIntensity ? currPos : posAtMax;
		maxIntensity = max(maxIntensity, currIntensity);

		totalIntensity  += currIntensity;
		numTotalSamples += 1.0f;

		if (projMethod == 2u && currIntensity > firstHitThreshold)
			break;
	
		// Recall that:
		//		>  ceil(x) - 1 is the greatest integer strictly less than x
		//		> floor(x) + 1 is the lowest integer strictly greater than x
	
		// Check intersection with the adjacent Y-Z plane (normal facing the x axis)
		
		vec3 xNorm = vec3((rayDir.x > 0.0f ? -1.0f : 1.0f), 0.0f, 0.0f);
		vec3 xOrig = vec3((rayDir.x > 0.0f ? floor(currPos.x) + 1: ceil(currPos.x) - 1), currPos.y, currPos.z);
		vec2 xTest = TestRayIntersection(currPos, rayDir, xOrig, xNorm);
	
		// Check intersection with the adjacent X-Z plane (normal facing the y axis)
		vec3 yNorm = vec3(0.0f, (rayDir.y > 0 ? -1.0f : 1.0f), 0.0f);
		vec3 yOrig = vec3(currPos.x, (rayDir.y > 0.0f ? floor(currPos.y) + 1 : ceil(currPos.y) - 1), currPos.z);
		vec2 yTest = TestRayIntersection(currPos, rayDir, yOrig, yNorm);
	
		// Check intersection with the adjacent X-Y plane (normal facing the z axis)
		vec3 zNorm = vec3(0.0f, 0.0f, (rayDir.z > 0 ?  -1.0f : 1.0f ));
		vec3 zOrig = vec3(currPos.x, currPos.y, (rayDir.z > 0.0f ? floor(currPos.z) + 1 : ceil(currPos.z) - 1));
		vec2 zTest = TestRayIntersection(currPos, rayDir, zOrig, zNorm);
	
		// Move in the minimum amount possible such that only one voxel is traversed
		// at any given iteration to ensure full coverage
		float tMin = min(zTest.x, min(yTest.x, xTest.x));
	
		currPos += tMin * rayDir;
	}


	// Compute gradient
	float gx = texture(texture3d, CalculateTexturePos(posAtMax + dx)).r - texture(texture3d, CalculateTexturePos(posAtMax - dx)).r;
	float gy = texture(texture3d, CalculateTexturePos(posAtMax + dy)).r - texture(texture3d, CalculateTexturePos(posAtMax - dy)).r;
	float gz = texture(texture3d, CalculateTexturePos(posAtMax + dz)).r - texture(texture3d, CalculateTexturePos(posAtMax - dz)).r;

	vec3 gradient   = -normalize(vec3(gx, gy, gz) / 2.0f);
	vec3 lightDir   = normalize(volumeSize - currPos);	// volumeSize is the position of the light upper right corner
	vec3 reflectDir = reflect(-lightDir, gradient);

	vec3 ambient  = vec3(0.25f);
	vec3 diffuse  = vec3(1.00f) * max(0.5f, dot(gradient, lightDir));
	vec3 specular = vec3(1.00f) * pow(max(dot(rayDir, reflectDir), 0.0f), 32) * 0.5f;

	float finalIntensity = maxIntensity;

	switch (projMethod)
	{
		case 1u: finalIntensity = totalIntensity / numTotalSamples;break;
		case 2u: finalIntensity = currIntensity ;break;
		default: ;break;
	}

	FragmentColor = vec4(ambient + diffuse + specular, 1.0f) * texture(transferTexture, vec2(finalIntensity, 0.0f));;
}
