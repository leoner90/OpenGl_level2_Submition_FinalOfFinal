#include <iostream>
#include <GL/glew.h>
#include <3dgl/3dgl.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")
#include "globalVar.h"
#include "initFun.h"
#include "LightAndSkyBox.h"
#include "keyboardAndMouseActions.h"


//*********** RENDER SCENE ***********
void renderScene(mat4& matrixView, float time, float deltaTime)
{
	program.sendUniform("matrixView", matrixView);
	CalcCurrentDayTime();
	Directional();
	AmbientLight();

	//cristmasTree
	cristmasTree.render(setMatrix({ 4.f, 8.5f, -7.f }, 0.f, { 1.0f, 1.0f, 0.0f }, { 7.f,  7.f ,  7.f }, { 0.f, 1.f, 0.f }));
	cristmasTree.render(setMatrix({ 40.f, 24.5f, 27.f }, 0.f, { 1.0f, 1.0f, 0.0f }, { 7.f,  7.f ,  7.f }, { 0.f, 1.f, 0.f }));
	cristmasTree.render(setMatrix({ 70.f, 24.5f, 9.f }, 0.f, { 1.0f, 1.0f, 0.0f }, { 7.f,  7.f ,  7.f }, { 0.f, 1.f, 0.f }));

	//House
	house.render(setMatrix({ 53.f, 24.5f, 10.f }, 0.f, { 1.0f, 1.0f, 0.0f }, { 0.1f,  0.1f ,  0.1f }, { 1.f, 1.f, 1.f }));

	//Terrain
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, idTexNormal);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexSnow);

	program.sendUniform("shoreTime", true); // for SHORE :)))) check report
	program.sendUniform("materialSpecular", vec3(0.1, 0.1, 0.1));
	terrain.render(setMatrix({ 0.f, 0.f, 0.f }, 0.f, { 0.0f, 1.0f, 0.0f }, { 1.f,  1.f ,  1.f }, { 1.2f, 1.2f, 1.2f }));
	program.sendUniform("shoreTime", false);// for SHORE :)))) check report

	//Road
	program.sendUniform("materialSpecular", vec3(1, 1, 1.5));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, idTexNormalICe);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexRoad);
	road.render(setMatrix({ 0.f, 0.45f, 0.f }, 0.f, { 0.0f, 1.0f, 0.0f }, { 1.f,  1.f ,  1.f }, { 1.f, 1.f, 1.f }));

	//STREET LAMPS
	streetLampFun(5.5f, 13.4f, -14.f, "lightPoint.position", "lightPoint.diffuse", "lightPoint.specular", AreLeftLampsOff);
	streetLampFun(16.f, 16.f, -13.f, "lightPoint2.position", "lightPoint2.diffuse", "lightPoint2.specular", AreLeftLampsOff);
	streetLampFun(49.f, 17.2f, -21.f, "lightPoint3.position", "lightPoint3.diffuse", "lightPoint3.specular", AreRightLampsOff);
	streetLampFun(63.f, 18.2f, -44.f, "lightPoint4.position", "lightPoint4.diffuse", "lightPoint4.specular", AreLeftLampsOff);
	streetLampFun(76.f, 19.2f, -30.f, "lightPoint5.position", "lightPoint5.diffuse", "lightPoint5.specular", AreRightLampsOff);
	streetLampFun(99.f, 19.6f, -20.f, "lightPoint6.position", "lightPoint6.diffuse", "lightPoint6.specular", AreLeftLampsOff);
	streetLampFun(87.f, 22.4f, 8.f, "lightPoint7.position", "lightPoint7.diffuse", "lightPoint7.specular", AreRightLampsOff);
	streetLampFun(90.f, 24.f, 37.f, "lightPoint8.position", "lightPoint8.diffuse", "lightPoint8.specular", AreLeftLampsOff);
	streetLampFun(57.f, 28.5f, 37.f, "lightPoint9.position", "lightPoint9.diffuse", "lightPoint9.specular", AreRightLampsOff);
	streetLampFun(29.f, 28.9f, 37.f, "lightPoint10.position", "lightPoint10.diffuse", "lightPoint10.specular", AreRightLampsOff);

	// delete shininess for rest of scene  
	program.sendUniform("materialSpecular", vec3(0, 0, 0)); // bluish colouring
}

//******* REFLECTION -> STENCIL TEST + SCENE RENDER
void planarReflection(mat4& matrixView, float time, float deltaTime)
{
	// Enable stencil test for reflection
	glEnable(GL_STENCIL_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDisable(GL_DEPTH_TEST);
				glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
					glStencilFunc(GL_ALWAYS, 1, 1);
						programWater.use();
						mat4 m = matrixView;
						m = translate(m, vec3(0, waterLevel, 0));
						m = scale(m, vec3(1.f, 1.0f, 1.f));
						programWater.sendUniform("matrixModelView", m);
						water.render(m);
					glStencilFunc(GL_EQUAL, 1, 1);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glEnable(GL_DEPTH_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		//SKYBOX
		SkyBoxAndDayCalculation(true);

		//CLIP PLANE
		glEnable(GL_CLIP_PLANE0);
			vec4 planeClip = vec4(a, b, c, d);
			program.sendUniform("planeClip", planeClip);
			matrixView *=   matrixReflection;
			renderScene(matrixView, time, deltaTime);
		glDisable(GL_CLIP_PLANE0);

	glDisable(GL_STENCIL_TEST);

	//reset matrix back to normal
	matrixView *= matrixReflection;
}

//******* WATER, WATER DIMMER, RIVER BED RENDER and opacity set!
void PlanareReflectioOringBaseRender()
{
	program.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexPebbles);

	glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//TEXTURE RIVER BED
		mat4 m = matrixView;
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, idTexNormal);
		program.sendUniform("materialAmbient", vec3(1, 1, 1));
		program.sendUniform("materialDiffuse", vec3(1, 1, 1));
		m = translate(m, vec3(6, waterLevel - 9, 6));

		program.sendUniform("transpparancyLvl", 0.55f);

		waterUnderlayers.render(m);

		//JUST COLOR RIVER DIMMER
		glBindTexture(GL_TEXTURE_2D, 0);
		program.sendUniform("materialAmbient", vec3(0.0, 0.412, 0.580));
		program.sendUniform("materialDiffuse", vec3(0.0, 0.412, 0.580));
		m = matrixView;
		m = translate(m, vec3(0, waterLevel - 0.3, 0));
		program.sendUniform("transpparancyLvl", 0.6f);
		waterUnderlayers.render(m);

		//WATER RENDER
		programWater.use();
		m = matrixView;
		m = translate(m, vec3(0, waterLevel, 0));
		programWater.sendUniform("matrixModelView", m);
		water.render(m);

	glDisable(GL_BLEND);
	program.use();

}
 
 //********* PLAYER RENDER **********
void playerRender(mat4& matrixView, float time)
{
	//variables
	mat4 m;
	m = matrixView;
	std::vector<mat4> transformsidle;
	std::vector<mat4> transformRun;
	std::vector<mat4> finalTransforms;

	//smooth stop
	if (_startSlowDown && _vel.z > 0)
		_vel.z -= damping;
	else if (_startSlowDown && _vel.z <= 0)
	{
		_startSlowDown = false;
		_acc.z = _vel.z = 0;
	}

	//save anim to variables
	player.loadAnimations(&idle);
	player.getAnimData(0, time, transformsidle);
	player.loadAnimations(&run);
	player.getAnimData(0, time, transformRun);

	float f = smoothstep(0.0f, 10.0f, _vel.z);

	// Ensure the sizes match
	if (transformsidle.size() == transformRun.size())
	{
		// Blend transforms based on 'f'
		for (size_t i = 0; i < transformsidle.size(); i++)
		{
			mat4 m1 = transformsidle[i];
			mat4 m2 = transformRun[i];
			mat4 mixedTransform = (1.0f - f) * m1 + f * m2;

			// Store the mixed transform
			finalTransforms.push_back(mixedTransform);
		}

		// Send the final blended transforms to the shader
		program.sendUniform("bones", &finalTransforms[0], finalTransforms.size());
	}
	
	//player Position setup and render
	vec3 cameraPosition = vec3(inverse(matrixView)[3]);
	mat4 inverseView = inverse(matrixView);
	mat3 cameraRotation = mat3(inverseView);
	vec3 forwardDirection = vec3(inverse(matrixView)[2]);

	m = matrixView;
	m  = translate(m, cameraPosition - (forwardDirection * vec3(2.3, 2.2, 2.3))); // player pos based on camera pos - forward vector
	m = translate(m, vec3(0, -1.9f, 0));
	m = scale(m, vec3(0.012f, 0.012f, 0.012f));

	mat4 noYRotation = mat4
	(
		// Keep X and Z, zero out Y
		vec4(cameraRotation[0].x, 0, cameraRotation[0].z, 0), 
		vec4(0, 1, 0, 0), 
		vec4(cameraRotation[2].x, 0, cameraRotation[2].z, 0), 
		vec4(0, 0, 0, 1)
	);

	m *= noYRotation;

	// rotation
	m = rotate(m, radians(180.f), vec3(1, 0, 0)); // Rotate only X
	m = rotate(m, radians(180.f), vec3(0, 0, 1)); // Rotate only Z

	player.render(m);
}


//** POOOOOOSSSSTTT PROOOOOOCEEEESING (OFF BY DEFAUL PRESS 9 to activate -> deletes second sky box -> check report!!)
void PostProcesing()
{
	programEffect.use();
	glActiveTexture(GL_TEXTURE0);
	programEffect.sendUniform("texture0", 0);

	// Pass 2: on-screen rendering POST PROC
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
	programEffect.sendUniform("matrixProjection", ortho(0, 1, 0, 1, -1, 1));

	// clear screen and buffers & bind textures
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, idTexScreen);

	// setup identity matrix as the model-view
	programEffect.sendUniform("matrixModelView", mat4(1));

	GLuint attribVertex = programEffect.getAttribLocation("aVertex");
	GLuint attribTextCoord = programEffect.getAttribLocation("aTexCoord");
	glEnableVertexAttribArray(attribVertex);
	glEnableVertexAttribArray(attribTextCoord);
	glBindBuffer(GL_ARRAY_BUFFER, bufQuad);
	glVertexAttribPointer(attribVertex, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(attribTextCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableVertexAttribArray(attribVertex);
	glDisableVertexAttribArray(attribTextCoord);
	program.use();
}
 

//*********** RENDER GLUT ***********
void onRender()
{
	//these variables control time & animation
	static float prev = 0;
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;	// time since start in seconds
	float deltaTime = time - prev;						// time since last frame
	prev = time;		// framerate is 1/deltaTime
	programWater.sendUniform("t", time);
	glDisable(GL_CULL_FACE);

	// Pass 1: off-screen rendering POST PROC
	if(_togglePostProc)
		glBindFramebufferEXT(GL_FRAMEBUFFER, idFBOPostProc);

	//clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// setup the View Matrix (camera)
	_vel = clamp(_vel + _acc * deltaTime, -vec3(maxspeed), vec3(maxspeed));
	float pitch = getPitch(matrixView);
	matrixView = rotate(translate(rotate(mat4(1),
		pitch, vec3(1, 0, 0)),	// switch the pitch off
		_vel * deltaTime),		// animate camera motion (controlled by WASD keys)
		-pitch, vec3(1, 0, 0))	// switch the pitch on
		* matrixView;
 
	// move the camera up following the profile of terrain (Y coordinate of the terrain)
	float terrainY = -terrain.getInterpolatedHeight(inverse(matrixView)[3][0] , inverse(matrixView)[3][2]);
	matrixView = translate(matrixView, vec3(0, terrainY, 0));

	//program.sendUniform("matrixView", matrixView);
	SkyBoxAndDayCalculation();

	planarReflection(matrixView, time, deltaTime);

	// Enable stencil test for reflection
	glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glEnable(GL_CLIP_PLANE0);
			vec4 planeClip = vec4(a, b, c, d);
			program.sendUniform("planeClip", planeClip);
			program.sendUniform("matrixView", matrixView);
			renderScene(matrixView, time, deltaTime);
		glDisable(GL_CLIP_PLANE0);
	glDisable(GL_STENCIL_TEST);

	playerRender(matrixView, time);
	PlanareReflectioOringBaseRender();

	// the camera must be moved down by terrainY to avoid unwanted effects
	matrixView = translate(matrixView, vec3(0, -terrainY, 0));

	//POST PROCESING
	if(_togglePostProc)
		PostProcesing();

	//______________
	glutSwapBuffers(); 	// essential for double-buffering technique
	glutPostRedisplay();// proceed the animation

	//SHOW POS ON THE MAP!!!!!!!!!!!!!!!!!!!!!!
	//cout << inverse(matrixView)[3][0] << "       " << terrainY << "       " << inverse(matrixView)[3][2] << endl;
}



// ************* MAIN *************
int main(int argc, char** argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("3DGL Scene: First Terrain");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err) return 0;

	// register callbacks
	glutDisplayFunc(onRender);
	glutReshapeFunc(onReshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutMouseWheelFunc(onMouseWheel);

	if (!init()) return 0;
	glutMainLoop();	// enter GLUT event processing cycle
	return 1;
}