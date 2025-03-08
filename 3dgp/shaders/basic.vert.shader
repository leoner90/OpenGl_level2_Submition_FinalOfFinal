// VERTEX SHADER
#version 330

// Matrices
uniform mat4 matrixProjection; // position in space (perspective, near Plain etc.)
uniform mat4 matrixView; // camera
uniform mat4 matrixModelView; // combined two to simplify calculations (one operation instead of two!)

// Materials - can be one material instead of 3 
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess; // addition to specular light

//imput variables automaticly taken from a mesh (order Matter!!!! becouse they are ordered in a mesh)
in vec3 aVertex;
in vec3 aNormal;

//just to provide vertexes to fragment shader
in vec2 aTexCoord;
out vec2 texCoord0;

//gloabl vars
out vec4 color;
out vec4 position;
out vec3 normal;

//********** LIGHT DECLARATION **********

//AMBIENT LIGHT
struct AMBIENT
{
	vec3 color;
};

uniform AMBIENT lightAmbient, lightEmissive; // lightEmissive for bulb (not used)

vec4 AmbientLight(AMBIENT light)
{
	// Calculate Ambient Light
	return vec4(materialAmbient * light.color, 1);
}

//Normal Map
in vec3 aTangent;
in vec3 aBiTangent;
out mat3 matrixTangent;

//fog
out float fogFactor;
uniform float fogDensity;
 
//shadow
uniform mat4 matrixShadow;
out vec4 shadowCoord;
uniform mat4 shadowCameraView;

//water reflection
uniform vec4 planeClip;	

out float waterDepth;
uniform float waterLevel;


// Bone Transforms
#define MAX_BONES 100
uniform mat4 bones[MAX_BONES];
in ivec4 aBoneId; // Bone Ids
in vec4 aBoneWeight; // Bone Weights

void main(void)
{
	mat4 matrixBone;
	if (aBoneWeight[0] == 0.0)
	{
		// calculate position
		position = matrixModelView * vec4(aVertex, 1.0);
		gl_Position = matrixProjection * position;

		//****** NORMALS
		normal = normalize(mat3(matrixModelView) * aNormal); 
		vec3 tangent = normalize(mat3(matrixModelView) * aTangent);
		vec3 biTangent = normalize(mat3(matrixModelView) * aBiTangent);
		matrixTangent = mat3(tangent, biTangent, normal);
	}

	else
	{
		matrixBone = (bones[aBoneId[0]] * aBoneWeight[0] +
		bones[aBoneId[1]] * aBoneWeight[1] +
		bones[aBoneId[2]] * aBoneWeight[2] +
		bones[aBoneId[3]] * aBoneWeight[3]);

		// calculate position
		position = matrixModelView * matrixBone * vec4(aVertex, 1.0);
		gl_Position = matrixProjection * position;

		// calculate normal
		normal = normalize(mat3(matrixModelView) * mat3(matrixBone) * aNormal);
	}
 
	//****** STANSIL REFLECTION
	gl_ClipDistance[0] = dot(inverse(matrixView) * position, planeClip);

	//****** SHORE
	waterDepth = waterLevel - aVertex.y;

	// ****** Textures
	texCoord0 = aTexCoord;

	//****** FOG
	fogFactor = exp2(-fogDensity * length(position));
 
 	//****** LIGHT
	color = vec4(0, 0, 0, 1);
	color += AmbientLight(lightAmbient);

}