#include "MouseInput.h"


MouseInput::MouseInput(HWND hwnd, float adjustFactor){
	this->hwnd = hwnd;
	this->adjustFactor = adjustFactor;
}

void MouseInput::mousePos(float* x, float* y, int width, int height){
	LPPOINT cursorPos = new POINT;
	GetCursorPos(cursorPos);
	ScreenToClient(hwnd, cursorPos);//converte pra unidades da janela

	LONG windXPos = cursorPos->x;
	LONG windYPos = cursorPos->y;
	
	//ajuste para que a posição (0,0) dê no meio da tela
	windXPos -= width/2;
	windYPos -= height/2;

	//para cada 20 pixels que o mouse move, o cubo se move apenas 1
	*x = (float)  windXPos / adjustFactor;
	*y = (float) (height/adjustFactor - windYPos) / adjustFactor;
}