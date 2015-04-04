#include "MainCapture.h"

HANDLE m_hNextSkeletonEvent;
static int const eventCount = 1;
HANDLE hEvents[ eventCount];
MSG msg;

#define PI 3.14159265

BOOL CaptureModule::getKinectConnection() {

	this->kinectAngle = -27;

	this->initialized = false;

	int iSensorCount = 0;
	HRESULT hr = NuiGetSensorCount(&iSensorCount);

	if (FAILED(hr)){
		printf("não conseguiu obter a conexão com o Kinect\n");
		return false;
    }

	INuiSensor * pNuiSensor;

	// Look at each Kinect sensor
	for (int i = 0; i < iSensorCount; ++i){
		// Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr)){
            continue;
        }

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
        if (S_OK == hr) {
            m_pNuiSensor = pNuiSensor;
			printf("OK Kinect found!\n");
            break;
        }

        // This sensor wasn't OK, so release it since we're not using it
        pNuiSensor->Release();
	}

	if (NULL != m_pNuiSensor)  {
        if(initializeKinect(&hr)){
			printf("Kinect initialized\n");
		}else{
			printf("Problem initializing Kinect\n");
		}
    }

	if (NULL == m_pNuiSensor || FAILED(hr))
    {
        printf("No ready Kinect found!\n");
        return false;
    }

	this->initialized = true;
	return true;
}


BOOL CaptureModule::initializeKinect(HRESULT* hr){
	initSmoothMecanism();	
	
	// Initialize the Kinect and specify that we'll be using skeleton
	 *hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON); 
    if (SUCCEEDED(*hr))
    {
        // Create an event that will be signaled when skeleton data is available
        m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

        // Open a skeleton stream to receive skeleton data
        *hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 
			NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE | NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT); 
		return true;
    }
	return false;
}

void CaptureModule::initSmoothMecanism(){
	for(int i = 0; i < FRAMES_IN_SMOOTH_MECHANISM; i++){
		lastHeadPositions[i].x = 0.0f;
		lastHeadPositions[i].y = 0.0f;
		lastHeadPositions[i].z = 0.0f;
	}
	currentFramePos = 0;
}


/*
void CaptureModule::printJointPositions(){
	prepareSkeletonFrame();

	if(skel.eTrackingState == NUI_SKELETON_TRACKED){

		NUI_SKELETON_POSITION_TRACKING_STATE headState = skel.eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_HEAD];
		float headX = skel.SkeletonPositions[NUI_SKELETON_POSITION_HEAD].x;
		float headY = skel.SkeletonPositions[NUI_SKELETON_POSITION_HEAD].y;
		float headZ = skel.SkeletonPositions[NUI_SKELETON_POSITION_HEAD].z;

		NUI_SKELETON_POSITION_TRACKING_STATE lHandState = skel.eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_HAND_LEFT];
		float lHandX = skel.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].x;
		float lHandY = skel.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].y;
		float lHandZ = skel.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].z;

		NUI_SKELETON_POSITION_TRACKING_STATE rHandState = skel.eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_HAND_RIGHT];
		float rHandX = skel.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x;
		float rHandY = skel.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y;
		float rHandZ = skel.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].z;

		if(headState == NUI_SKELETON_POSITION_TRACKED){
			printf("head: %f %f %f ", headX, headY, headZ);
		}
		if(lHandState == NUI_SKELETON_POSITION_TRACKED){
			printf("lhand: %f %f %f ", lHandX, lHandY, lHandZ);
		}
		if(rHandState == NUI_SKELETON_POSITION_TRACKED){
			printf("rhand: %f %f %f ", rHandX, rHandY, rHandZ);
		}
		printf("\n");

	}
}*/

void CaptureModule::prepareSkeletonFrame(){
	if(!initialized){
		MessageBox(NULL, "Não foi possível obter a conexão com o Kinect", "Erro", MB_OK);
		exit(1);
	}

	hEvents[0] = m_hNextSkeletonEvent;

	// Check to see if we have either a message (by passing in QS_ALLEVENTS)
	// Or a Kinect event (hEvents)
	// Update() will check for Kinect events individually, in case more than one are signalled
	DWORD dwEvent = MsgWaitForMultipleObjects(eventCount, hEvents, FALSE, INFINITE, QS_ALLINPUT);
		         
	// Check if this is an event we're waiting on and not a timeout or message
	if (WAIT_OBJECT_0 == dwEvent)
	{
		waitAndProcessSkeleton();
	}

	//coisas do windows, repassando mensagens
	if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

 }

void CaptureModule::waitAndProcessSkeleton(){

    if (NULL == m_pNuiSensor)
    {
        return;
    }

    // Wait for 0ms, just quickly test if it is time to process a skeleton
    if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hNextSkeletonEvent, 0) )
    {
        processSkeleton();
    }
}

void CaptureModule::processSkeleton(){
	NUI_SKELETON_FRAME skeletonFrame = {0};

	HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
    if ( FAILED(hr) )
    {
        return;
    }

	// smooth out the skeleton data
    m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);

	 for (int i = 0 ; i < NUI_SKELETON_COUNT; ++i)
    {
		const NUI_SKELETON_DATA & skeleton = skeletonFrame.SkeletonData[i];
		switch (skeleton.eTrackingState)
		{
		case NUI_SKELETON_TRACKED:// We're tracking the skeleton
			skel = skeleton;
			break;
 
		case NUI_SKELETON_POSITION_ONLY:// we've only received the center point of the skeleton
			skel = skeleton;
			break;
		}
	 }
 }


void CaptureModule::fixKinectAngleEffects(Vector4* pos) {
	
	float Zk = pos->z;
	float Yk = pos->y;

	float distToOrigin = sqrt(Zk*Zk + Yk*Yk);

	float alfa = kinectAngle * PI / 180.0;
	float beta = atan(Yk / Zk);

	float effectiveAngle;
	//if(Yk > 0){
		effectiveAngle =  alfa + beta;
	//}else{
	//	effectiveAngle = alfa - beta;
	//}

	float realY = sin(effectiveAngle) * distToOrigin;
	float realZ = cos(effectiveAngle) * distToOrigin;

	pos->y = realY;
	pos->z = realZ;
}

void CaptureModule::getJointPosition(NUI_SKELETON_POSITION_INDEX jointName, PONTO* result){
	if(skel.eTrackingState == NUI_SKELETON_TRACKED){

		NUI_SKELETON_POSITION_TRACKING_STATE state = skel.eSkeletonPositionTrackingState[jointName];
		if(state == NUI_SKELETON_POSITION_TRACKED){
			Vector4 pos = skel.SkeletonPositions[jointName];

			//convertToStandardBasis(&pos);
			//fixKinectAngleEffects(&pos);

			pos.y -= tvCenterToKinectDistance;

			if(jointName == NUI_SKELETON_POSITION_HEAD) {
				pos = smoothJointData(pos);
			}

			result->x = pos.x;
			result->y = pos.y;
			result->z = pos.z;
		} 
	}
}




Vector4 CaptureModule::smoothJointData(Vector4 pos) {
	lastHeadPositions[currentFramePos] = pos;

	//iniciate the values with the current pos
	float somaX = pos.x;
	float somaY = pos.y;
	float somaZ = pos.z;

	//will iterate over the "circular list" summing the other values
	for(int i = currentFramePos + 1; 
		i % FRAMES_IN_SMOOTH_MECHANISM != currentFramePos; 
		i++){
			int iMod = i % FRAMES_IN_SMOOTH_MECHANISM;
			somaX += lastHeadPositions[iMod].x;
			somaY += lastHeadPositions[iMod].y;
			somaZ += lastHeadPositions[iMod].z;
	}
	currentFramePos = (currentFramePos + 1) % FRAMES_IN_SMOOTH_MECHANISM;

	Vector4 result;
	result.x = somaX / FRAMES_IN_SMOOTH_MECHANISM;
	result.y = somaY / FRAMES_IN_SMOOTH_MECHANISM;
	result.z = somaZ / FRAMES_IN_SMOOTH_MECHANISM;

	return result;
}





void multiplyMatrixTimesVector(float* m, Vector4* elemPos){
	

	float p[4] = {elemPos->x, elemPos->y, elemPos->z};

	elemPos->x = m[0]*p[0] + m[1]*p[1] + m[2]*p[2];
	elemPos->y = m[3]*p[0] + m[4]*p[1] + m[5]*p[2];
	elemPos->z = m[6]*p[0] + m[7]*p[1] + m[8]*p[2];
}

void convertToStandardBasis(Vector4* point){
	float rad = /*KinectAngle **/ PI / 180.0;

	float rotatearoundX[9] = {  1,     0   ,    0     ,
								0, cos(rad), -sin(rad),
								0, sin(rad), cos(rad)};

	float* changeOfBasisMatrix = rotatearoundX;

	multiplyMatrixTimesVector(changeOfBasisMatrix, point);
}