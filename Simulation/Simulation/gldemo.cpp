
#include "stdafx.h"
#include <glew\glew.h>
#include <glew\wglew.h>
#include <freeglut\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include <CGImport\CGModel\CGModel.h>
#include <CGImport\Importers\CGImporters.h>
#include "helper_functions.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"
#include <random>
#include <time.h>


using namespace std;
using namespace CoreStructures;

// Global Variables - Bad practice, but un-avoidable with the use of freeglut. Cannot pass arguments between vital functions used with freeglut. 
struct Data
{
	
	const int VERT_NUM = 1089;
	const int IND_NUM = 6144;

	const int height = 32; const int width = 32;

	GUVector4 terrFlatVertices[1089];
	GUVector4 terrFlatColours[1089];
	unsigned short terrFlatIndices[6144];

#pragma region Animation Variables

	bool isPause, reverseMotion;
	int mode;

	bool store1, store2;
	float randomNumberTmp, randomNumberTmp2; // used for store bool.
	float numberChange; // floating point number that will be changed on update and sent to the GPU. The number will be different on each update, so can be used for animation. 
			
	float randomNumber, randomNumber2; // These random numbers will be passed to provide random vertice movement in mode 4.

#pragma endregion 

#pragma region Scene variables and resources

	// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in

	int	mouse_x, mouse_y;
	bool mDown = false;

#pragma region Terrain Buffer Object Variables

	// Terrain Flat VBO declaration

	GLuint terrFlatVertBuff, terrFlatColBuff, terrFlatIndBuff, terrFlatVAO; // Terrain Buffer and Array Object Variables
	GLuint	terrShad; // Terrain shader declaration

#pragma endregion

#pragma region Camera Uniform Buffer Object Variables

	// UBO model for CameraMatrixBlock
	GUPivotCamera *mainCamera = nullptr;

	gu_byte	*cameraBuffer; // system memory buffer
	GLint cameraBufferSize;
	GLuint cameraUBO; // ID of buffer on GPU
	GLint CameraMatrixBlock_offsets[2]; // offset of start of uniform data
	GLint CameraMatrixBlock_matrixStrides[2]; // mat4 column stride (column-major default)
	GLuint terrainCameraBlockIndex; // store uniform block indices for each shader that uses the block

#pragma endregion

#pragma endregion

};

#pragma region Unavoidable Globals

GUClock	*mainClock = nullptr;
Data sceneData;

random_device rd;
uniform_real_distribution<float> rHeight(0.0f, 5.0f);
uniform_real_distribution<float> rColour(0.0f, 1.0f);

mt19937 mt(rd());

#pragma endregion

#pragma region Function Prototypes

void init(void); // Main scene initialisation function
void update(void); // Main scene update function
void display(void); // Main scene render function

// Event handling functions
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void mouseWheel(int wheel, int direction, int x, int y);
void keyDown(unsigned char key, int x, int y);
void closeWindow(void);

#pragma endregion



int _tmain(int argc, char* argv[]) {

	// Initialise GL Utility Toolkit and initialise COM so we can use the Windows Imaging Component (WIC) library
	glutInit(&argc, argv);
	CoInitialize(NULL);

	// Setup the OpenGL environment and initialise scene variables and resources
	init();

	// Setup and start the main clock
	mainClock = new GUClock();

	// Main event / render loop
	glutMainLoop();

	// Stop clock and report final timing data
	if (mainClock) {

		mainClock->stop();
		mainClock->reportTimingData();
		mainClock->release();

	}
	
	// Shut down COM and exit
	CoUninitialize();
	return 0;
}

void reportInstructions()
{
	// Print application instructions to the command prompt window opened alongside the main window.

	cout << "\n\n\n\n";
	cout << "\t\t\t\t|| Press button 1-6 to change the simulation mode	||" << endl;
	cout << "\t\t\t\t|| Press P to Pause and Reset				||" << endl;
	cout << "\t\t\t\t|| Press X to Erratically change scene 4		||" << endl;
	cout << "\n\n\n\n";

}

#pragma region Initialisation, Update and Render

void init(void) {

	// Request an OpenGL 4.5 context with the Compatibility profile
	glutInitContextVersion(4, 5);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	// Setup OpenGL Display mode - include MSAA x4
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 4);

	// Setup window
	int windowWidth = 1920;
	int windowHeight = 1080;
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("Real-Time Rendering Techniques");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Register callback functions
	glutIdleFunc(update); // Main scene update function
	glutDisplayFunc(display); // Main render function
	glutKeyboardFunc(keyDown); // Key down handler
	glutMouseFunc(mouseButtonDown); // Mouse button handler
	glutMotionFunc(mouseMove); // Mouse move handler
	glutMouseWheelFunc(mouseWheel); // Mouse wheel event handler
	glutCloseFunc(closeWindow); // Main resource cleanup handler

	// Initialise GLEW library
	GLenum err = glewInit();

	// Ensure GLEW was initialised successfully before proceeding
	if (err == GLEW_OK) {

		cout << "GLEW initialised okay\n";

	} else {

		cout << "GLEW could not be initialised\n";
		throw;
	}
	
	// Print app instructions to console.
	reportInstructions();
	
	// Initiaise scene resources and variables


	// Initialise OpenGL
	wglSwapIntervalEXT(0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	// Setup main camera
	float viewportAspect = (float)windowWidth / (float)windowHeight;
	sceneData.mainCamera = new GUPivotCamera(0.0f, 10.0f, 15.0f, 55.0f, viewportAspect, 0.1f);

	sceneData.mode = 1; // Default mode on start up is 1
	
	sceneData.store1 = false;
	sceneData.store2 = false;

	sceneData.isPause = false;
	sceneData.reverseMotion = false;

	sceneData.randomNumber2 = rColour(mt);
	//Set up vertex points for the terrain lattice.

	for (int z = 0; z <= sceneData.height; z++)
	{
		
		for (int x = 0; x <= sceneData.width; x++)
		{
			
			int fX = ((z*sceneData.width + x) + z);

			sceneData.terrFlatVertices[fX].x = x - (sceneData.width / 2); // Divide 2 added to make grid central
			sceneData.terrFlatVertices[fX].y = 0.0f;
			sceneData.terrFlatVertices[fX].z = z -(sceneData.height / 2); // Divide 2 added to make grid central
			sceneData.terrFlatVertices[fX].w = 1.0f;
			
		}
	}

	//set up the lattice indices to be used by the draw function.

	for (int z = 0; z < sceneData.height; z++)
	{
		for (int x = 0; x < sceneData.width; x++)
		{
			int vertIndex = ((z * sceneData.width +  x)+z);
			int fX = ((z * sceneData.width) + x) * 6;
			sceneData.terrFlatIndices[fX] = vertIndex;
			sceneData.terrFlatIndices[fX + 1] = vertIndex + 33;
			sceneData.terrFlatIndices[fX + 2] = vertIndex + 34;
			sceneData.terrFlatIndices[fX + 3] = vertIndex;
			sceneData.terrFlatIndices[fX + 4] = vertIndex + 34;
			sceneData.terrFlatIndices[fX + 5] = vertIndex + 1;
		}


	}

#pragma region Terrain VAO and VBO set up

	// setup VAO for terrain object
	glGenVertexArrays(1, &sceneData.terrFlatVAO);
	glBindVertexArray(sceneData.terrFlatVAO);

	// setup VBO for vertex position data
	glGenBuffers(1, &sceneData.terrFlatVertBuff);
	glBindBuffer(GL_ARRAY_BUFFER, sceneData.terrFlatVertBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sceneData.terrFlatVertices), sceneData.terrFlatVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	// setup VBO for vertex colour data
	glGenBuffers(1, &sceneData.terrFlatColBuff);
	glBindBuffer(GL_ARRAY_BUFFER, sceneData.terrFlatColBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sceneData.terrFlatColours), sceneData.terrFlatColours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	// setup VBO for terrain face index array
	glGenBuffers(1, &sceneData.terrFlatIndBuff);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sceneData.terrFlatIndBuff);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sceneData.terrFlatIndices), sceneData.terrFlatIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);

#pragma endregion

	err = ShaderLoader::createShaderProgram(string("Resources\\Shaders\\terrain_vert.vs"), string("Resources\\Shaders\\terrain_frag.fs"), &sceneData.terrShad);


	// UBO setup code for CameraMatrixBlock uniform block

	sceneData.cameraUBO = 0;

	// Get index of uniform block (blocks have indices just like normal uniform variables as discussed in previous lectures)
	sceneData.terrainCameraBlockIndex = glGetUniformBlockIndex(sceneData.terrShad, "CameraMatrixBlock");

	// get actual size of block in bytes (necessary to allocate a buffer of the same size in system memory using malloc)
	glGetActiveUniformBlockiv(sceneData.terrShad, sceneData.terrainCameraBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &sceneData.cameraBufferSize);

	sceneData.cameraBuffer = (gu_byte*)calloc(sceneData.cameraBufferSize, 1);

	if (sceneData.cameraBuffer) {

		// create a new UBO object to represent the camera data block
		glGenBuffers(1, &sceneData.cameraUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, sceneData.cameraUBO);

		// Set up the camera buffer objects structure members names for uniform index retrival 
		const GLchar* CameraMatrixBlock_names[2] = {
			"CameraMatrixBlock.viewMatrix",
			"CameraMatrixBlock.projectionMatrix"
		};

		GLuint CameraMatrixBlock_indices[2] = { 0, 0 };

		// Retrieve uniform indices to make use for 
		glGetUniformIndices(
			sceneData.terrShad,
			2,
			CameraMatrixBlock_names,
			CameraMatrixBlock_indices);

		// get structure data for CameraMatrixBlock in the terrain shader
		glGetActiveUniformsiv(sceneData.terrShad, 2, CameraMatrixBlock_indices, GL_UNIFORM_OFFSET, sceneData.CameraMatrixBlock_offsets);
		glGetActiveUniformsiv(sceneData.terrShad, 2, CameraMatrixBlock_indices, GL_UNIFORM_MATRIX_STRIDE, sceneData.CameraMatrixBlock_matrixStrides);


		// Bind cameraUBO to binding point 0...
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, sceneData.cameraUBO);

		// Bind the CameraMatrixBlock in the terrain shader to binding point 0 (so the data we give the cameraUBO object will be picked up in the CameraMatrixBlock in the shader)
		glUniformBlockBinding(sceneData.terrShad, sceneData.terrainCameraBlockIndex, 0);

		

		
	}
}


// Main scene update function (called by FreeGLUT's main event loop every frame) 
void update(void) {

	// Update clock
	mainClock->tick();

	// Redraw the screen
	display();

	// Update the window title to show current frames-per-second and seconds-per-frame data
	char timingString[256];
	sprintf_s(timingString, 256, "Real-Time Rendering Demo. Average fps: %.0f; Average spf: %f", mainClock->averageFPS(), mainClock->averageSPF() / 1000.0f);
	glutSetWindowTitle(timingString);
}

void drawSurface()
{
	glUseProgram(sceneData.terrShad);
	
	switch (sceneData.mode)
	{
	case 1: // Mode 1 has no animation or randomisation, so no uniform variables need changing.

		break;

	case 2:

		if (sceneData.numberChange < 150 && sceneData.reverseMotion == false)
		{
			sceneData.numberChange += 0.001;
		}
		else
		{
			sceneData.reverseMotion = true;
			sceneData.numberChange -= 0.001;
		}

		if (sceneData.numberChange > 0 && sceneData.reverseMotion == true)
		{
			sceneData.numberChange -= 0.001;
		}
		else
		{
			sceneData.reverseMotion = false;
		}
		break;

	case 3:

		break;

	case 4:

		sceneData.randomNumberTmp = rHeight(mt);
		if (sceneData.store1 == false)
		{
			sceneData.randomNumber = sceneData.randomNumberTmp;
			sceneData.store1 = true;
		}
		sceneData.randomNumberTmp2 = rColour(mt);
		if (sceneData.store2 == false)
		{
			sceneData.randomNumber2 = sceneData.randomNumberTmp2;
			sceneData.store2 = true;
		}
		sceneData.numberChange += 0.001;
		break;

	case 5:

		sceneData.numberChange += 0.025f;
		break;

	case 6:

		if (sceneData.numberChange < 50 && sceneData.reverseMotion == false)
		{
			sceneData.numberChange += 0.001;
		}
		else
		{
			sceneData.reverseMotion = true;
			sceneData.numberChange -= 0.001;
		}

		if (sceneData.numberChange > 0 && sceneData.reverseMotion == true)
		{
			sceneData.numberChange -= 0.001;
		}
		else
		{
			sceneData.reverseMotion = false;
		}

		break;

	default:
		throw;

	}
	if (sceneData.isPause)
	{
		sceneData.numberChange = 0;
	}

	//Set uniform variables to be used by the shader.


	// Set mode number in shader.
	static GLint modeSettingLoc = glGetUniformLocation(sceneData.terrShad, "mode");
	glUniform1i(modeSettingLoc, sceneData.mode);

	// Set the first random number in shader. 
	static GLint randNumLoc = glGetUniformLocation(sceneData.terrShad, "randomNumber");
	glUniform1f(randNumLoc, sceneData.randomNumber);

	// Set the second random number in shader. 
	static GLint randNum2Loc = glGetUniformLocation(sceneData.terrShad, "randomNumber2");
	glUniform1f(randNum2Loc, sceneData.randomNumber2);

	// Set the changing number for animation in the shader. 
	static GLint numberPassLoc = glGetUniformLocation(sceneData.terrShad, "numberPass");
	glUniform1f(numberPassLoc, sceneData.numberChange);

	

	glBindVertexArray(sceneData.terrFlatVAO);

	//Draw the vertices contained in the terrFlat Vertex Array, using the GL_TRIANGLES draw mode.
	glDrawElements(GL_TRIANGLES, sceneData.IND_NUM, GL_UNSIGNED_SHORT, (const GLvoid*)0);
	glBindVertexArray(0);

	

}

// Main scene rendering function
void display(void) {

	// Clear the screen
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport to the client area of the current window
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	
	GUMatrix4 cameraViewMatrix = sceneData.mainCamera->viewTransform();
	GUMatrix4 cameraProjMatrix = sceneData.mainCamera->projectionTransform();

	GUMatrix4 viewProjectionMatrix = cameraProjMatrix * cameraViewMatrix;


	

	// Setup the camera buffer with the latest camera transformation matrices
	glBindBuffer(GL_UNIFORM_BUFFER, sceneData.cameraUBO);

	memcpy(sceneData.cameraBuffer + sceneData.CameraMatrixBlock_offsets[0], &(cameraViewMatrix), 0x40);
	memcpy(sceneData.cameraBuffer + sceneData.CameraMatrixBlock_offsets[1], &(cameraProjMatrix), 0x40);

	glBufferData(GL_UNIFORM_BUFFER, sceneData.cameraBufferSize, sceneData.cameraBuffer, GL_DYNAMIC_DRAW);


	drawSurface();


	glutSwapBuffers();
}

#pragma endregion



#pragma region Event handling functions

void mouseButtonDown(int button_id, int state, int x, int y) {

	if (button_id == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {

			sceneData.mouse_x = x;
			sceneData.mouse_y = y;

			sceneData.mDown = true;

		}
		else if (state == GLUT_UP) {

			sceneData.mDown = false;
		}
	}
}


void mouseMove(int x, int y) {

	int dx = x - sceneData.mouse_x;
	int dy = y - sceneData.mouse_y;

	if (sceneData.mainCamera)
		sceneData.mainCamera->transformCamera((float)-dy, (float)-dx, 0.0f);
		
	sceneData.mouse_x = x;
	sceneData.mouse_y = y;
}


void mouseWheel(int wheel, int direction, int x, int y) 
{

	if (sceneData.mainCamera) {

		if (direction<0)
			sceneData.mainCamera->scaleCameraRadius(1.1f);
		else if (direction>0)
			sceneData.mainCamera->scaleCameraRadius(0.9f);
	}
}


void keyDown(unsigned char key, int x, int y) // Handle key press
{


	switch (key)
	{
	case 'f':
		glutFullScreenToggle();
		break;

	case '1':
		sceneData.mode = 1;
		sceneData.numberChange = 0; // Animating number changed back to zero to effectively restart the scene when mode is changed.
		break;

	case '2':
		sceneData.mode = 2;
		sceneData.numberChange = 0;
		break;

	case '3':
		sceneData.mode = 3;
		sceneData.numberChange = 0;
		break;

	case '4':
		sceneData.mode = 4;
		sceneData.numberChange = 0;
		break;

	case '5':
		sceneData.mode = 5;
		sceneData.numberChange = 0;
		break;

	case '6':
		sceneData.mode = 6;
		sceneData.numberChange = 0;
		break;

	case 'p':
		if (sceneData.isPause)
		{
			sceneData.isPause = false;
		}
		else
		{
			sceneData.isPause = true;
		}
		break;

	case 'x':
		sceneData.store1 = false;
		sceneData.store2 = false;
		break;

	default:
		break;
	}

}


void closeWindow(void) {

	// Clean-up scene resources

	if (sceneData.mainCamera)
		sceneData.mainCamera->release();

}


#pragma endregion
