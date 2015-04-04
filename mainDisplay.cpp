#include "mainDisplay.h"
#include "Debug.h"




#define PI 3.14159265

int DisplayModule::windowId;

CaptureModule DisplayModule::kinect;

DisplayModule::States DisplayModule::state;

PONTO* DisplayModule::headPos;
PONTO* DisplayModule::leftHandPos;
PONTO* DisplayModule::rightHandPos;

Cube* DisplayModule::leftCube;
Cube* DisplayModule::rightCube;
Shape* DisplayModule::objects[3];
Shape* DisplayModule::mainObj;

double DisplayModule::lastContactAngle = 0.0;
float DisplayModule::ballSpeed = -1.0f;

bool DisplayModule::adjustPointOfView = true;
bool DisplayModule::debugMode = false;
bool DisplayModule::perspectiveMode = false;



CAMERA DisplayModule::camera;

float DisplayModule::zPos;



std::vector<DisplayParameter*> DisplayModule::acceptableParams;

std::map<std::string, float> DisplayModule::paramValues;


XYZ origin = {0.0,0.0,0.0};


std::vector<DisplayParameter*> DisplayModule::getAcceptableParams(){
	std::string metros = "metros";
	std::string pixels = "pixels";

	acceptableParams.push_back(new DisplayParameter(paramFocalDistance,  "distância entre o usuário e a tela", metros, true));
	acceptableParams.push_back(new DisplayParameter(paramWidth, "largura da janela", pixels, true));
	acceptableParams.push_back(new DisplayParameter(paramHeight, "altura da janela", pixels, true));
	acceptableParams.push_back(new DisplayParameter(paramTVWidth, "largura da tela", metros, true));
	acceptableParams.push_back(new DisplayParameter(paramTVHeight, "altura da tela", metros, true));
	acceptableParams.push_back(new DisplayParameter(paramEyeSeparation, "a separação entre os olhos", metros, true));
	acceptableParams.push_back(new DisplayParameter(paramObjectSize, "o tamanho que o objeto deve ter" , metros, true));
	acceptableParams.push_back(new DisplayParameter(paramZPos, "a distância que o objeto deve estar para fora da tela", metros, true));
	acceptableParams.push_back(new DisplayParameter(paramKinectToTV, "distância entre o Kinect e o centro da TV ou monitor", metros, true));
	acceptableParams.push_back(new DisplayParameter(paramFullscreen, "exibir em tela cheia", "", false));

	return acceptableParams;
}

bool DisplayModule::initializeDisplayModule(CaptureModule captureModule, std::map<std::string, float> params) {
	kinect = captureModule;

	paramValues = params;

	initObjects();

	int iArgc = 2; // must/should match the number of strings in argv
	char *cppArgv[] = {"foo", "bar"};
	glutInit(&iArgc, cppArgv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH | GLUT_STEREO);
	initCamera();
	 
	glutInitWindowSize(camera.screenwidth, camera.screenheight);
	glutInitWindowPosition(2000, 200);
	windowId = glutCreateWindow("Sterescopic Gesture Interactions");
	
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);							
	glClearColor(0.3f, 0.3f, 0.3f, 0.5f);					
	glClearDepth(1.0f);									
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);		  

	if(paramValues[paramFullscreen]){
		glutFullScreen();
	}

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	

	resizeGLScene(camera.screenwidth, camera.screenheight);

	glutDisplayFunc(updateObjsAndDraw);
	glutIdleFunc(updateObjsAndDraw);
	glutReshapeFunc(resizeGLScene);
	return true;
}

void DisplayModule::initObjects() {
	zPos = paramValues[paramZPos] ? paramValues[paramZPos] : 0.80f;

	headPos = new PONTO(0, 0, -4000);
	leftHandPos = new PONTO;
	rightHandPos = new PONTO;

	leftCube = new Cube(0.01f);
	rightCube = new Cube(0.01f);	

	float objSize = paramValues[paramObjectSize] ? paramValues[paramObjectSize] : 0.12f;

	objects[0] = new Cube(objSize);
	((Cube*)objects[0])->limitX = 0.12f;
	((Cube*)objects[0])->limitY = 0.05f;

	objects[1] = new Sphere(objSize);
	((Sphere*)objects[1])->limitX = 0.12f;
	((Sphere*)objects[1])->limitY = 0.05f;

	objects[2] = new Parallelepiped(3*objSize/4.0f, objSize, objSize/2.0f);
	((Parallelepiped*)objects[2])->limitX = 0.12f;
	((Parallelepiped*)objects[2])->limitY = 0.05f;

	mainObj = objects[0];
}

void DisplayModule::initCamera(){

	camera.screenwidth = (int) paramValues[paramWidth] ? paramValues[paramWidth] : 640;
	camera.screenheight = (int) paramValues[paramHeight] ? paramValues[paramHeight] : 360;

	camera.aperture = 50;
	camera.focallength = paramValues[paramFocalDistance] ? paramValues[paramFocalDistance] : 1.50f;
	camera.eyesep = paramValues[paramEyeSeparation] ? paramValues[paramEyeSeparation] : 0.062f;
	camera.pr = origin;

	camera.vp.x = 0;
	camera.vp.y = 0;
	camera.vp.z = 7;

	camera.vd.x = 0;
	camera.vd.y = 0;
	camera.vd.z = 0;

	camera.vu.x = 0;
	camera.vu.y = 1;
	camera.vu.z = 0;
}

void DisplayModule::resizeGLScene(GLsizei width, GLsizei height){
	if (height==0){
		height=1;										
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0,0,width,height);						// Reset The Current Viewport
	
	camera.screenwidth = width;
	camera.screenheight = height;
	
	
	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();				

	//gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f); //TODO: isso é inútil agora

	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
}

void DisplayModule::updateObjsAndDraw(){

	updateSkeletonJointsPositions();
	updateOrientationCubesPos();
	updateObjectPos();

	drawGLScene();

	glFlush();
	glutSwapBuffers();
}

void DisplayModule::updateSkeletonJointsPositions(){
	kinect.prepareSkeletonFrame();

	kinect.getJointPosition(NUI_SKELETON_POSITION_HEAD, headPos);
	kinect.getJointPosition(NUI_SKELETON_POSITION_HAND_RIGHT, rightHandPos);
	kinect.getJointPosition(NUI_SKELETON_POSITION_HAND_LEFT, leftHandPos);
}

void DisplayModule::updateOrientationCubesPos(){
	float horizontalFactor = /*0.20f/0.46f*/ 1;
	float verticalFactor = /*0.11f/0.26f*/ 1;

	updateOrientationCubePos(leftCube, leftHandPos);
	updateOrientationCubePos(rightCube, rightHandPos);

	/*
	leftCube->x = leftHandPos->x;
	leftCube->y = leftHandPos->y;
	leftCube->z = zPos;

	float m = (rightHandPos->x - headPos->x) / (rightHandPos->z - headPos->z);
	float cubeX = (/*rightHandPos->z 0 - headPos->z) * m + headPos->x;

	rightCube->x = rightHandPos->x /*cubeX;
	rightCube->y = rightHandPos->y;
	rightCube->z = zPos;*/
}

void DisplayModule::updateOrientationCubePos(Cube* cube, PONTO* point){

	BOOL a = false;

	float xm = (point->x - headPos->x) / (point->z - headPos->z);
	float cubeX = (/*rightHandPos->z*/ 0 - headPos->z) * xm + headPos->x;
	if(a)
		cube->x = cubeX;
	else
		cube->x = point->x;
	
	float ym = (point->y - headPos->y) / (point->z - headPos->z);
	float cubeY = (/*rightHandPos->z*/ 0 - headPos->z) * ym + headPos->y;
	if(a)
		cube->y = cubeY;
	else
		cube->y = point->y;
	
	cube->z = zPos;
}


void DisplayModule::updateObjectPos(){
	BOOL leftInvasion = mainObj->hasColision(leftCube);
	BOOL rightInvasion = mainObj->hasColision(rightCube);

	if(leftInvasion && rightInvasion){
		//ignore two hands gestures
	}else if(leftInvasion){
		state = HAND_CONTACT;
		mainObj->reactToPush(leftCube);
	}else if(rightInvasion){
		state = HAND_CONTACT;
		mainObj->reactToPush(rightCube);
	}else if(state != WAITING){
		state = MOVING;
	}
	if(state == MOVING){}
	mainObj->z = zPos;
}

void DisplayModule::drawGLScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	//ambos em metros
	float tvHalfWidth = paramValues[paramTVWidth] ? (paramValues[paramTVWidth] / 2.0f) : ((92.98f / 2.0f) / 100);
	float tvHalfHeight = paramValues[paramTVHeight] ? (paramValues[paramTVHeight] / 2.0f) : ((52.3f / 2.0f) / 100);
	float halfEyeDistance = camera.eyesep / 2.0f;

	float xmin =-/*6.0f*/ tvHalfWidth, xmax = /*6.0f*/ tvHalfWidth, 
		ymin =-/*4.8f*/tvHalfHeight,  ymax = /*4.8f*/ tvHalfHeight,
		znear = /*1.0f*/ camera.focallength - 0.01f, zfar = -0.0f, 
		zzps = 0.0f, dist = camera.focallength;

	/* Draw left subfield */
	glDrawBuffer(GL_BACK_LEFT);

	//glClearColor(0.07, 0.07, 0.07, 0.00);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	/* Z-coordinate of plane of zero parallax is 0.0. In that plane, the coord
		range drawn to the screen will be

		(-6.0 to 6.0, -4.8 to 4.8).

		Z-coordinate clipping planes are -6.0 and 6.0. The eyes are set at world
		coord distance 14.5 from the plane of zero parallax, and the eye
		separation is 0.62 in world coords. These two values were calculated
		using equations 11 to 15, and 17 to 19 in chapter 5. */

	calculateStereoProj(xmin, xmax, ymin, ymax, znear, zfar, zzps, dist, -halfEyeDistance);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//fixOrientationCubesPos();

	drawObjects();
	if(debugMode){
		printMatrix();
		prepareFPS();
		debugText(*leftHandPos, *rightHandPos, *headPos, *mainObj->getPos(), zPos, tvHalfWidth, tvHalfHeight, perspectiveMode);
	}
	glFlush();

	/* Draw right subfield */
	//  stereo_right_buffer();
	glDrawBuffer(GL_BACK_RIGHT);
	//  glStencilFunc(GL_EQUAL,1,1); // draws if stencil <> 0
 
	// we must clear twice now
	//glClearColor(0.07, 0.07, 0.07, 0.00);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	/* Same as above stereoproj() call, except that eye arg is positive */
	calculateStereoProj(xmin, xmax, ymin, ymax, znear, zfar, zzps, dist, halfEyeDistance);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glRotatef((float) position, 0.0, 1.0, 0.0);
	//glRotatef(-10.0, 1.0, 0.0, 0.0);
	//glColor3f(0,0,1); // right airplain blue
	drawObjects();
	if(debugMode)
		debugText(*leftHandPos, *rightHandPos, *headPos, *mainObj->getPos(), zPos, tvHalfWidth, tvHalfHeight, perspectiveMode);
	glFlush();

	//glutSwapBuffers();    /* Update screen */

}



/* This routine performs the perspective projection for one eye's subfield.
   The projection is in the direction of the negative z axis.

   xmin, ymax, ymin, ymax = the coordinate range, in the plane of zero
   parallax setting, that will be displayed on the screen. The ratio between
   (xmax-xmin) and (ymax-ymin) should equal the aspect ration of the display.

   znear, zfar = the z-coordinate values of the clipping planes.

   zzps = the z-coordinate of the plane of zero parallax setting.

   dist = the distance from the center of projection to the plane of zero
   parallax.

   eye = half the eye separation; positive for the right eye subfield,
   negative for the left eye subfield. */
void DisplayModule::calculateStereoProj(float xmin, float xmax, float ymin, float ymax,
  float znear, float zfar, float zzps, float dist, float eyeSep) {

  float xmid, ymid, clip_near, clip_far, top, bottom, left, right, dx, dy, n_over_d;

  dx = xmax - xmin;
  dy = ymax - ymin;

  xmid = (xmax + xmin) / 2.0f;
  ymid = (ymax + ymin) / 2.0f;

  clip_near = dist + zzps - znear;
  clip_far = dist + zzps - zfar;

  n_over_d = clip_near / dist;

  top = n_over_d * dy / 2.0f;
  bottom = -top;
  right = n_over_d * (dx / 2.0f - eyeSep);
  left = n_over_d * (-dx / 2.0f - eyeSep);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if(!perspectiveMode){
	int adjust = 65;
	glOrtho(left * adjust, right * adjust, bottom * adjust , top * adjust, clip_near, clip_far);
  }else{
	glFrustum(left, right, bottom, top, clip_near, clip_far);
  }
 
  //glTranslatef(-xmid - eye, -ymid, -zzps - dist);
  if(adjustPointOfView){
	  gluLookAt(-xmid + eyeSep + headPos->x, -ymid + headPos->y, -(-zzps - dist),
				-xmid + eyeSep, -ymid, zPos,//tanto faz o Z da direção: x e y estão definidos
				0, 1, 0);
  } else {
	  gluLookAt(-xmid + eyeSep             , -ymid             , -(-zzps - dist),
				-xmid + eyeSep, -ymid, zPos,//tanto faz o Z da direção: x e y estão definidos
				0, 1, 0);
  }

  glGetFloatv (GL_PROJECTION_MATRIX, modelMatrix);
}



GLfloat* invertMatrix(GLfloat* matrix){
	GLfloat a[5][5];
	a[1][1] = matrix[0];
	a[1][2] = matrix[1];
	a[1][3] = matrix[2];
	a[1][4] = matrix[3];

	a[2][1] = matrix[4];
	a[2][2] = matrix[5];
	a[2][3] = matrix[6];
	a[2][4] = matrix[7];

	a[3][1] = matrix[8];
	a[3][2] = matrix[9];
	a[3][3] = matrix[10];
	a[3][4] = matrix[11];

	a[4][1] = matrix[12];
	a[4][2] = matrix[13];
	a[4][3] = matrix[14];
	a[4][4] = matrix[15];

	GLfloat b[5][5];
	b[1][1] = a[2][2]*a[3][3]*a[4][4] + a[2][3]*a[3][4]*a[4][2] + a[2][4]*a[3][2]*a[4][3] - a[2][2]*a[3][4]*a[4][3] - a[2][3]*a[3][2]*a[4][4] - a[2][4]*a[3][3]*a[4][2];
	b[1][2] = a[1][2]*a[3][4]*a[4][3] + a[1][3]*a[3][2]*a[4][4] + a[1][4]*a[3][3]*a[4][2] - a[1][2]*a[3][3]*a[4][4] - a[1][3]*a[3][4]*a[4][2] - a[1][4]*a[3][2]*a[4][3];
	b[1][3] = a[1][2]*a[2][3]*a[4][4] + a[1][3]*a[2][4]*a[4][2] + a[1][4]*a[2][2]*a[4][3] - a[1][2]*a[2][4]*a[4][3] - a[1][3]*a[2][2]*a[4][4] - a[1][4]*a[2][3]*a[4][2];
	b[1][4] = a[1][2]*a[2][4]*a[3][3] + a[1][3]*a[2][2]*a[3][4] + a[1][4]*a[2][3]*a[3][2] - a[1][2]*a[2][3]*a[3][4] - a[1][3]*a[2][4]*a[3][2] - a[1][4]*a[2][2]*a[3][3];
	b[2][1] = a[2][1]*a[3][4]*a[4][3] + a[2][3]*a[3][1]*a[4][4] + a[2][4]*a[3][3]*a[4][1] - a[2][1]*a[3][3]*a[4][4] - a[2][3]*a[3][4]*a[4][1] - a[2][4]*a[3][1]*a[4][3];
	b[2][2] = a[1][1]*a[3][3]*a[4][4] + a[1][3]*a[3][4]*a[4][1] + a[1][4]*a[3][1]*a[4][3] - a[1][1]*a[3][4]*a[4][3] - a[1][3]*a[3][1]*a[4][4] - a[1][4]*a[3][3]*a[4][1];
	b[2][3] = a[1][1]*a[2][4]*a[4][3] + a[1][3]*a[2][1]*a[4][4] + a[1][4]*a[2][3]*a[4][1] - a[1][1]*a[2][3]*a[4][4] - a[1][3]*a[2][4]*a[4][1] - a[1][4]*a[2][1]*a[4][3];
	b[2][4] = a[1][1]*a[2][3]*a[3][4] + a[1][3]*a[2][4]*a[3][1] + a[1][4]*a[2][1]*a[3][3] - a[1][1]*a[2][4]*a[3][3] - a[1][3]*a[2][1]*a[3][4] - a[1][4]*a[2][3]*a[3][1];
	b[3][1] = a[2][1]*a[3][2]*a[4][4] + a[2][2]*a[3][4]*a[4][1] + a[2][4]*a[3][1]*a[4][2] - a[2][1]*a[3][4]*a[4][2] - a[2][2]*a[3][1]*a[4][4] - a[2][4]*a[3][2]*a[4][1];
	b[3][2] = a[1][1]*a[3][4]*a[4][2] + a[1][2]*a[3][1]*a[4][4] + a[1][4]*a[3][2]*a[4][1] - a[1][1]*a[3][2]*a[4][4] - a[1][2]*a[3][4]*a[4][1] - a[1][4]*a[3][1]*a[4][2];
	b[3][3] = a[1][1]*a[2][2]*a[4][4] + a[1][2]*a[2][4]*a[4][1] + a[1][4]*a[2][1]*a[4][2] - a[1][1]*a[2][4]*a[4][2] - a[1][2]*a[2][1]*a[4][4] - a[1][4]*a[2][2]*a[4][1];
	b[3][4] = a[1][1]*a[2][4]*a[3][2] + a[1][2]*a[2][1]*a[3][4] + a[1][4]*a[2][2]*a[3][1] - a[1][1]*a[2][2]*a[3][4] - a[1][2]*a[2][4]*a[3][1] - a[1][4]*a[2][1]*a[3][2];
	b[4][1] = a[2][1]*a[3][3]*a[4][2] + a[2][2]*a[3][1]*a[4][3] + a[2][3]*a[3][2]*a[4][1] - a[2][1]*a[3][2]*a[4][3] - a[2][2]*a[3][3]*a[4][1] - a[2][3]*a[3][1]*a[4][2];
	b[4][2] = a[1][1]*a[3][2]*a[4][3] + a[1][2]*a[3][3]*a[4][1] + a[1][3]*a[3][1]*a[4][2] - a[1][1]*a[3][3]*a[4][2] - a[1][2]*a[3][1]*a[4][3] - a[1][3]*a[3][2]*a[4][1];
	b[4][3] = a[1][1]*a[2][3]*a[4][2] + a[1][2]*a[2][1]*a[4][3] + a[1][3]*a[2][2]*a[4][1] - a[1][1]*a[2][2]*a[4][3] - a[1][2]*a[2][3]*a[4][1] - a[1][3]*a[2][1]*a[4][2];
	b[4][4] = a[1][1]*a[2][2]*a[3][3] + a[1][2]*a[2][3]*a[3][1] + a[1][3]*a[2][1]*a[3][2] - a[1][1]*a[2][3]*a[3][2] - a[1][2]*a[2][1]*a[3][3] - a[1][3]*a[2][2]*a[3][1];

	float detA = 
		  a[1][1]*a[2][2]*a[3][3]*a[4][4] + a[1][1]*a[2][3]*a[3][4]*a[4][2] + a[1][1]*a[2][4]*a[3][2]*a[4][3]
		+ a[1][2]*a[2][1]*a[3][4]*a[4][3] + a[1][2]*a[2][3]*a[3][1]*a[4][4] + a[1][2]*a[2][4]*a[3][3]*a[4][1]
		+ a[1][3]*a[2][1]*a[3][2]*a[4][4] + a[1][3]*a[2][2]*a[3][4]*a[4][1] + a[1][3]*a[2][4]*a[3][1]*a[4][2]
		+ a[1][4]*a[2][1]*a[3][3]*a[4][2] + a[1][4]*a[2][2]*a[3][1]*a[4][3] + a[1][4]*a[2][3]*a[3][2]*a[4][1]
		- a[1][1]*a[2][2]*a[3][4]*a[4][3] - a[1][1]*a[2][3]*a[3][2]*a[4][4] - a[1][1]*a[2][4]*a[3][3]*a[4][2]
		- a[1][2]*a[2][1]*a[3][3]*a[4][4] - a[1][2]*a[2][3]*a[3][4]*a[4][1] - a[1][2]*a[2][4]*a[3][1]*a[4][3]
		- a[1][3]*a[2][1]*a[3][4]*a[4][2] - a[1][3]*a[2][2]*a[3][1]*a[4][4] - a[1][3]*a[2][4]*a[3][2]*a[4][1]
		- a[1][4]*a[2][1]*a[3][2]*a[4][3] - a[1][4]*a[2][2]*a[3][3]*a[4][1] - a[1][4]*a[2][3]*a[3][1]*a[4][2];

	float invertedDetA = 1.0f / detA;

	for(int i = 0; i < 5; i++){
		for(int j = 0; j < 5; j++){
			b[i][j] = b[i][j] * invertedDetA;
		}
	}

	GLfloat* result = (GLfloat*) malloc(sizeof(GLfloat) * 16);
	result[0] = b[1][1];
	result[1] = b[1][2];
	result[2] = b[1][3];
	result[3] = b[1][4];

	result[4] = b[2][1];
	result[5] = b[2][2];
	result[6] = b[2][3];
	result[7] = b[2][4];

	result[8] = b[3][1];
	result[9] = b[3][2];
	result[10] = b[3][3];
	result[11] = b[3][4];

	result[12] = b[4][1];
	result[13] = b[4][2];
	result[14] = b[4][3];
	result[15] = b[4][4];

	return result;
}

PONTO* multiplyInvertedMatrix(PONTO* cubePos){
	PONTO* result = new PONTO;

	float p[4] = {cubePos->x, cubePos->y, cubePos->z, 1};
	GLfloat* m = invertMatrix(modelMatrix);

	result->x = m[0]*p[0] + m[1]*p[1] + m[2]*p[2] + m[3]*p[3];
	result->y = m[4]*p[0] + m[5]*p[1] + m[6]*p[2] + m[7]*p[3];
	result->z = m[8]*p[0] + m[9]*p[1] + m[10]*p[2] + m[11]*p[3];

	return result;
}

/*
void DisplayModule::fixOrientationCubesPos(){
	PONTO* leftNewPos = multiplyInvertedMatrix(leftCube->getPos());
	//leftCube->move(leftNewPos->x, leftNewPos->y, leftNewPos->z);
	leftCube->move(leftNewPos->x, leftNewPos->y, leftCube->getPos()->z);
}*/



void DisplayModule::drawObjects(){
	glLoadIdentity(); 

	if(debugMode){
	
		glColor3f(1, 1, 1);
		glBegin(GL_LINES);
			glVertex3f(0.0f,-60.0f, 0.0f);
			glVertex3f(0.0f, 60.0f, 0.0f);
			glVertex3f(60.0f, 0.0f, 0.0f);
			glVertex3f(-60.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, -0.9f);
			glVertex3f(0.0f, 0.0f, 0.9f);

			//glVertex3f(0.1f, 0.1f, -0.9f);
			//glVertex3f(0.1f, 0.1f, 0.9f);

		glEnd();

		glBegin(GL_LINES);
			//da cabeça ao cubo
			/*glVertex3f(leftCube->x, leftCube->y, 0);
			glVertex3f(headPos->x, headPos->y, 0);

			glVertex3f(rightCube->x, rightCube->y, 0);
			glVertex3f(headPos->x, headPos->y, 0);*/

			//linha de toque com perspectiva
			bool correge = false;

			if(!correge){
				glVertex3f(leftCube->x, leftCube->y, -0.2f);
				glVertex3f(leftCube->x, leftCube->y, zPos);

				glVertex3f(rightCube->x, rightCube->y, -0.2f);
				glVertex3f(rightCube->x, rightCube->y, zPos);
			}else{
				//linha de toque com perspectiva corrigida
				glVertex3f(leftHandPos->x, leftHandPos->y, -0.0f);
				glVertex3f(leftCube->x * 0.20f/0.46f, leftCube->y * 0.11f/0.26f, zPos);

				glVertex3f(rightHandPos->x, rightHandPos->y, -0.0f);
				glVertex3f(rightCube->x * 0.20f/0.46f, rightCube->y * 0.11f/0.26f, zPos);
			}
		glEnd();

		leftCube->draw();
		rightCube->draw();
	}

	mainObj->draw();
}

void DisplayModule::changeObject(int index){
	int sizeOfList = sizeof(objects)/sizeof(objects[0]);

	if(index > 0 && index <= sizeOfList){
		mainObj = objects[index - 1];
	}
}

void DisplayModule::resetObjectsPos() {
	int sizeOfList = sizeof(objects)/sizeof(objects[0]);

	for(int i = 0; i < sizeOfList; i++){
		objects[i]->move(0.0f, 0.0f, 0.0f);
	}
}

void DisplayModule::destroyWindow(){
	glutDestroyWindow(windowId);
}

/*	b[1][1] = a22*a33*a44 + a23*a34*a42 + a24a32a43 - a22a34a43 - a23a32a44 - a24a33a42;
	b[1][2] = a12a34a43 + a13a32a44 + a14a33a42 - a12a33a44 - a13a34a42 - a14a32a43;
	b[1][3] = a12a23a44 a13a24a42 a14a22a43 a12a24a43 a13a22a44 a14a23a42;
	b[1][4] = a12a24a33 a13a22a34 a14a23a32 a12a23a34 a13a24a32 a14a22a33;
	b[2][1] = a21a34a43 a23a31a44 a24a33a41 a21a33a44 a23a34a41 a24a31a43;
	b[2][2] = a11a33a44 a13a34a41 a14a31a43 a11a34a43 a13a31a44 a14a33a41;
	b[2][3] = a11a24a43 a13a21a44 a14a23a41 a11a23a44 a13a24a41 a14a21a43;
	b[2][4] = a11a23a34 a13a24a31 a14a21a33 a11a24a33 a13a21a34 a14a23a31;
	b[3][1] = a21a32a44 a22a34a41 a24a31a42 a21a34a42 a22a31a44 a24a32a41;
	b[3][2] = a11a34a42 a12a31a44 a14a32a41 a11a32a44 a12a34a41 a14a31a42;
	b[3][3] = a11a22a44 a12a24a41 a14a21a42 a11a24a42 a12a21a44 a14a22a41;
	b[3][4] = a11a24a32 a12a21a34 a14a22a31 a11a22a34 a12a24a31 a14a21a32;
	b[4][1] = a21a33a42 a22a31a43 a23a32a41 a21a32a43 a22a33a41 a23a31a42;
	b[4][2] = a11a32a43 a12a33a41 a13a31a42 a11a33a42 a12a31a43 a13a32a41;
	b[4][3] = a11a23a42 a12a21a43 a13a22a41 a11a22a43 a12a23a41 a13a21a42;
	b[4][4] = a11a22a33 a12a23a31 a13a21a32 a11a23a32 a12a21a33 a13a22a31;*/