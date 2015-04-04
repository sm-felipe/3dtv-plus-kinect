#include "DisplayParameter.h"

DisplayParameter::DisplayParameter(const char* commandLineParam, char* description, std::string unit, bool expectValue){
	this->commandLineParam = commandLineParam;
	this->description = description;
	this->unit = unit;
	this->hasValue = expectValue;
}

bool DisplayParameter::expectValue(){
	return hasValue;
}