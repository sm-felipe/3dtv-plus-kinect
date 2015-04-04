#include "mainDisplay.h"
#include <iostream>

class IntegrationModule {

public:

	IntegrationModule(int argCount, LPWSTR *argList);

	void run();

private:

	static void handleKeyboard(unsigned char key, int x, int y);

	static std::map<std::string, float> parseCommandLineParams(int argCount, LPWSTR *argList, std::vector<DisplayParameter*> acceptableParams);

	static DisplayModule* display;

	static CaptureModule kinect;

};