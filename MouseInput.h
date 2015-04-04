#include <windows.h>		// Header File For Windows


class MouseInput{

public:
	MouseInput(HWND hwnd, float adjustFactor);

	void mousePos(float* x, float* y, int width, int height);

private:
	HWND hwnd;
	float adjustFactor;

};