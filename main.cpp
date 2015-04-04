#include "mainIntegration.h"
#include <map>

DisplayModule* IntegrationModule::display;
CaptureModule IntegrationModule::kinect;


IntegrationModule::IntegrationModule(int argCount, LPWSTR *argList){

	display = new DisplayModule;
	std::map<std::string, float> params = parseCommandLineParams(argCount, argList, display->getAcceptableParams());

	if(!kinect.getKinectConnection()){
        MessageBox(NULL, "Não foi possível obter a conexão com o Kinect", "Erro", MB_OK);
		exit(1);
	}

	kinect.tvCenterToKinectDistance = params[paramKinectToTV] ? params[paramKinectToTV] : /*0.170f*/ 0.170f;

	display->initializeDisplayModule(kinect, params);
	glutKeyboardFunc(handleKeyboard);
}

char* toChar(LPWSTR param);
float toFloat(LPWSTR param);
BOOL equals(char* arg, std::string paramName);
float validAndConvert(DisplayParameter* key, LPWSTR *argList, int* i, int argCount);
std::string buildHelpMessage(std::vector<DisplayParameter*> acceptableParams);


std::map<std::string, float> IntegrationModule::parseCommandLineParams(int argCount, LPWSTR *argList, std::vector<DisplayParameter*> acceptableParams){

	std::map<std::string, float> params;
	
	if(argCount > 1){

		int i = 1;
		while(i < argCount){
			char* arg = toChar(argList[i]);

			if(equals(arg, "--help")){
				std::string testing = buildHelpMessage(acceptableParams);
				MessageBoxA(NULL, testing.c_str(), "Ajuda", MB_OK);
				exit(0);
			}else {
				for(unsigned int j = 0; j < acceptableParams.size(); j++){
					DisplayParameter* parameter = acceptableParams.at(j);
					if(equals(arg, parameter->commandLineParam)){
						params[parameter->commandLineParam] = validAndConvert(parameter, argList, &i, argCount);
						break;
					}
				}
			}
		}
	}

	return params;
}

char* toChar(LPWSTR param) {
	int length = WideCharToMultiByte(CP_ACP, 0, param, -1, 0, 0, NULL, NULL);
	char* output = new char[length];
	WideCharToMultiByte(CP_ACP, 0, param, -1, output , length, NULL, NULL);

	return output;
}

BOOL equals(char* arg, std::string paramName){
	return strcmp(arg, paramName.c_str()) == 0;
}

std::string buildHelpMessage(std::vector<DisplayParameter*> acceptableParams){
	std::string helpMessage = "";

	std::string valuePart = " <valor>";
	std::string colon = ": ";
	std::string openBracket = " (";
	std::string closeBracket = ")\n";
	

	for(unsigned int i = 0; i < acceptableParams.size(); i++){
		DisplayParameter* parameter = acceptableParams[i];
		std::string commandLine = parameter->commandLineParam;
		std::string description = parameter->description;
		std::string unit = parameter->unit;

		std::string result;
		result += commandLine;
		if(parameter->expectValue()){
			result += valuePart;
		}
		result += colon;
		result += description;
		if(parameter->expectValue()){
			result += openBracket;
			result += unit;
			result += closeBracket;
		}

		helpMessage += result;
	}
	return helpMessage;
}

float validAndConvert(DisplayParameter* parameter, LPWSTR *argList, int* i, int argCount){
	float converted;

	if(parameter->expectValue()){
		if(*i + 1 >= argCount) {
			char errorMsg[60] = "Não foi passado o valor para o parâmetro: ";
			strcat_s(errorMsg, parameter->commandLineParam.c_str());
			MessageBoxA(NULL, errorMsg, "Erro", MB_OK);
			exit(0);
		}

		converted = toFloat(argList[++*i]);
		if(converted == 0.0f){
			char errorMsg[60] = "Foi enviado um valor inválido para o parâmetro: ";
			strcat_s(errorMsg, parameter->commandLineParam.c_str());
			MessageBoxA(NULL, errorMsg, "Erro", MB_OK);
			exit(0);
		}
	}else{
		converted = 1.0f;
	}
	++*i;

	return converted;
}


float toFloat(LPWSTR param){
	char* output = toChar(param);

	char* isNotNumber;
	double converted = strtod(output, &isNotNumber);
	
	if(*isNotNumber){
		return 0.0f;
	}else{
		return (float) converted;
	}
}

void IntegrationModule::handleKeyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case '+':
		display->zPos+= 0.01f;
		break;
	case '-':
		display->zPos-= 0.01f;
		break;
	case 'r':
		display->resetObjectsPos();
		break;
	case 'h':
		display->adjustPointOfView = !display->adjustPointOfView;
		break;
	case 'd':
		display->debugMode = !display->debugMode;
		break;
	case 'p':
		display->perspectiveMode = !display->perspectiveMode;
		break;
	case '1':
		display->changeObject(1);
		break;
	case '2':
		display->changeObject(2);
		break;
	case '3':
		display->changeObject(3);
		break;
	case 27:
		display->destroyWindow();
		exit (0);
		glutPostRedisplay();
		break;
	
	default:
		break;
	}

} 

void IntegrationModule::run(){
	glutMainLoop();
	FreeConsole();
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	LPWSTR *argList;
    int argCount;
    argList = CommandLineToArgvW(GetCommandLineW(), &argCount);

	IntegrationModule* integ = new IntegrationModule(argCount, argList);
	integ->run();

	LocalFree(argList);
	return 0;
}