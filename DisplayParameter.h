#include <string>

class DisplayParameter {
public:
	DisplayParameter(const char* commandLineParam, char* description, std::string unit, bool hasValue);
	bool expectValue();

	std::string commandLineParam;
	std::string description;
	std::string unit;
	
private:
	bool hasValue;
};
