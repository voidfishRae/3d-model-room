/******************************************
*
* Official Name : Rae Tasker
*
* Nickname :
	*
	*E - mail : rtasker@syr.edu
	*
	*Assignment:  Final Project
	*
	*Environment / Compiler : Visual Studio Community 2019
	*
	*Date : May 14th 2021
	*
	*References : https://gamedev.stackexchange.com/questions/46931/texture-will-not-rotate-with-glusphere
	*			https://cs.brynmawr.edu/Courses/cs312/fall2010/lectures/gl_05.pdf
	*			http://www.cplusplus.com/forum/beginner/22286/
	*
	*Interactions : Press the escape key to quit.
	*  Arrow keys to move and turn. 
	*
	*******************************************/

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>


#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

//#define PI 3.14159265;
#define PI 3.14159

using namespace std;

////////////////////////////////
//globals
///////////////////////////////
static float meX = 0, meY = 7, meZ = -7;
static float angle = 0;
static float stepsize = 1.0;  //step size
static float turnsize = 10.0; //degrees to turn


bool overheadLight = false;
bool movingCurtain = false; //false when closing, true when opening
bool flagsMet = false; 


float l = -8;
float r = 8;
float bot = 0;
float top = 20;
float near = -20;
float far = 6;

int cubeRandom;
float cubePoints[5][3] = { {r-5, 10, far+6}, //outside the window
	{8, 9.5, -8.5},	//beside the clock
	{0, 0, near + 1},	//behind player (in fireplace if we get there)
	{0, top - 1, -4 }, //on the roof
	{-7.5, 0, -9.5} };	//beside oven


float skyAngle = 0;

float treeRatio = 0.85;
float treeAngle = 40; 
float maxLevel = 3;
float randomTrees[15];
float randomZ[15];
bool cherry = false; 
bool shade = false;
int randomLevel[15];



//global variables for curtain intial values
float curtCenterL = 3; float curtMidx1 = 3.5; float curtMidx2 = 4; float curtEndL = 5;
float curtFar1 = far - 1.3; float curtFar2 = far - 0.8; float curtFar3 = far - 1;
//intialize control points for curtains
static float controlPointsLeft[6][4][3] = {
	{{curtCenterL, top-5, curtFar3}, {curtMidx1, top - 5, curtFar2}, {curtMidx2, top - 5, curtFar1},{curtEndL, top - 5, curtFar2}},
	{{curtCenterL, top - 8, curtFar3},{curtMidx1, top - 8, curtFar2},{curtMidx2, top - 8, curtFar1},{curtEndL, top - 8, curtFar2}},
	{{curtCenterL, top - 10, curtFar3},{curtMidx1, top - 10, curtFar2},{curtMidx2, top - 10, curtFar1},{curtEndL, top - 10, curtFar2}},
	{{curtCenterL, top - 12, curtFar3},{curtMidx1, top - 12, curtFar2},{curtMidx2, top - 12, curtFar1},{curtEndL, top - 12, curtFar2}},
	{{curtCenterL, top - 14, curtFar3},{curtMidx1, top - 14, curtFar2},{curtMidx2, top - 14, curtFar1},{curtEndL, top - 14, curtFar2}},
	{{curtCenterL, top - 16, curtFar3},{curtMidx1, top - 16, curtFar2},{curtMidx2, top - 16, curtFar1},{curtEndL, top - 16, curtFar2}}
};

//global variables for curtain intial values
float curtCenterR = -2; float curtMidx1R = -3; float curtMidx2R = -4; float curtEndR = -5;
//intialize control points for curtains
static float controlPointsRight[6][4][3] = {
	{{curtCenterR, top - 5, curtFar3}, {curtMidx1R, top - 5, curtFar2}, {curtMidx2R, top - 5, curtFar1},{curtEndR, top - 5, curtFar2}},
	{{curtCenterR, top - 8, curtFar3},{curtMidx1R, top - 8, curtFar2},{curtMidx2R, top - 8, curtFar1},{curtEndR, top - 8, curtFar2}},
	{{curtCenterR, top - 10, curtFar3},{curtMidx1R, top - 10, curtFar2},{curtMidx2R, top - 10, curtFar1},{curtEndR, top - 10, curtFar2}},
	{{curtCenterR, top - 12, curtFar3},{curtMidx1R, top - 12, curtFar2},{curtMidx2R, top - 12, curtFar1},{curtEndR, top - 12, curtFar2}},
	{{curtCenterR, top - 14, curtFar3},{curtMidx1R, top - 14, curtFar2},{curtMidx2R, top - 14, curtFar1},{curtEndR, top - 14, curtFar2}},
	{{curtCenterR, top - 16, curtFar3},{curtMidx1R, top - 16, curtFar2},{curtMidx2R, top - 16, curtFar1},{curtEndR, top - 16, curtFar2}}
};

//texture info
static unsigned int texture[7]; // Array of texture indices.  //textureEdit
static unsigned char chessboard[64][64][3]; // Storage for chessboard image.


//////////////////////////////////////
//Tree Fractal Initialization
//////////////////////////////////////

class Source; // Make source class visible.

// Sequel class.
class Sequel
{
public:
	Sequel() { coords.clear(); v.clear(); }
	void drawTree(); // Routine to draw Sequel object that represents a tree.

	friend class Source;

private:
	vector<float> coords; // Vector of x, y co-ordinates of points specifying sequel object.
	vector<Source> v; // Vector of associated source objects to be produced recursively.
};

// Routine to draw Sequel object tree.
void Sequel::drawTree()
{
	float matBark[] = { 0.6,0.1,0.1,1 };
	float matShade[] = { 0,0,0,0.5 };

	if (shade) {
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matShade);
	}
	else {
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matBark);
	}

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 3; i++)
		glVertex2f(coords[2 * i], coords[2 * i + 1]);
	glEnd();
}

class Source
{
public:
	Source() { }
	Source(float coordsVal[4])
	{
		for (int i = 0; i < 4; i++) coords[i] = coordsVal[i];
	}

	void draw(); // Routine to draw source line segment.
	Sequel sourceToSequelTree(); // Routine to generate Sequel object in case of tree.
	void produceTree(int level); // Recursive routine to produce tree.

	friend class Sequel;

private:
	float coords[4]; // x, y co-ordinates of a line segment endpoints.
};

// Routine to draw source line segment.
void Source::draw()
{
	float matBark[] = { 0.6,0.1,0.1,1 };
	float matShade[] = { 0,0,0,0.5 };

	if (shade) {
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matShade);
	}
	else {
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matBark);
	}
	

	//glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	for (int i = 0; i < 2; i++)
		glVertex2f(coords[2 * i], coords[2 * i + 1]);
	glEnd();
}

// Routine to generate Sequel object in case of tree.
Sequel Source::sourceToSequelTree()
{
	float x0, y0, x1, y1, coordsVal[10], coordsVal1[4];
	int i, j;
	Source s;
	Sequel seq = *new Sequel();

	x0 = coords[0]; y0 = coords[1]; x1 = coords[2]; y1 = coords[3];

	// From the co-ordinates of the two segment endpoints calculate the co-ordinates of
	// the 3 vertices of the sub-branch V-shape.
	coordsVal[0] = x1 + treeRatio * cos((PI / 180.0) * treeAngle / 2.0) * (x1 - x0) - treeRatio * sin((PI / 180.0) * treeAngle / 2.0) * (y1 - y0);
	coordsVal[1] = y1 + treeRatio * cos((PI / 180.0) * treeAngle / 2.0) * (y1 - y0) + treeRatio * sin((PI / 180.0) * treeAngle / 2.0) * (x1 - x0);
	coordsVal[2] = x1;
	coordsVal[3] = y1;
	coordsVal[4] = x1 + treeRatio * cos((PI / 180.0) * treeAngle / 2.0) * (x1 - x0) + treeRatio * sin((PI / 180.0) * treeAngle / 2.0) * (y1 - y0);
	coordsVal[5] = y1 + treeRatio * cos((PI / 180.0) * treeAngle / 2.0) * (y1 - y0) - treeRatio * sin((PI / 180.0) * treeAngle / 2.0) * (x1 - x0);

	// Enter V-shape vertices into sequel object.
	for (i = 0; i < 6; i++) seq.coords.push_back(coordsVal[i]);

	// Specify both sub-branch segments of the V-shape for recursive production.
	for (i = 0; i < 2; i++)
	{
		coordsVal1[0] = coordsVal[2];
		coordsVal1[1] = coordsVal[3];
		for (j = 2; j < 4; j++)
		{
			coordsVal1[j] = coordsVal[4 * i + j - 2];
		}
		s = *new Source(coordsVal1);
		seq.v.push_back(s);
	}

	return seq;
}

// Routine to draw leaf called by the following routine to produce tree.
void drawLeaf(float x, float y)
{
	float matLeafs[] = { 0,1,0,1 };
	float matCherry[] = { 1, 0.4,0.5,1 };
	float matShade[] = { 0,0,0,0.5 };

	if (cherry) {
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matCherry);
	}
	else if (shade) {
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matShade);
	}
	else {
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matLeafs);
	}
	

	glColor3f(0.0, 1.0, 0.0);
	glPushMatrix();
	glTranslatef(x, y, 0.0);
	//glRotatef(rand() % 360, 0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glVertex2f(0.0, 0.0);
	glVertex2f(1.0, 2.0);
	glVertex2f(0.0, 3.0);
	glVertex2f(-1.0, 2.0);
	glEnd();
	glPopMatrix();
}

// Recursive routine to produce tree.
void Source::produceTree(int level)
{
	glColor3f(0.4, 0.5, 0.5);

	// Branches are thinner up the tree.
	glLineWidth(2 * (maxLevel - level));

	// Source and sequels at all prior levels are drawn
	if (maxLevel == 0) this->draw();
	else if (maxLevel == 1) { this->draw(); this->sourceToSequelTree().drawTree(); }
	else if (level < maxLevel)
	{
		if (level == 0) this->draw();
		this->sourceToSequelTree().drawTree();
		for (int i = 0; i < 2; i++) this->sourceToSequelTree().v[i].produceTree(level + 1);
	}

	// Embellish with leaves.
	if (level == maxLevel - 1)
	{
		drawLeaf(this->sourceToSequelTree().coords[0], this->sourceToSequelTree().coords[1]);
		drawLeaf(this->sourceToSequelTree().coords[4], this->sourceToSequelTree().coords[5]);
	}

	// Restore line width.
	glLineWidth(1.0);
}

////////////////////////////////////////
//Textures Initialization
////////////////////////////////////////

// Struct of bitmap file.
struct BitMapFile
{
	int sizeX;
	int sizeY;
	unsigned char* data;
};

// Routine to read a bitmap file.
// Works only for uncompressed bmp files of 24-bit color.
BitMapFile* getBMPData(string filename)
{
	BitMapFile* bmp = new BitMapFile;
	unsigned int size, offset, headerSize;

	// Read input file name.
	ifstream infile(filename.c_str(), ios::binary);

	// Get the starting point of the image data.
	infile.seekg(10);
	infile.read((char*)&offset, 4);

	// Get the header size of the bitmap.
	infile.read((char*)&headerSize, 4);

	// Get width and height values in the bitmap header.
	infile.seekg(18);
	infile.read((char*)&bmp->sizeX, 4);
	infile.read((char*)&bmp->sizeY, 4);

	// Allocate buffer for the image.
	size = bmp->sizeX * bmp->sizeY * 24;
	bmp->data = new unsigned char[size];

	// Read bitmap data.
	infile.seekg(offset);
	infile.read((char*)bmp->data, size);

	// Reverse color from bgr to rgb.
	int temp;
	for (int i = 0; i < size; i += 3)
	{
		temp = bmp->data[i];
		bmp->data[i] = bmp->data[i + 2];
		bmp->data[i + 2] = temp;
	}

	return bmp;
}

// Load external textures. -- for most textures
void loadExternalTextures()
{
	// Local storage for bmp image data.
	BitMapFile* image[6];			//textureEdit

	// Load the texture.
	image[0] = getBMPData("rtaskerTEXTURES/wallpaper1.bmp");		//textureEdit
	image[1] = getBMPData("rtaskerTEXTURES/wallpaper2.bmp");
	image[2] = getBMPData("rtaskerTEXTURES/carpet.bmp");
	image[3] = getBMPData("rtaskerTEXTURES/wallpaper3.bmp");
	image[4] = getBMPData("rtaskerTEXTURES/curtain.bmp");
	image[5] = getBMPData("rtaskerTEXTURES/sky.bmp");

	// Activate texture index texture[0]. 
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	// Set texture parameters for wrapping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Specify an image as the texture to be bound with the currently active texture index.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);

	// Activate texture wallpaper2 (texture[1])
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	// Set texture parameters for wrapping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Specify an image as the texture to be bound with the currently active texture index.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[1]->sizeX, image[1]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[1]->data);

	// Activate texture carpet (texture[2])
	glBindTexture(GL_TEXTURE_2D, texture[2]);

	// Set texture parameters for wrapping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Specify an image as the texture to be bound with the currently active texture index.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[2]->sizeX, image[2]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[2]->data);

	// Activate texture wallpaper3 (texture[4])
	glBindTexture(GL_TEXTURE_2D, texture[4]);

	// Set texture parameters for wrapping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Specify an image as the texture to be bound with the currently active texture index.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[3]->sizeX, image[3]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[3]->data);

	// Activate texture curtain (texture[5])
	glBindTexture(GL_TEXTURE_2D, texture[5]);

	// Set texture parameters for wrapping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Specify an image as the texture to be bound with the currently active texture index.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[4]->sizeX, image[4]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[4]->data);

	// Activate texture sky (texture[6])
	glBindTexture(GL_TEXTURE_2D, texture[6]);

	// Set texture parameters for wrapping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Specify an image as the texture to be bound with the currently active texture index.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[5]->sizeX, image[5]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[5]->data);
}


// Routine to load a program-generated image as a texture. -- for checkerboard wall 
void loadProceduralTextures()
{
	// Activate texture index texture[1]. checker wall
	glBindTexture(GL_TEXTURE_2D, texture[3]);

	// Set texture parameters for wrapping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture parameters for filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Specify an image as the texture to be bound with the currently active texture index.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, chessboard);
}

// Create 64 x 64 RGB image of a chessboard. from loadTextures.cpp
void createChessboard(void)
{
	int i, j;
	for (i = 0; i < 64; i++)
		for (j = 0; j < 64; j++)
			if ((((i / 8) % 2) && ((j / 8) % 2)) || (!((i / 8) % 2) && !((j / 8) % 2)))
			{
				chessboard[i][j][0] = 0xF9;
				chessboard[i][j][1] = 0xE2;
				chessboard[i][j][2] = 0x9C;
			}
			else
			{
				chessboard[i][j][0] = 0xFC;
				chessboard[i][j][1] = 0xF5;
				chessboard[i][j][2] = 0xCC;
			}
}

//////////////////////////////////
//Drawing methods
/////////////////////////////////

void drawWalls() {


	float matAmbAndDif1[] = { 1.0, 1.0, 0.9, 1.0 };

	glEnable(GL_LIGHTING);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif1);
	//triangle strip with a hole in the middle
	//glColor3f(1, 1, 0.9);
	glNormal3f(0, 0, -1);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(l, top, far);
	glVertex3f(l + 3, top - 5, far);
	glVertex3f(l, bot, far);
	glVertex3f(l + 3, bot + 4, far);
	glVertex3f(r, bot, far);
	glVertex3f(r - 3, bot + 4, far);
	glVertex3f(r, top, far);
	glVertex3f(r - 3, top - 5, far);
	glVertex3f(l, top, far);
	glVertex3f(l + 3, top - 5, far);

	glEnd();

	

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);


	//enable texture wallpaper1
	glBindTexture(GL_TEXTURE_2D, texture[3]);

	//side walls
	//left wall
	glColor3f(1, 0, 1);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex3f(l, bot, near);
	glTexCoord2f(4, 0);
	glVertex3f(l, bot, far);
	glTexCoord2f(4, 4);
	glVertex3f(l, top, far);
	glTexCoord2f(0, 4);
	glVertex3f(l, top, near);
	glEnd();


	//enable texture wallpaper2
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	//right wall
	glColor3f(0, 1, 0);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex3f(r, bot, near);
	glTexCoord2f(5, 0);
	glVertex3f(r, bot, far);
	glTexCoord2f(5, 5);
	glVertex3f(r, top, far);
	glTexCoord2f(0, 5);
	glVertex3f(r, top, near);
	glEnd();


	//bind wallpaper3 texture
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	//4th wall
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex3f(r, bot, near);
	glTexCoord2f(0, 1);
	glVertex3f(r, top, near);
	glTexCoord2f(1, 1);
	glVertex3f(l, top, near);
	glTexCoord2f(1, 0);
	glVertex3f(l, bot, near);
	glEnd();


	//bind carpet texture
	glBindTexture(GL_TEXTURE_2D, texture[2]);

	//floor 
	glColor3f(0, 0, 1);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); 
	glVertex3f(l, bot, far);
	glTexCoord2f(10, 0); 
	glVertex3f(l, bot, near);
	glTexCoord2f(10, 10); 
	glVertex3f(r, bot, near);
	glTexCoord2f(0, 10); 
	glVertex3f(r, bot, far);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	//roof (with light)
	glColor3f(0.8, 0.8, 0.9);
	glBegin(GL_POLYGON);
	glVertex3f(l, top, far);
	glVertex3f(l, top, near);
	glVertex3f(r, top, near);
	glVertex3f(r, top, far);
	glEnd();
}

void drawWindow() {
	float matAmbAndDif1[] = { 0.5, 0.9, 1.0, 0.5 };
	float matFrame[] = { 0.7, 0.4, 0, 1.0 };
	float normalShine[] = { 0.0 };
	float shinyWindow[] = { 5.0 };

	//Material properties for  window
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif1);
	glMaterialfv(GL_FRONT, GL_SHININESS, shinyWindow);

	//glass (uses blending)
	glColor4f(0.5, 0.9, 1.0, 0.5);
	glBegin(GL_POLYGON);
	glVertex3f(l + 3, top - 5, far);
	glVertex3f(l + 3, bot + 4, far);
	glVertex3f(r - 3, bot + 4, far);
	glVertex3f(r - 3, top - 5, far);
	glEnd();

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matFrame);
	glMaterialfv(GL_FRONT, GL_SHININESS, normalShine);

	glColor3f(0, 1, 0);
	glBegin(GL_POLYGON);
	glVertex3f(0, top - 4.74, far - 0.1);
	glVertex3f(0.5, top - 4.75, far - 0.1);
	glVertex3f(0.5, bot + 3.75, far - 0.1);
	glVertex3f(0, bot + 3.75, far - 0.1);
	glEnd();



	for (int i = 0; i < 6; i++) {
		glBegin(GL_POLYGON);
		glVertex3f(5 + i, top - 5, far - 0.1);
		glVertex3f(5.1 + i, top - 5, far - 0.1);
		glVertex3f(5.1 + i, bot + 4, far - 0.1);
		glVertex3f(5 + i, bot + 4, far - 0.1);
		glEnd();
	}


	//for loop to generate horizontal lines
	for (int i = 0; i < 6; i++) {
		glBegin(GL_LINE);
		glVertex3f(l + 3, bot + i, far - 0.1);
		glVertex3f(r - 3, bot + i, far - 0.1);
		glEnd();
	}

	//frame
	glPushMatrix();

	glTranslatef(0, bot + 3.75, far);

	glPushMatrix();
	glScalef(15.5, 1, 1);
	glutSolidCube(0.65); //upper part of bottom sill
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -0.5, 0);
	glScalef(19, 1, 1);
	glutSolidCube(0.5); //lower part of bottom sill
	glPopMatrix();

	glPopMatrix(); //translate both back. 
	
	glPushMatrix();
	glTranslatef(0, top - 4.75, far);
	glScalef(15.5, 1, 1);
	glutSolidCube(0.65);
	glPopMatrix();

}

void drawCurtains(){

	float matAmbAndDifRod[] = { 0.8, 0.3, 0.1, 1.0 };
	float matAmbAndDifFabric[] = { 1,1,1,1 };

	//Material properties for rod
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDifRod);

	//two spheres and a cylinder rod
	glColor3f(0.8, 0.8, 0);
	glPushMatrix();
	glTranslatef(l + 3, top - 5, far - 1);
	glutSolidSphere(0.3, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(r - 3, top - 5, far - 1);
	glutSolidSphere(0.3, 20, 20);
	glPopMatrix();

	GLUquadricObj* qobj;
	qobj = gluNewQuadric();

	glPushMatrix();
	glTranslatef(l+3, top - 5, far-1);
	glRotatef(90, 0, 1, 0);
	gluCylinder(qobj, 0.1, 0.1, 10, 10, 10);
	glPopMatrix();

	//mesh - a solid surface, not wire? maybe wire for dev
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDifFabric);

	glEnable(GL_AUTO_NORMAL);
	glShadeModel(GL_FLAT);

	glEnable(GL_TEXTURE_2D);

	

	//Left curtain
	//specify and enable bezier surface
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 6, controlPointsLeft[0][0]);
	glEnable(GL_MAP2_VERTEX_3);
	
	glBindTexture(GL_FRONT, texture[5]);
	glTexCoord2f(0, 0); glVertex3f(controlPointsLeft[5][0][0], controlPointsLeft[5][0][1], controlPointsLeft[5][0][2]);
	glTexCoord2f(1, 0); glVertex3f(controlPointsLeft[0][0][0], controlPointsLeft[0][0][1], controlPointsLeft[0][0][2]);
	glTexCoord2f(1, 1); glVertex3f(controlPointsLeft[0][3][0], controlPointsLeft[0][3][1], controlPointsLeft[0][3][2]);
	glTexCoord2f(0, 1); glVertex3f(controlPointsLeft[5][3][0], controlPointsLeft[5][3][1], controlPointsLeft[5][3][2]);

	//draw bezier surface using mesh approximation
	glColor3f(1.0, 1.0, 1.0);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);


	//Right Curtain
	//specify and enable bezier surface
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 6, controlPointsRight[0][0]);
	glEnable(GL_MAP2_VERTEX_3);

	//draw bezier surface using mesh approximation
	glColor3f(1.0, 1.0, 1.0);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);

	glDisable(GL_AUTO_NORMAL);
	glDisable(GL_TEXTURE_2D);
}

void drawManyTrees() {
	//outside the window - draws the basic shape of a tree
	float matSky[] = { 0.5, 0.8, 1, 1 };
	float matearth[] = { 0.2,0.7,0.2,1 };
	double skyeqn[] = { 0, 0, 1, -far };


	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matSky);
	glClipPlane(GL_CLIP_PLANE0, skyeqn);

	//skybox- rotates slowly. 
	glPushMatrix();
	glTranslatef(0, 10, far + 10);
	glRotatef(skyAngle, 0, 1, 0);
	//bind sky texture to the sphere
	glBindTexture(GL_TEXTURE_2D, texture[6]);

	////// https://gamedev.stackexchange.com/questions/46931/texture-will-not-rotate-with-glusphere
	////// GL_OBJECT_LINEAR was used because I read this page

	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_2D);

	//sphere for the sky + clipping plane
	glEnable(GL_CLIP_PLANE0);
	//glutSolidCube(30);
	glutSolidSphere(30, 40, 40);
	glPopMatrix();
	glDisable(GL_CLIP_PLANE0);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);
	

	//ground
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matearth);
	glBegin(GL_POLYGON);
	glVertex3f(30, 0, far);
	glVertex3f(30, 0, far + 40);
	glVertex3f(-30, 0, far + 40);
	glVertex3f(-30, 0, far);
	glEnd();

	//draw the one cherry tree
	treeAngle =50;
	cherry = true;
	maxLevel = 5;
	glPushMatrix();
	glTranslatef(0, 0, far + 5);
	float coordsVal[4] = { l+15, 0.0, l+15, 4.0 };
	Source src = *new Source(coordsVal); // Vertical line segment.
	src.produceTree(0);
	cherry = false;

	//shadow
	glPushMatrix();
	shade = true;
	float coordsValS[4] = { l + 15, 0.0, l + 17, 4.0 };
	Source srcS = *new Source(coordsValS); // Vertical line segment.
	glRotatef(90, 1, 0, 0);
	srcS.produceTree(0);
	glPopMatrix();
	shade = false;

	glPopMatrix();
	treeAngle = 30; 

	//draw 15 trees placed and sized randomly
	for (int i = 0; i < 15; i++) {
		maxLevel = randomLevel[i];
		glPushMatrix();
		glTranslatef(-20, 0, 20 + randomZ[i]);
		float coordsVal1[4] = { randomTrees[i], 0.0, randomTrees[i], 2.0 };
		Source src1 = *new Source(coordsVal1); // Vertical line segment.
		src1.produceTree(0);

		//shadows
		glPushMatrix();
		shade = true;
		float coordsValShade[4] = { randomTrees[i], 0.0, randomTrees[i]+2, 2.0 };
		Source src1S = *new Source(coordsValShade); // Vertical line segment.
		glRotatef(90, 1, 0, 0);
		src1S.produceTree(0);
		glPopMatrix();
		shade = false;

		glPopMatrix();
	}

}

void drawSwitch() {
	//circle and cube for ligthswitch
	glColor3f(0.6, 0.6, 0);
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();

	glPushMatrix();
	glTranslatef(r-1.5, bot+10, far-0.1);
	//glRotatef(90, 0, 1, 0);
	gluCylinder(qobj, 0.4, 0.4, 0.2, 10, 10);

	glNormal3f(0, 0, -1);
	glPushMatrix();
	glScalef(1, 1, 0);
	glutSolidSphere(0.4, 10, 10);
	glPopMatrix();

	glPopMatrix();
	

	//if on up, if off down
	if (overheadLight) {
		//up
		//enable light1
		glEnable(GL_LIGHT1);
	}
	else {
		//down
	}
}

void drawClock() {
	float matClock[] = { 0.3, 0.2, 0.1, 1 };
	double clockeqn[] = { 0, 0, 1, 21 };
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();

	glClipPlane(GL_CLIP_PLANE1, clockeqn);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matClock);

	glPushMatrix();
	glTranslatef(r, 0, -7);
	glRotatef(270, 0, 1, 0);

	glPushMatrix();
	glScalef(1.2, 0.5, 1.2);
	glutSolidCube(3);
	glPopMatrix();

	glPushMatrix();
	glScalef(1, 6, 1);
	glutSolidCube(3);
	glPopMatrix();

	glEnable(GL_CLIP_PLANE1);
	glPushMatrix();
	glTranslatef(0, 10, 0);
	gluCylinder(qobj, 1.5, 1.5, 1.5, 10, 10);

	glPushMatrix();
	glScalef(1, 1, 0);
	glTranslatef(0, 0, 1);
	glutSolidSphere(1.5, 10, 10);  //clock face
	glPopMatrix();

	glPopMatrix();
	glDisable(GL_CLIP_PLANE1);

	glPopMatrix();
}

void drawKitchen() {
	float matOven[] = { 0.2,0.2,0.2,1 };
	float matSink[] = { 1,0,1,1 };
	float matMetal[] = { 0.6,0.6,0.6,1 };
	float matFlame[] = { 1, 0.6, 0, 1 };
	float normalEmis[] = { 0,0,0,1 };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matOven);

	//oven and light from within it. 
	glPushMatrix();
	glTranslatef(l, 0, -7);
	glScalef(1, 2, 1);
	glutSolidCube(4);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matFlame);
	glMaterialfv(GL_FRONT, GL_EMISSION, matFlame);

	glTranslatef(1.2, 0, 0);
	glutSolidCube(2);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_EMISSION, normalEmis);

	//sink. 

	//faucet. 

}



void drawCube() {
	float matCube[] = { 0,0,0.2,1 };

	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matCube);
	glDisable(GL_LIGHTING);

	glColor3f(0, 0, 0.2);
	glPushMatrix();
	glTranslatef(cubePoints[cubeRandom][0],cubePoints[cubeRandom][1],cubePoints[cubeRandom][2]);
	glutSolidCube(0.5);
	glPopMatrix();

	glEnable(GL_LIGHTING);
};

void drawScene() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glDisable(GL_LIGHTING);

	gluLookAt(meX, meY, meZ,
		meX + sin(angle * PI / 180), meY, meZ + cos(angle * PI / 180),
		//meX, meY, meZ + 1.0,
		0, 1, 0);

	glDisable(GL_LIGHTING);
	drawWalls();
	glEnable(GL_LIGHTING);

	drawKitchen();
	drawCurtains();
	drawSwitch();
	drawClock();
	drawManyTrees();
	drawCube();
	drawWindow();


	glutSwapBuffers();

}

void animate() {
	//rotate the sky sphere
	if (skyAngle >= 360) {
		skyAngle -= 360; //keep the number below 360 for my own sanity
	}
	skyAngle += 0.1;
	glutPostRedisplay();
}

void setupWindow() {

	glClearColor(1, 1, 1, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	float lightAmb[] = { 0.5, 0.5, 0.5, 0.6 };
	float lightDifAndSpec[] = { 1.0, 1.0, 1.0, 0.6 };
	float lightPos[] = { 0.0, 1.5, 3.0, 1.0 };
	float globAmb[] = { 0.2, 0.2, 0.2, 1.0 };



	// Light properties.
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.


	//spotlight for roof light
	float lightPos1[] = { 0.0, top, 3.0, 1.0 }; // Spotlight position.
	float spotDirection[] = { 0.0, -1.0, 0.0 }; // Spotlight direction.   

	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);

	//enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Specify blending parameters.
	//glDisable(GL_BLEND);

	//enable Textures
	// Create texture index array.
	glGenTextures(7, texture);			//textureEdit

	// Load external texture and generate and load procedural texture.
	loadExternalTextures();
	createChessboard();
	loadProceduralTextures();

	// Turn on OpenGL texturing.
	glEnable(GL_TEXTURE_2D);

	// Specify how texture values combine with current surface color values.
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glDisable(GL_TEXTURE_2D);

	srand(time(NULL));
	//generate random locations for the trees
	//generate random levels for the trees - shorter and taller
	for (int i = 0; i < 15; i++) {
		randomTrees[i] = rand() % 50;
		randomLevel[i] = rand() % 5 + 1;
		randomZ[i] = rand() % 5;
	}

	srand(time(NULL));
	cubeRandom = rand() % 4; //which set of cube coords are we using
	//cubeRandom = 4;
}

void reshapeWindow(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120, w/h, 3, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}


void keyInput(unsigned char key, int x, int y) {
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'o':
		//turn on spotlight
		overheadLight = !overheadLight;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

//Console window output
void consoleOutput() {

	cout << "Interactions:" << endl;
	cout << "Press escape key to quit." << endl;
	cout << "Arrow keys to move and turn." << endl;
	

}

void specialKeyInput(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP: //forward
		//collision detection
		if (meX + stepsize * sin(angle * PI / 180.0) <= l+3.0 || meX + stepsize * sin(angle * PI / 180.0) >= r-3.0 
			|| meZ + stepsize * cos(angle * PI / 180.0) <= near+3.0 || meZ + stepsize * cos(angle * PI / 180.0) >= far-3.0) {
			break;
		}
		meZ += stepsize * cos(angle * PI / 180.0);
		meX += stepsize * sin(angle * PI / 180.0);
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN: //back
		//collision detection
		if (meX - stepsize * sin(angle * PI / 180.0) <= l + 3.0 || meX - stepsize * sin(angle * PI / 180.0) >= r - 3.0
			|| meZ - stepsize * cos(angle * PI / 180.0) <= near + 3.0 || meZ - stepsize * cos(angle * PI / 180.0) >= far - 3.0) {
			break;
		}
		meZ -= stepsize * cos(angle * PI / 180);
		meX -= stepsize * sin(angle * PI / 180);
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT: //turn right
		angle -= turnsize;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT: //turn left
		angle += turnsize;
		glutPostRedisplay();
		break;
	}//end switch
	glutPostRedisplay();
}

//mouse input
void mouseInput(int button, int state, int x, int y) {
	float color[4];

	//click the black cube which will close the game. 
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, color);
		//printf("R: %f G: %f, B: %f ", color[0], color[1], color[2]);

		if (color[0] == 0 && color[1] ==0 && color[2] == 0.2) {
			printf("Well Done!");
			//end the game
			//exit(0);
		}
	}
}


//Main
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	consoleOutput();
	//define overhead window
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(25, 75);
	glutCreateWindow("Seasons");
	setupWindow();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(reshapeWindow);
	glutMouseFunc(mouseInput);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);
	glutIdleFunc(animate);

	glutMainLoop();

	return 0;
}