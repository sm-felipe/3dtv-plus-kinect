#include <time.h>

float fr;
static char frbuf[80];

GLfloat modelMatrix[16];


static float gettime(void)
{
  static clock_t told=0;
  clock_t tnew,ris;

  tnew=clock();

  ris=tnew-told;

  told=tnew;

  return(ris/(float)CLOCKS_PER_SEC);
}

static void printstring(void *font, char *string)
{
  int len,i;

  len=(int)strlen(string);
  for(i=0;i<len;i++)
    glutBitmapCharacter(font,string[i]);
}

void prepareFPS(){
	fr=gettime();
}

void printPosition(short xPos, short yPos, char* name, float x, float y, float z){
	glRasterPos2i(xPos, yPos);
	printstring(GLUT_BITMAP_TIMES_ROMAN_24, name);
	char* xspace = x >= 0 ? "  " : "";
	char* yspace = y >= 0 ? "  " : "";
	char* zspace = z >= 0 ? "  " : "";

	sprintf_s(frbuf, " x: %s%0.2f y: %s%0.2f z: %s%0.2f",xspace, x, yspace, y, zspace, z);
	printstring(GLUT_BITMAP_TIMES_ROMAN_24, frbuf);
}

void printWithOneParam(short xPos, short yPos, char* pattern, float param) {
	glRasterPos2i(xPos, yPos);
	sprintf_s(frbuf,pattern, param);
	printstring(GLUT_BITMAP_TIMES_ROMAN_24,frbuf);
}

void printMatrixLine(short xPos, short yPos, float a, float b, float c, float d){
	glRasterPos2i(xPos, yPos);
	char* aspace = a >= 0 ? "  " : "";
	char* bspace = b >= 0 ? "  " : "";
	char* cspace = c >= 0 ? "  " : "";
	char* dspace = d >= 0 ? "  " : "";

	sprintf_s(frbuf, " %s%0.2f %s%0.2f %s%0.2f %s%0.2f", aspace, a, bspace, b, cspace, c, dspace, d);
	printstring(GLUT_BITMAP_TIMES_ROMAN_24, frbuf);

}

void printMatrix(){
	//glShadeModel(GL_FLAT);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5,639.5,-0.5,479.5,-1.0,1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0,1.0,1.0);

	short xPos = 525;
	short yPos = 330;

	printMatrixLine(xPos, yPos     ,modelMatrix[0], modelMatrix[1], modelMatrix[2], modelMatrix[3]);
	printMatrixLine(xPos, yPos - 10,modelMatrix[4], modelMatrix[5], modelMatrix[6], modelMatrix[7]);
	printMatrixLine(xPos, yPos - 20,modelMatrix[8], modelMatrix[9], modelMatrix[10], modelMatrix[11]);
	printMatrixLine(xPos, yPos - 30,modelMatrix[12], modelMatrix[13], modelMatrix[14], modelMatrix[15]);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
}

void debugText(PONTO left, PONTO right, PONTO head, PONTO obj, float systemZ, float tvHalfWidth, float tvHalfHeight, bool perspectiveMode) {
	

	//glShadeModel(GL_FLAT);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5,639.5,-0.5,479.5,-1.0,1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_LINES);
		glVertex3f(525, 525, 0);
		glVertex3f(5555, 555535, 0);
	glEnd();

	glColor3f(1.0,1.0,1.0);

	short horizontalPos = 525;

	printWithOneParam(horizontalPos, 470, "Frame rate: %0.2f", 1.0f/fr);
	
	printPosition(horizontalPos, 450, "left hand", left.x, left.y, left.z);
	printPosition(horizontalPos, 430, "rigt hand", right.x, right.y, right.z);
	printPosition(horizontalPos, 410, "head", head.x, head.y, head.z);
	printPosition(horizontalPos, 390, "object", obj.x, obj.y, obj.z);

	printWithOneParam(horizontalPos, 370, "system z pos: %0.2f", systemZ);
	
	printWithOneParam(horizontalPos, 350, "perspective: %0.f", perspectiveMode);


	printWithOneParam(horizontalPos - 70, 470, "half height: %0.2f", tvHalfHeight);
	printWithOneParam(horizontalPos - 140, 470, "half width: %0.4f", tvHalfWidth);


	//printMatrix(horizontalPos, 330);
	

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
}

void setOrthographicProjection() {
	/*
	// switch to projection mode
	glMatrixMode(GL_PROJECTION);

	// save previous matrix which contains the
	//settings for the perspective projection
	glPushMatrix();

	// reset matrix
	glLoadIdentity();

	// set a 2D orthographic projection
	gluOrtho2D(0, windWidth/2, windHeight/2, 0);

	// switch back to modelview mode
	glMatrixMode(GL_MODELVIEW);*/
}

void restorePerspectiveProjection() {
	/*
	glMatrixMode(GL_PROJECTION);
	// restore previous projection matrix
	glPopMatrix();

	// get back to modelview mode
	glMatrixMode(GL_MODELVIEW);*/
}



void renderBitmapString(
		float x,
		float y,
		char *string) {
	/*
  char *c;
  glRasterPos3f(x, y, 0);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
  }*/
}

void countFrames(){
	/*
	frame++;
	time=glutGet(GLUT_ELAPSED_TIME);

	if (time - timebase > 1000) {
		fps = (frame*1000.0/(time-timebase));
	 	timebase = time;
		frame = 0;
	}
	
	

	glPushMatrix();
	glLoadIdentity();
	setOrthographicProjection();
	glColor3f(0.0f,1.0f,1.0f);
	renderBitmapString(0,0, "Lighthouse3D");
	/*glPopMatrix();
	restorePerspectiveProjection();*/
}