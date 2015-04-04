#include "Shapes.h"


Sphere::Sphere(float diameter){
	this->radius = diameter / 2.0f;
	x = 0;
	y = 0;
	z = 0;
}

void Sphere::draw(){
	glLoadIdentity();							// Reset The Current Modelview Matrix
	glTranslatef(x, y, z);
	glBegin(GL_POLYGON);
		glColor3f(1.0f,0.0f,0.0f);
		GLUquadricObj *obj = gluNewQuadric();
		gluSphere(obj, radius, 30, 30);
	glEnd();
}

BOOL Sphere::hasColision(Shape* another){
	if(euclidianDist(another) < radius){
		return true;
	}
	return false;
}

float Sphere::euclidianDist(Shape* another){
	float xx = pow(another->x - x, 2);
	float yy = pow(another->y - y, 2);
	return sqrt(xx + yy);
}

void Sphere::reactToPush(Shape* another){
	float hipotenuse = euclidianDist(another);
	float oppositeSide = another->y - y;
	float adjacentSide = another->x - x;

	float sin = oppositeSide / hipotenuse;
	float cos = adjacentSide / hipotenuse;

	float sizeOfMovement = radius - hipotenuse;
	
	float oldX = x;
	float oldY = y;

	x = x - cos * sizeOfMovement;
	y = y - sin * sizeOfMovement;

	revertMoveIfPassLimits(oldX, oldY);
}