#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <math.h>

class PONTO {
public:
	float x;
	float y;
	float z;

	PONTO(){
		x = 0;
		y = 0;
		z = 0;
	}

	PONTO(float x, float y, float z){
		this->x = x;
		this->y = y;
		this->z = z;
	}
};


class Shape {

public:
	float x;
	float y;
	float z;

	float limitX;
	float limitY;

	virtual void move(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	virtual void draw() = 0;

	virtual BOOL hasColision(Shape* another) = 0;

	virtual void reactToPush(Shape* another) = 0;

	virtual PONTO* getPos() {
		return new PONTO(x, y, z);
	}

	void revertMoveIfPassLimits(float oldX, float oldY){
		if(abs(x) > limitX)
			x = oldX;
		if(abs(y) > limitY)
			y = oldY;
	}

};

class Cube : public Shape
{
public:

	Cube(float edgeSize);

	void draw();

	virtual BOOL hasColision(Shape* another);

	virtual void reactToPush(Shape* another);

private:

	float halfEdgeSize;

};

class Sphere : public Shape
{
public:

	Sphere(float diameter);

	void draw();

	virtual BOOL hasColision(Shape* another);

	virtual void reactToPush(Shape* another);

private:

	float radius;

	float euclidianDist(Shape* another);
};

class Parallelepiped : public Shape
{
public:
	//Parallelepiped(PONTO* start, PONTO* end);

	Parallelepiped(float height, float width, float depth);

	void draw();

	virtual BOOL hasColision(Shape* another);

	virtual void reactToPush(Shape* another);

	float getHeight();

	float getWidth();

	float getDepth();

private:
	float height;
	float width;
	float depth;	
};