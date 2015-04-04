#include "Shapes.h"


/*Parallelepiped::Parallelepiped(PONTO* start, PONTO* end){
	height = abs(start->y - end->y);
	width = abs(start->x - end->x);
	depth = abs(start->z - end->z);

	x = width/2.0f + start->x;
	y = height/2.0f + start->y;
	z = depth/2.0f + start->z;
}*/

Parallelepiped::Parallelepiped(float height, float width, float depth){
	this->height = height;
	this->width = width;
	this->depth = depth;

	x = 0;
	y = 0;
	z = 0;
}

float Parallelepiped::getHeight(){
	return height;
}

float Parallelepiped::getWidth(){
	return width;
}

float Parallelepiped::getDepth(){
	return depth;
}

void Parallelepiped::draw() {

	glLoadIdentity();							// Reset The Current Modelview Matrix
	glTranslatef(x, y, z);

	float x = width / 2.0f;
	float y = height / 2.0f;
	float z = depth / 2.0f;

	bool useTexture = false;

	if(useTexture){
	//	glBindTexture(GL_TEXTURE_2D, textureID);

		glBegin(GL_QUADS);
			// Front Face
			glNormal3f( 0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y,  z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( x, -y,  z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( x,  y,  z);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-x,  y,  z);
			// Back Face
			glNormal3f( 0.0f, 0.0f,-1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-x, -y, -z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-x,  y, -z);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( x,  y, -z);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( x, -y, -z);
			// Top Face
			glNormal3f( 0.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-x,  y, -z);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-x,  y,  z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( x,  y,  z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( x,  y, -z);
			// Bottom Face
			glNormal3f( 0.0f,-1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-x, -y, -z);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( x, -y, -z);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( x, -y,  z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-x, -y,  z);
			// Right face
			glNormal3f( 1.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( x, -y, -z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( x,  y, -z);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( x,  y,  z);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( x, -y,  z);
			// Left Face
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y, -z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-x, -y,  z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-x,  y,  z);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-x,  y, -z);
		glEnd();
	}else{

		glBegin(GL_QUADS);							// Draw A Quad								
			glColor3f(0.0f,1.0f,0.0f);				// Set The Color To Green
			glVertex3f( x, y,-z);					// Top Right Of The Quad (Top)
			glVertex3f(-x, y,-z);					// Top Left Of The Quad (Top)
			glVertex3f(-x, y, z);					// Bottom Left Of The Quad (Top)
			glVertex3f( x, y, z);					// Bottom Right Of The Quad (Top)
			glColor3f(1.0f,0.5f,0.0f);				// Set The Color To Orange
			glVertex3f( x,-y, z);					// Top Right Of The Quad (Bottom)
			glVertex3f(-x,-y, z);					// Top Left Of The Quad (Bottom)
			glVertex3f(-x,-y,-z);					// Bottom Left Of The Quad (Bottom)
			glVertex3f( x,-y,-z);					// Bottom Right Of The Quad (Bottom)
			glColor3f(1.0f,0.0f,0.0f);				// Set The Color To Red
			glVertex3f( x, y, z);					// Top Right Of The Quad (Front)
			glVertex3f(-x, y, z);					// Top Left Of The Quad (Front)
			glVertex3f(-x,-y, z);					// Bottom Left Of The Quad (Front)
			glVertex3f( x,-y, z);					// Bottom Right Of The Quad (Front)
			glColor3f(1.0f,1.0f,0.0f);				// Set The Color To Yellow
			glVertex3f( x,-y,-z);					// Top Right Of The Quad (Back)
			glVertex3f(-x,-y,-z);					// Top Left Of The Quad (Back)
			glVertex3f(-x, y,-z);					// Bottom Left Of The Quad (Back)
			glVertex3f( x, y,-z);					// Bottom Right Of The Quad (Back)
			glColor3f(0.0f,0.0f,1.0f);				// Set The Color To Blue
			glVertex3f(-x, y, z);					// Top Right Of The Quad (Left)
			glVertex3f(-x, y,-z);					// Top Left Of The Quad (Left)
			glVertex3f(-x,-y,-z);					// Bottom Left Of The Quad (Left)
			glVertex3f(-x,-y, z);					// Bottom Right Of The Quad (Left)
			glColor3f(0.0f,0.0f,1.0f);				// Set The Color To Blue
			glVertex3f( x, y,-z);					// Top Right Of The Quad (Right)
			glVertex3f( x, y, z);					// Top Left Of The Quad (Right)
			glVertex3f( x,-y, z);					// Bottom Left Of The Quad (Right)
			glVertex3f( x,-y,-z);					// Bottom Right Of The Quad (Right)
		glEnd();									// Done Drawing The Quad
	}
}

BOOL Parallelepiped::hasColision(Shape* another){
	float xHalfSize = this->getWidth()/2.0f;
	float yHalfSize = this->getHeight()/2.0f;
	//float zHalfSize = this->getDepth()/2.0f;

	float objTop = y + yHalfSize;
	float objBottom = y - yHalfSize;
	float objLeft = x - xHalfSize;
	float objRight = x + xHalfSize;
	//float objFront = z + zHalfSize;
	//float objBack = z - zHalfSize; 

	BOOL verticalInvasion = another->y < objTop && another->y > objBottom;
	BOOL horizontalInvasion = another->x > objLeft && another->x < objRight;
	//BOOL depthInvasion = another->z > objBack && another->z < objFront;

	BOOL hasInvasion = verticalInvasion && horizontalInvasion /*&& depthInvasion*/;

	if(hasInvasion){
		return true;
	}

	return false;
}

void Parallelepiped::reactToPush(Shape* another){
	float xHalfSize = this->getWidth()/2.0f;
	float yHalfSize = this->getHeight()/2.0f;
	//float zHalfSize = this->getDepth()/2.0f;

	float objTop = y + yHalfSize;
	float objBottom = y - yHalfSize;
	float objLeft = x - xHalfSize;
	float objRight = x + xHalfSize;
	//float objFront = z + zHalfSize;
	//float objBack = z - zHalfSize;

	float topDistance = abs(objTop - another->y);
	float bottomDistance = abs(objBottom - another->y);
	float leftDistance = abs(another->x - objLeft);
	float rightDistance = abs( objRight - another->x);

	BOOL deCima = topDistance < bottomDistance && topDistance < leftDistance && topDistance < rightDistance;
	BOOL deBaixo = bottomDistance < topDistance && bottomDistance < leftDistance && bottomDistance < rightDistance;
	BOOL daDireita = rightDistance < topDistance && rightDistance < bottomDistance && rightDistance < leftDistance;
	BOOL daEsquerda = leftDistance < topDistance && leftDistance < bottomDistance && leftDistance < rightDistance;

	float oldX = x;
	float oldY = y;

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

