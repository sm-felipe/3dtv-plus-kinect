// SimpleKinect.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <Windows.h>
#include <NuiApi.h>
#include "Shapes.h"

class CaptureModule
{
public:
	
	BOOL getKinectConnection();

	//void printJointPositions();

	void prepareSkeletonFrame();

	void getJointPosition(NUI_SKELETON_POSITION_INDEX jointName, PONTO* result);

	float tvCenterToKinectDistance;

	short kinectAngle;

private:

	BOOL initialized;

	INuiSensor * m_pNuiSensor;
	NUI_SKELETON_DATA  skel;

	static short const FRAMES_IN_SMOOTH_MECHANISM = 15;
	Vector4 lastHeadPositions[FRAMES_IN_SMOOTH_MECHANISM];
	short currentFramePos;

	BOOL initializeKinect(HRESULT* hr);	

	void initSmoothMecanism();

	void waitAndProcessSkeleton();

	void processSkeleton();

	void fixKinectAngleEffects(Vector4* pos);

	Vector4 smoothJointData(Vector4 pos);
};

