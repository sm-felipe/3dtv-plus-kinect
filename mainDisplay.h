#include "MainCapture.h"
#include <GL/glut.h>
#include <map>
#include <vector>
#include <string>
#include "DisplayParameter.h"


typedef struct {
   double x,y,z;
} XYZ;

typedef struct {
   XYZ vp;              /* View position           */
   XYZ vd;              /* View direction vector   */
   XYZ vu;              /* View up direction       */
   XYZ pr;              /* Point to rotate about   */
   float focallength;  /* Focal Length along vd   */
   float aperture;     /* Camera aperture         */
   float eyesep;       /* Eye separation          */
   int screenwidth,screenheight;
} CAMERA;


static const char* paramFocalDistance = "-focalDist";
static const char* paramWidth = "-w";
static const char* paramHeight = "-h";
static const char* paramTVWidth = "-tvW";
static const char* paramTVHeight = "-tvH";
static const char* paramEyeSeparation = "-eyeDist";
static const char* paramObjectSize = "-objSize";
static const char* paramZPos = "-zPos";
static const char* paramKinectToTV = "-kinectToTV";
static const char* paramFullscreen = "-f";

class DisplayModule {
public:

	static bool initializeDisplayModule(CaptureModule kinect, std::map<std::string, float> params);

	static float zPos;

	static bool adjustPointOfView;

	static bool debugMode;

	static bool perspectiveMode;

	static std::vector<DisplayParameter*> getAcceptableParams();

	static void changeObject(int index);

	static void resetObjectsPos();

	static void destroyWindow();

private:

	static int windowId;

	static CAMERA camera;

	static std::vector<DisplayParameter*> acceptableParams;

	static std::map<std::string, float> paramValues;

	static CaptureModule kinect;

	//controle do jogo
	enum States	{WAITING, HAND_CONTACT,	MOVING};
	static States state;
	static double lastContactAngle;
	static float ballSpeed;
	
	static PONTO* headPos;
	static PONTO* leftHandPos;
	static PONTO* rightHandPos;
	
	static Cube* leftCube;
	static Cube* rightCube;	
	static Shape* objects[3];
	static Shape* mainObj;



	static void initObjects();

	static void initCamera();

	static void resizeGLScene(GLsizei width, GLsizei height);

	static void updateObjsAndDraw();

	static void updateSkeletonJointsPositions();

	static void updateOrientationCubesPos();

	static void  updateOrientationCubePos(Cube* cube, PONTO* point);

	static void updateObjectPos();

	//static void fixOrientationCubesPos();

	static void drawGLScene();

	static void calculateStereoProj(float xmin, float xmax, float ymin, float ymax,
  float znear, float zfar, float zzps, float dist, float eyeSep);

	static void drawObjects();
};


