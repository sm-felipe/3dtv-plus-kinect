#include "Shapes.h"

Cube::Cube(float edgeSize){
	this->halfEdgeSize = edgeSize/2.0f;
	x = 0;
	y = 0;
	z = 0;
}

void Cube::draw(){

	float l = halfEdgeSize;

	glLoadIdentity();							// Reset The Current Modelview Matrix
	glTranslatef(x, y, z);
	glBegin(GL_QUADS);							// Draw A Quad								
		glColor3f(0.0f,1.0f,0.0f);				// Set The Color To Green
		glVertex3f( l, l,-l);					// Top Right Of The Quad (Top)
		glVertex3f(-l, l,-l);					// Top Left Of The Quad (Top)
		glVertex3f(-l, l, l);					// Bottom Left Of The Quad (Top)
		glVertex3f( l, l, l);					// Bottom Right Of The Quad (Top)
		glColor3f(1.0f,0.5f,0.0f);				// Set The Color To Orange
		glVertex3f( l,-l, l);					// Top Right Of The Quad (Bottom)
		glVertex3f(-l,-l, l);					// Top Left Of The Quad (Bottom)
		glVertex3f(-l,-l,-l);					// Bottom Left Of The Quad (Bottom)
		glVertex3f( l,-l,-l);					// Bottom Right Of The Quad (Bottom)
		glColor3f(1.0f,0.0f,0.0f);				// Set The Color To Red
		glVertex3f( l, l, l);					// Top Right Of The Quad (Front)
		glVertex3f(-l, l, l);					// Top Left Of The Quad (Front)
		glVertex3f(-l,-l, l);					// Bottom Left Of The Quad (Front)
		glVertex3f( l,-l, l);					// Bottom Right Of The Quad (Front)
		glColor3f(1.0f,1.0f,0.0f);				// Set The Color To Yellow
		glVertex3f( l,-l,-l);					// Top Right Of The Quad (Back)
		glVertex3f(-l,-l,-l);					// Top Left Of The Quad (Back)
		glVertex3f(-l, l,-l);					// Bottom Left Of The Quad (Back)
		glVertex3f( l, l,-l);					// Bottom Right Of The Quad (Back)
		glColor3f(0.0f,0.0f,1.0f);				// Set The Color To Blue
		glVertex3f(-l, l, l);					// Top Right Of The Quad (Left)
		glVertex3f(-l, l,-l);					// Top Left Of The Quad (Left)
		glVertex3f(-l,-l,-l);					// Bottom Left Of The Quad (Left)
		glVertex3f(-l,-l, l);					// Bottom Right Of The Quad (Left)
		glColor3f(0.0f,0.0f,1.0f);				// Set The Color To Violet
		glVertex3f( l, l,-l);					// Top Right Of The Quad (Right)
		glVertex3f( l, l, l);					// Top Left Of The Quad (Right)
		glVertex3f( l,-l, l);					// Bottom Left Of The Quad (Right)
		glVertex3f( l,-l,-l);					// Bottom Right Of The Quad (Right)
	glEnd();									// Done Drawing The Quad
}

BOOL Cube::hasColision(Shape* another){
	float objTop = y + halfEdgeSize;
	float objBottom = y - halfEdgeSize;
	float objLeft = x - halfEdgeSize;
	float objRight = x + halfEdgeSize;

	BOOL verticalInvasion = another->y < objTop && another->y > objBottom;
	BOOL horizontalInvasion = another->x > objLeft && another->x < objRight;
	BOOL hasInvasion = verticalInvasion && horizontalInvasion;
	return hasInvasion;
}

void Cube::reactToPush(Shape* another){
	float objTop = y + halfEdgeSize;
	float objBottom = y - halfEdgeSize;
	float objLeft = x - halfEdgeSize;
	float objRight = x + halfEdgeSize;


	float topDistance = abs(objTop - another->y);
	float bottomDistance = abs(objBottom - another->y);
	float leftDistance = abs(another->x - objLeft);
	float rightDistance = abs( objRight - another->x);

	BOOL deCima = topDistance < bottomDistance && topDistance < leftDistance && topDistance < rightDistance;
	BOOL deBaixo = bottomDistance < topDistance && bottomDistance < leftDistance && bottomDistance < rightDistance;
	BOOL daDireita = rightDistance < topDistance && rightDistance < bottomDistance && rightDistance < leftDistance;
	BOOL daEsquerda = leftDistance < topDistance && leftDistance < bottomDistance && leftDistance < rightDistance;
		
	float oldY = y;
	float oldX = x;

	if(deCima){
		y = y - (objTop - another->y);
	}
	if(deBaixo){
		y = y + (another->y - objBottom);
	}
	if(daDireita){
		x = x - (objRight - another->x);
	}
	if(daEsquerda){
		x = x + (another->x - objLeft);
	}

	revertMoveIfPassLimits(oldX, oldY);
}
