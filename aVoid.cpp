#include <iostream>
#include <chrono>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <map>
#include <glut.h>
#include "assimp.h"
#include "aiPostProcess.h"
#include "aiScene.h"
#include "il.h"
#include <irrKlang.h>

using namespace std;
using namespace std::chrono;
using namespace irrklang;

ISoundEngine* SoundEngine = createIrrKlangDevice();		 //Sound 

irrklang::ISoundSource* backMusic = SoundEngine->addSoundSourceFromFile("audio/backMusic.wav");
irrklang::ISoundSource* power_up = SoundEngine->addSoundSourceFromFile("audio/power_up.wav");
irrklang::ISoundSource* power_down = SoundEngine->addSoundSourceFromFile("audio/power_down.wav");
irrklang::ISoundSource* game_over = SoundEngine->addSoundSourceFromFile("audio/game_over.wav");
irrklang::ISoundSource* menuBack = SoundEngine->addSoundSourceFromFile("audio/menuBack.wav");


int velPlayer = 5;			//player velocity
int enemyVelocity = 5;		//enemy velocity
float xWingCoord = 0;		//XWING COORDINATE
float zWingCoord = 0;		//XWING COORDINATE
float xPauseCoord = 1500;	//PAUSE COORDINATE
float zPauseCoord = 1500;	//PAUSE COORDINATE
float zWingRot = 0;			//XWING MOVE ROTATION AXIS Z
float xWingRot = 0;			//XWING MOVE ROTATION AXIS X
float xPauseRot = 0;		//PAUSE ROTATION AXIS X

int flagz = 0;				//flag for return xWing in position
int flagx = 0;				//flag for return xWing in position 
bool pause = false;			//PAUSE

int collision;				//result of checkCollision
int shield = 0;				//numbers of shield
bool timetoblink = false;	//if there is a collision with shield
int counterblink = 50;		//How many times xWing blink
float xWingCoordBack;		//backup for xWingCoord
float zWingCoordBack;		//backup for zWingCoord
int flagedit = 0;			//for keyboard input

int xPrincipalMenu = 0;		//PRINCIPAL MENU COORDINATE
int zPrincipalMenu = -700;	//PRINCIPAL MENU COORDINATE

int xPauseMenu = 0;			//PAUSE MENU COORDINATE
int zPauseMenu = -700;		//PAUSE MENU COORDINATE

int xLoseMenu = 0;			//LOSE MENU COORDINATE
int zLoseMenu = -700;		//LOSE MENU COORDINATE

int xExit = 0;				//EXIT BUTTON COORDINATE
int zExit = -700;			//EXIT BUTTON COORDINATE

char ShieldMsg[50];			//Shield message
char PointMsg[50];			//Point message
char WhatPowerUp[255];		//PowerUp message
int PowerUpPers = 100;
int point = 0;				//point counter
int levelUp = 0;			//Level UP counter

char CreditsMsg1[255] = "Project developed during the course Informatica Grafica A.A. 2017/2018 of Politecnico di Torino";
char CreditsMsg2[255] = "Created By:";
char CreditsMsg3[255] = "Francesco Marra";
char CreditsMsg4[255] = "Alessandro Picardi";
char CreditsMsg5[255] = "Simone Terzuolo";
char TutorialMsg1[255] = "Press 'W' 'A' 'S' 'D' or arrow keys to move";
char TutorialMsg2[255] = "Press 'Esc' or 'P' to pause";
char TutorialMsg3[255] = "Choose wisely your PowerUp";
char TutorialMsg4[255] = "aVoid the spaceships of the enemies";
char HighScoreMsg1[255] = "SCOREBOARD:";
char HighScoreMsg2[255];
char HighScoreMsg3[255];
char HighScoreMsg4[255];
char LastGame[255];
int HighScore[3];

int xShieldCoord;			//SHIELD COORDINATE
int zShieldCoord;			//SHIELD COORDINATE
int ShieldRot = 0;			//SHIELD ROTATION

int zEnemyUpCoord;			//ENEMY UP COORDINATE
int xEnemyUpCoord;			//ENEMY UP COORDINATE
int EnemyUpRot = 0;			//ENEMY UP ROTATION

int zEnemyDownCoord;		//ENEMY DOWN COORDINATE
int xEnemyDownCoord;		//ENEMY DOWN COORDINATE
int EnemyDownRot = 0;		//ENEMY DOWN ROTATION

int zPlayerUpCoord;			//PLAYER UP COORDINATE
int xPlayerUpCoord;			//PLAYER UP COORDINATE
int PlayerUpRot = 0;		//PLAYER UP ROTATION

int zPlayerDownCoord;		//PLAYER DOWN COORDINATE
int xPlayerDownCoord;		//PLAYER DOWN COORDINATE
int PlayerDownRot = 0;		//PLAYER DOWN ROTATION

int xtFightIntCoord[10];	//ARRAY OF TFIGHTER COORDINATE
int ztFightIntCoord[10];	//ARRAY OF TFIGHTER COORDINATE

int start = 0;				//STATE OF GAME

char LastChar = '?';
bool FirstTime = true;
int FramesEgg = 1000;
int TfightRot[10];

int window;

//border of my screen 1920x1080 in my room
//this border works good with that screen (for my laptop zoom a bit the camera)
int borderScreenUp = -150;
int borderScreenDown = 50;
int borderScreenLeft = -220;
int borderScreenRight = 220;

// currently this is hardcoded
//static const std::string basepath = "./models/textures/"; //obj..
static const std::string basepath = "./models/"; //per i file blend 

// the global Assimp scene object
const struct aiScene* scene = NULL;
GLuint scene_list = 0;
struct aiVector3D scene_min, scene_max, scene_center;

// current rotation angle
static float angle = 0.f;
static float xAsd = 0.f;	//TRANSLATE variable
static float yAsd = 0.f;
static float zAsd = 0.f;

// images / texture
std::map<std::string, GLuint*> textureIdMap;	// map image filenames to textureIds
GLuint* textureIds;							// pointer to texture Array

GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[] = { 0.0f, 0.0f, 15.0f, 1.0f };

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)
#define TRUE                1
#define FALSE               0

int randomizeX() {
	return (rand() % (borderScreenRight * glutGet(GLUT_WINDOW_WIDTH) / 1920 * 2)) - borderScreenRight * glutGet(GLUT_WINDOW_WIDTH) / 1920;
}

int randomizeZ() {
	int z = 0;
	int min = 0;

	for (int i = 0; i < 5; i++) {
		if (ztFightIntCoord[i] < min) min = ztFightIntCoord[i];
	}
	if (zShieldCoord < min) min = zShieldCoord;
	if (zEnemyUpCoord < min) min = zEnemyUpCoord;
	if (zEnemyDownCoord < min) min = zEnemyDownCoord;
	if (zPlayerUpCoord < min) min = zPlayerUpCoord;
	if (zPlayerDownCoord < min) min = zPlayerDownCoord;

	z = min - 150 - (rand() % 500);

	return z;
}

int checkCollisionInterceptor() {
	int sx = 0;
	int dx = 0;
	int up = 0;
	int down = 0;

	if (timetoblink == true) return 1;

	for (int i = 0; i < 5; i++) {
		sx = xtFightIntCoord[i] - 25;
		dx = xtFightIntCoord[i] + 25;
		down = ztFightIntCoord[i] + 25;
		up = ztFightIntCoord[i] - 25;
		if (down >= zWingCoord - 55 && down <= zWingCoord + 25) {
			if (sx >= xWingCoord - 30 && sx <= xWingCoord + 30) {
				if (shield > 0) {
					shield--;
					return 2;
				}
				return 0;
			}
			if (dx <= xWingCoord + 30 && dx >= xWingCoord - 30) {
				if (shield > 0) {
					shield--;
					return 2;
				}
				return 0;
			}
		}
		if (up <= zWingCoord + 25 && up >= zWingCoord - 55) {
			if (sx >= xWingCoord - 30 && sx <= xWingCoord + 30) {
				if (shield > 0) {
					shield--;
					return 2;
				}
				return 0;
			}
			if (dx <= xWingCoord + 30 && dx >= xWingCoord - 30) {
				if (shield > 0) {
					shield--;
					return 2;
				}
				return 0;
			}
		}
	}
	return 1;

}

int checkCollisionPowerUp(int x, int z) {
	int sx = x - 17;
	int dx = x + 17;
	int down = z + 17;
	int up = z - 17;
	if (down >= zWingCoord - 55 && down <= zWingCoord + 25) {
		if (sx >= xWingCoord - 30 && sx <= xWingCoord + 30) {
			return 1;
		}
		if (dx <= xWingCoord + 30 && dx >= xWingCoord - 30) {
			return 1;
		}
	}
	if (up <= zWingCoord + 25 && up >= zWingCoord - 55) {
		if (sx >= xWingCoord - 30 && sx <= xWingCoord + 30) {
			return 1;
		}
		if (dx <= xWingCoord + 30 && dx >= xWingCoord - 30) {
			return 1;
		}
	}
	return 0;
}

void resetStart() {
	ztFightIntCoord[0] = -1000;
	xtFightIntCoord[0] = randomizeX();

	ztFightIntCoord[1] = -1500;
	xtFightIntCoord[1] = randomizeX();

	ztFightIntCoord[2] = -2000;
	xtFightIntCoord[2] = randomizeX();

	ztFightIntCoord[3] = -2500;
	xtFightIntCoord[3] = randomizeX();

	ztFightIntCoord[4] = -3000;
	xtFightIntCoord[4] = randomizeX();

	zShieldCoord = 400;
	xShieldCoord = randomizeX();

	zEnemyUpCoord = 500;
	xEnemyUpCoord = randomizeX();

	zEnemyDownCoord = 600;
	xEnemyDownCoord = randomizeX();

	zPlayerUpCoord = 700;
	xPlayerUpCoord = randomizeX();

	zPlayerDownCoord = 800;
	xPlayerDownCoord = randomizeX();

	zWingRot = 0;
	xWingRot = 0;
	velPlayer = 5;
	enemyVelocity = 5;
	xWingCoord = 0;
	zWingCoord = 0;
	point = 0;
	shield = 1;
	LastChar = '?';
	FirstTime = true;
	timetoblink = false;
	FramesEgg = 1000;
	sprintf_s(WhatPowerUp, "");
}

//don't touch this function
void reshape(int width, int height)
{
	const double aspectRatio = (float)width / height, fieldOfView = 45.0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, aspectRatio,
		1.0, 1000.0);  // Znear and Zfar 
	glViewport(0, 0, width, height);
}

//don't touch this function
void get_bounding_box_for_node(const struct aiNode* nd, struct aiVector3D* min, struct aiVector3D* max, struct aiMatrix4x4* trafo) {
	struct aiMatrix4x4 prev;
	unsigned int n = 0, t;
	prev = *trafo;
	aiMultiplyMatrix4(trafo, &nd->mTransformation);
	for (; n < nd->mNumMeshes; ++n) {
		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {
			struct aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp, trafo);
			min->x = aisgl_min(min->x, tmp.x);
			min->y = aisgl_min(min->y, tmp.y);
			min->z = aisgl_min(min->z, tmp.z);
			max->x = aisgl_max(max->x, tmp.x);
			max->y = aisgl_max(max->y, tmp.y);
			max->z = aisgl_max(max->z, tmp.z);
		}
	}
	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(nd->mChildren[n], min, max, trafo);
	}
	*trafo = prev;
}

//don't touch this function
void get_bounding_box(struct aiVector3D* min, struct aiVector3D* max)
{
	struct aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	min->x = min->y = min->z = 1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene->mRootNode, min, max, &trafo);
}

void color4_to_float4(const struct aiColor4D* c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

//don't touch this function
void apply_material(const struct aiMaterial* mtl)
{
	float c[4];
	GLenum fill_mode;
	int ret1, ret2;
	struct aiColor4D diffuse;
	struct aiColor4D specular;
	struct aiColor4D ambient;
	struct aiColor4D emission;
	float shininess, strength;
	int two_sided;
	int wireframe;
	int max;
	int texIndex = 0;
	aiString texPath;	//contains filename of texture
	if (AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath))
	{
		//bind texture
		unsigned int texId = *textureIdMap[texPath.data];
		glBindTexture(GL_TEXTURE_2D, texId);
	}
	set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		color4_to_float4(&diffuse, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);
	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
		color4_to_float4(&specular, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
	set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
		color4_to_float4(&ambient, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);
	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
		color4_to_float4(&emission, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);
	max = 1;
	ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, (unsigned int*)& max);
	max = 1;
	ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, (unsigned int*)& max);
	if ((ret1 == AI_SUCCESS) && (ret2 == AI_SUCCESS))
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
	else {
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
		set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
	}
	max = 1;
	if (AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, (unsigned int*)& max))
		fill_mode = wireframe ? GL_LINE : GL_FILL;
	fill_mode = GL_FILL;
	glPolygonMode(GL_FRONT_AND_BACK, fill_mode);
	max = 1;
	if ((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, (unsigned int*)& max)) && two_sided)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

// Can't send color down as a pointer to aiColor4D because AI colors are ABGR.
void Color4f(const struct aiColor4D* color)
{
	glColor4f(color->r, color->g, color->b, color->a);
}

//don't touch this function
void recursive_render(const struct aiScene* sc, const struct aiNode* nd, float scale)
{
	unsigned int i;
	unsigned int n = 0, t;
	struct aiMatrix4x4 m = nd->mTransformation;
	printf("Node name: %s\n", nd->mName.data);
	//m.Scaling(aiVector3D(scale, scale, scale), m);
	// update transform
	m.Transpose();
	glPushMatrix();
	glMultMatrixf((float*)& m);
	// draw all meshes assigned to this node
	for (; n < nd->mNumMeshes; ++n)
	{
		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		printf("Drawing MESH with this name: %s\n", mesh->mName.data);
		apply_material(sc->mMaterials[mesh->mMaterialIndex]);
		if (mesh->HasTextureCoords(0))
			glEnable(GL_TEXTURE_2D);
		else
			glDisable(GL_TEXTURE_2D);
		if (mesh->mNormals == NULL)
			glDisable(GL_LIGHTING);
		else
			glEnable(GL_LIGHTING);
		if (mesh->mColors[0] != NULL)
			glEnable(GL_COLOR_MATERIAL);
		else
			glDisable(GL_COLOR_MATERIAL);
		for (t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];
			GLenum face_mode;
			switch (face->mNumIndices)
			{
			case 1: face_mode = GL_POINTS; break;
			case 2: face_mode = GL_LINES; break;
			case 3: face_mode = GL_TRIANGLES; break;
			default: face_mode = GL_POLYGON; break;
			}
			glBegin(face_mode);
			for (i = 0; i < face->mNumIndices; i++)		// go through all vertices in face
			{
				int vertexIndex = face->mIndices[i];	// get group index for current index
				if (mesh->mColors[0] != NULL)
					Color4f(&mesh->mColors[0][vertexIndex]);
				if (mesh->mNormals != NULL)
					if (mesh->HasTextureCoords(0))		//HasTextureCoords(texture_coordinates_set)
					{
						glTexCoord2f(mesh->mTextureCoords[0][vertexIndex].x, 1 - mesh->mTextureCoords[0][vertexIndex].y); //mTextureCoords[channel][vertex]
					}
				glNormal3fv(&mesh->mNormals[vertexIndex].x);
				glVertex3fv(&mesh->mVertices[vertexIndex].x);
			}
			glEnd();
		}

	}
	// draw all children
	for (n = 0; n < nd->mNumChildren; ++n)
	{
		recursive_render(sc, nd->mChildren[n], scale);
	}

	glPopMatrix();
}


// ------------------------------------------------------//
// ----------------------- DO MOTION --------------------//
// ------------------------------------------------------//
void do_motion(void)
{
	static GLint prev_time = 0;
	angle = 1;
	int time = glutGet(GLUT_ELAPSED_TIME);
	prev_time = time;
	glutPostRedisplay();
}


// ----------------------------------------------------------//
// --------------------------- DISPLAY ----------------------//
// ----------------------------------------------------------//
void display(void)
{
	float tmp;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// Camera
	gluLookAt(0.f, 2.f, 4.f, 0.f, 0.f, -5.f, 0.f, 1.f, 0.f);  //old with isometric view from behind
	//gluLookAt(eye,  at/center,  up);
	//(x y z)
	//gluLookAt(0.f, 10.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, -1.f);  //view from top

	// scale the whole asset to fit into our view frustum 
	tmp = scene_max.x - scene_min.x;
	tmp = aisgl_max(scene_max.y - scene_min.y, tmp);
	tmp = aisgl_max(scene_max.z - scene_min.z, tmp);
	tmp = 1.f / tmp;
	glScalef(tmp, tmp, tmp);

	//GAME
	if (start == 1) {

		glDisable(GL_LIGHTING);
		//SHIELD
		glPushMatrix();
		glTranslatef(xShieldCoord, -0.0, zShieldCoord);
		glRotatef(90, 1, 0, 0);
		glRotatef(ShieldRot, 0, 0, 1);
		recursive_render(scene, scene->mRootNode->mChildren[3], 1.0);
		glPopMatrix();

		//XWING
		glPushMatrix();
		glTranslatef(xWingCoord, 0, zWingCoord);
		glRotatef(zWingRot, 0, 0, 1);
		glRotatef(xWingRot, 1, 0, 0);
		recursive_render(scene, scene->mRootNode->mChildren[4], 1.0);
		glPopMatrix();

		//TIEFIGHTERINTERCEPTOR
		for (int i = 0; i < 5; i++) {
			glPushMatrix();
			glTranslatef(xtFightIntCoord[i], 0, ztFightIntCoord[i]);
			glRotated(180, 0, 1, 0);
			recursive_render(scene, scene->mRootNode->mChildren[5 + i], 1.0);
			glPopMatrix();
		}

		//PAUSE
		glPushMatrix();
		glTranslatef(xPauseCoord, 0, zPauseCoord);
		glRotatef(xPauseRot, 1, 0, 0);
		recursive_render(scene, scene->mRootNode->mChildren[10], 1.0);
		glPopMatrix();

		//ENEMY_UP
		glPushMatrix();
		glTranslatef(xEnemyUpCoord, -0.0, zEnemyUpCoord);
		glRotatef(90, 1, 0, 0);
		glRotatef(EnemyUpRot, 0, 0, 1);
		recursive_render(scene, scene->mRootNode->mChildren[11], 1.0);
		glPopMatrix();

		//ENEMY DOWN
		glPushMatrix();
		glTranslatef(xEnemyDownCoord, -0.0, zEnemyDownCoord);
		glRotatef(90, 1, 0, 0);
		glRotatef(EnemyDownRot, 0, 0, 1);
		recursive_render(scene, scene->mRootNode->mChildren[12], 1.0);
		glPopMatrix();

		//PLAYER UP
		glPushMatrix();
		glTranslatef(xPlayerUpCoord, -0.0, zPlayerUpCoord);
		glRotatef(90, 1, 0, 0);
		glRotatef(PlayerUpRot, 0, 0, 1);
		recursive_render(scene, scene->mRootNode->mChildren[13], 1.0);
		glPopMatrix();

		//PLAYER DOWN
		glPushMatrix();
		glTranslatef(xPlayerDownCoord, -0.0, zPlayerDownCoord);
		glRotatef(90, 1, 0, 0);
		glRotatef(PlayerDownRot, 0, 0, 1);
		recursive_render(scene, scene->mRootNode->mChildren[14], 1.0);
		glPopMatrix();

		//POINT MESSAGE
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glDisable(GL_LIGHTING);
		glRasterPos2d(-200 * glutGet(GLUT_WINDOW_WIDTH) / 1920, 200);
		for (unsigned int i = 0; i < strlen(PointMsg); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, PointMsg[i]);
		}
		glPopMatrix();

		//SHIELD MESSAGE
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-203 * glutGet(GLUT_WINDOW_WIDTH) / 1920, 180);
		for (unsigned int i = 0; i < strlen(ShieldMsg); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ShieldMsg[i]);
		}
		glPopMatrix();

		//POWER UP MESSAGE
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glDisable(GL_LIGHTING);
		glRasterPos2d(-205 * glutGet(GLUT_WINDOW_WIDTH) / 1920, 160);
		for (unsigned int i = 0; i < strlen(WhatPowerUp); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, WhatPowerUp[i]);
		}
		glPopMatrix();

		glEnable(GL_LIGHTING);

		glutSwapBuffers();

		do_motion();
	}

	//PRINCIPAL MENU
	if (start == 0) {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT1);

		glPushMatrix();
		glTranslatef(xPrincipalMenu, -100, zPrincipalMenu);
		glRotatef(90, 1, 0, 0);
		glScalef(12, 4, 12);
		recursive_render(scene, scene->mRootNode->mChildren[2], 1.0);
		glPopMatrix();

		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);

		glutSwapBuffers();

		do_motion();
	}

	//PAUSE MENU
	if (start == 2) {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT1);

		glPushMatrix();
		glTranslatef(xPrincipalMenu, -100, zPrincipalMenu);
		glRotatef(90, 1, 0, 0);
		glScalef(12, 4, 12);
		recursive_render(scene, scene->mRootNode->mChildren[1], 1.0);
		glPopMatrix();

		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);

		glutSwapBuffers();

		do_motion();
	}

	//LOSE MENU
	if (start == 3) {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT1);


		glPushMatrix();
		glTranslatef(xPrincipalMenu, -100, zPrincipalMenu);
		glRotatef(90, 1, 0, 0);
		glScalef(12, 4, 12);
		recursive_render(scene, scene->mRootNode->mChildren[0], 1.0);
		glPopMatrix();

		//POINT MESSAGE
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glDisable(GL_LIGHTING);
		glRasterPos2d(-200 * glutGet(GLUT_WINDOW_WIDTH) / 1920, 200);
		for (unsigned int i = 0; i < strlen(PointMsg); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, PointMsg[i]);
		}
		glPopMatrix();

		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);
		glutSwapBuffers();

		do_motion();
	}

	//HIGH SCORE PAGE
	if (start == 4) {

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT1);

		glPushMatrix();
		glTranslatef(xExit, -350, zExit);
		glRotatef(90, 1, 0, 0);
		glScalef(12, 4, 12);
		recursive_render(scene, scene->mRootNode->mChildren[15], 1.0);
		glPopMatrix();

		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glDisable(GL_LIGHTING);
		glRasterPos2d(-20, 175);
		for (unsigned int i = 0; i < strlen(HighScoreMsg1); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, HighScoreMsg1[i]);
		}
		glPopMatrix();
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-20, 160);
		for (unsigned int i = 0; i < strlen(HighScoreMsg2); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, HighScoreMsg2[i]);
		}
		glPopMatrix();
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-20, 145);
		for (unsigned int i = 0; i < strlen(HighScoreMsg3); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, HighScoreMsg3[i]);
		}
		glPopMatrix();
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-20, 130);
		for (unsigned int i = 0; i < strlen(HighScoreMsg4); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, HighScoreMsg4[i]);
		}
		glPopMatrix();

		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-20, 115);
		for (unsigned int i = 0; i < strlen(LastGame); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, LastGame[i]);
		}
		glPopMatrix();

		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);
		glutSwapBuffers();

		do_motion();
	}

	//TUTORIAL PAGE
	if (start == 5) {

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT1);

		glPushMatrix();
		glTranslatef(xExit, -350, zExit);
		glRotatef(90, 1, 0, 0);
		glScalef(12, 4, 12);
		recursive_render(scene, scene->mRootNode->mChildren[15], 1.0);
		glPopMatrix();

		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glDisable(GL_LIGHTING);
		glRasterPos2d(-53, 175);
		for (unsigned int i = 0; i < strlen(TutorialMsg1); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, TutorialMsg1[i]);
		}
		glPopMatrix();
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-31, 160);
		for (unsigned int i = 0; i < strlen(TutorialMsg2); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, TutorialMsg2[i]);
		}
		glPopMatrix();
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-31, 145);
		for (unsigned int i = 0; i < strlen(TutorialMsg3); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, TutorialMsg3[i]);
		}
		glPopMatrix();
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-43, 130);
		for (unsigned int i = 0; i < strlen(TutorialMsg4); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, TutorialMsg4[i]);
		}
		glPopMatrix();

		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);
		glutSwapBuffers();

		do_motion();
	}

	//CREDITS PAGE
	if (start == 6) {

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT1);

		glPushMatrix();
		glTranslatef(xExit, -350, zExit);
		glRotatef(90, 1, 0, 0);
		glScalef(12, 4, 12);
		recursive_render(scene, scene->mRootNode->mChildren[15], 1.0);
		glPopMatrix();



		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glDisable(GL_LIGHTING);
		glRasterPos2d(-120, 175);
		for (unsigned int i = 0; i < strlen(CreditsMsg1); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, CreditsMsg1[i]);
		}
		glPopMatrix();
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-13, 160);
		for (unsigned int i = 0; i < strlen(CreditsMsg2); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, CreditsMsg2[i]);
		}
		glPopMatrix();
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-20, 145);
		for (unsigned int i = 0; i < strlen(CreditsMsg3); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, CreditsMsg3[i]);
		}
		glPopMatrix();
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-22, 130);
		for (unsigned int i = 0; i < strlen(CreditsMsg4); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, CreditsMsg4[i]);
		}
		glPopMatrix();
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-19, 115);
		for (unsigned int i = 0; i < strlen(CreditsMsg5); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, CreditsMsg5[i]);
		}
		glPopMatrix();

		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);
		glutSwapBuffers();

		do_motion();
	}

	if (start == 7) {			//EASTER EGG

		glDisable(GL_LIGHTING);

		//SHIELD
		glPushMatrix();
		glTranslatef(xShieldCoord, -0.0, zShieldCoord);
		glRotatef(90, 1, 0, 0);
		glRotatef(ShieldRot, 0, 0, 1);
		recursive_render(scene, scene->mRootNode->mChildren[3], 1.0);
		glPopMatrix();

		//XWING
		glPushMatrix();
		glTranslatef(xWingCoord, 0, zWingCoord);
		glRotatef(zWingRot, 0, 0, 1);
		glRotatef(xWingRot, 1, 0, 0);
		recursive_render(scene, scene->mRootNode->mChildren[4], 1.0);
		glPopMatrix();

		//TIEFIGHTERINTERCEPTOR
		for (int i = 0; i < 5; i++) {
			glPushMatrix();
			glTranslatef(xtFightIntCoord[i], 0, ztFightIntCoord[i]);
			glRotatef(90, 1, 0, 0);
			glRotatef(TfightRot[i], 0, 0, 1);
			recursive_render(scene, scene->mRootNode->mChildren[3], 1.0);
			glPopMatrix();
		}

		//ENEMY_UP
		glPushMatrix();
		glTranslatef(xEnemyUpCoord, -0.0, zEnemyUpCoord);
		glRotatef(90, 1, 0, 0);
		glRotatef(EnemyUpRot, 0, 0, 1);
		recursive_render(scene, scene->mRootNode->mChildren[3], 1.0);
		glPopMatrix();

		//ENEMY DOWN
		glPushMatrix();
		glTranslatef(xEnemyDownCoord, -0.0, zEnemyDownCoord);
		glRotatef(90, 1, 0, 0);
		glRotatef(EnemyDownRot, 0, 0, 1);
		recursive_render(scene, scene->mRootNode->mChildren[3], 1.0);
		glPopMatrix();

		//PLAYER UP
		glPushMatrix();
		glTranslatef(xPlayerUpCoord, -0.0, zPlayerUpCoord);
		glRotatef(90, 1, 0, 0);
		glRotatef(PlayerUpRot, 0, 0, 1);
		recursive_render(scene, scene->mRootNode->mChildren[3], 1.0);
		glPopMatrix();

		//PLAYER DOWN
		glPushMatrix();
		glTranslatef(xPlayerDownCoord, -0.0, zPlayerDownCoord);
		glRotatef(90, 1, 0, 0);
		glRotatef(PlayerDownRot, 0, 0, 1);
		recursive_render(scene, scene->mRootNode->mChildren[3], 1.0);
		glPopMatrix();

		//POINT MESSAGE
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glDisable(GL_LIGHTING);
		glRasterPos2d(-200 * glutGet(GLUT_WINDOW_WIDTH) / 1920, 200);
		for (unsigned int i = 0; i < strlen(PointMsg); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, PointMsg[i]);
		}
		glPopMatrix();

		//SHIELD MESSAGE
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glRasterPos2d(-203 * glutGet(GLUT_WINDOW_WIDTH) / 1920, 180);
		for (unsigned int i = 0; i < strlen(ShieldMsg); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ShieldMsg[i]);
		}
		glPopMatrix();

		//POWER UP MESSAGE
		glPushMatrix();
		glColor3f(1, 0.68, 0);
		glDisable(GL_LIGHTING);
		glRasterPos2d(-205 * glutGet(GLUT_WINDOW_WIDTH) / 1920, 160);
		for (unsigned int i = 0; i < strlen(WhatPowerUp); i++) {
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, WhatPowerUp[i]);
		}
		glPopMatrix();

		glEnable(GL_LIGHTING);

		glutSwapBuffers();

		do_motion();
	}
}


// ------------------------------------------------------//
// ----------------------- KEYBOARD ---------------------//
// ------------------------------------------------------//
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'i':
		if (start == 1) {
			if (pause == false) {
				LastChar = 'i';
			}
		}
		break;
	case 'n':
		if (start == 1) {
			if (pause == false) {
				if (LastChar == 'i') LastChar = 'n';
				else LastChar = '?';
			}
		}
		break;
	case 'f':
		if (start == 1) {
			if (pause == false) {
				if (LastChar == 'n') LastChar = 'f';
				else LastChar = '?';
			}
		}
		break;
	case 'o':
		if (start == 1) {
			if (pause == false) {
				if (LastChar == 'f') LastChar = 'o';
				else LastChar = '?';
			}
		}
		break;
	case 'g':
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (LastChar == 'o' && FirstTime == true) {
					LastChar = '?';
					FirstTime = false;
					start = 7;
					//sprintf_s(WhatPowerUp, "");
				}
				else LastChar = '?';
			}
		}
		break;
	case 'w':
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (flagedit == 0) {
					if (zWingCoord > borderScreenUp)
						zWingCoord -= velPlayer;
					glutPostRedisplay();
				}
				else {
					if (zWingCoordBack > borderScreenUp) zWingCoordBack -= velPlayer;
				}
				LastChar = '?';
			}
		}
		break;
	case 'W':
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (flagedit == 0) {
					if (zWingCoord > borderScreenUp)
						zWingCoord -= velPlayer;
					glutPostRedisplay();
				}
				else {
					if (zWingCoordBack > borderScreenUp) zWingCoordBack -= velPlayer;
				}
				LastChar = '?';
			}
		}
		break;
	case 's':
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (flagedit == 0) {
					if (zWingCoord < borderScreenDown)
						zWingCoord += velPlayer;
					glutPostRedisplay();
				}
				else {
					if (zWingCoordBack < borderScreenDown) zWingCoordBack += velPlayer;
				}
				LastChar = '?';
			}
		}
		break;
	case 'S':
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (flagedit == 0) {
					if (zWingCoord < borderScreenDown)
						zWingCoord += velPlayer;
					glutPostRedisplay();
				}
				else {
					if (zWingCoordBack < borderScreenDown) zWingCoordBack += velPlayer;
				}
				LastChar = '?';
			}
		}
		break;
	case 'a':
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (flagedit == 0) {
					if (xWingCoord > borderScreenLeft * glutGet(GLUT_WINDOW_WIDTH) / 1920) {
						xWingCoord -= velPlayer;
						zWingRot += 1;
						if (zWingRot > 45) zWingRot = 45;
						flagz = 0;
					}
					glutPostRedisplay();
				}
				else {
					if (xWingCoordBack > borderScreenLeft * glutGet(GLUT_WINDOW_WIDTH) / 1920) xWingCoordBack -= velPlayer;
				}
				LastChar = '?';
			}
		}
		break;
	case 'A':
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (flagedit == 0) {
					if (xWingCoord > borderScreenLeft * glutGet(GLUT_WINDOW_WIDTH) / 1920) {
						xWingCoord -= velPlayer;
						zWingRot += 1;
						if (zWingRot > 45) zWingRot = 45;
						flagz = 0;
					}
					glutPostRedisplay();
				}
				else {
					if (xWingCoordBack > borderScreenLeft * glutGet(GLUT_WINDOW_WIDTH) / 1920) xWingCoordBack -= velPlayer;
				}
				LastChar = '?';
			}
		}
		break;
	case 'd':
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (flagedit == 0) {
					if (xWingCoord < borderScreenRight * glutGet(GLUT_WINDOW_WIDTH) / 1920) {
						xWingCoord += velPlayer;
						zWingRot -= 1;
						if (zWingRot < -45) zWingRot = -45;
						flagz = 0;
					}
					glutPostRedisplay();
				}
				else {
					if (xWingCoordBack < borderScreenRight * glutGet(GLUT_WINDOW_WIDTH) / 1920) xWingCoordBack += velPlayer;
				}
				LastChar = '?';
			}
		}
		break;
	case 'D':
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (flagedit == 0) {
					if (xWingCoord < borderScreenRight * glutGet(GLUT_WINDOW_WIDTH) / 1920) {
						xWingCoord += velPlayer;
						zWingRot -= 1;
						if (zWingRot < -45) zWingRot = -45;
						flagz = 0;
					}
					glutPostRedisplay();
				}
				else {
					if (xWingCoordBack < borderScreenRight * glutGet(GLUT_WINDOW_WIDTH) / 1920) xWingCoordBack += velPlayer;
				}
				LastChar = '?';
			}
		}
		break;
	case 'p':
		if (start == 1 || start == 7) {
			/*if (pause == false) pause = true;
			else pause = false;*/
			start = 2;
			LastChar = '?';
		}
		else if (start == 2) {
			start = 1;
		}
		break;

	case 27:	//esc key
		if (start == 1 || start == 7) {
			start = 2;
			LastChar = '?';
		}
		else if (start == 2) {
			start = 1;
		}
		break;

	default:
		LastChar = '?';
		break;

	}
}



// ------------------------------------------------------//
// --------------------- SPECIAL INPUT ------------------//
// ------------------------------------------------------//
void SpecialInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (flagedit == 0) {
					if (zWingCoord > borderScreenUp)
						zWingCoord -= velPlayer;
					glutPostRedisplay();
				}
				else {
					if (zWingCoordBack > borderScreenUp) zWingCoordBack -= velPlayer;
				}
			}
		}
		break;

	case GLUT_KEY_DOWN:
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (flagedit == 0) {
					if (zWingCoord < borderScreenDown)
						zWingCoord += velPlayer;
					glutPostRedisplay();
				}
				else {
					if (zWingCoordBack < borderScreenDown) zWingCoordBack += velPlayer;
				}
			}
		}
		break;

	case GLUT_KEY_LEFT:
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (flagedit == 0) {
					if (xWingCoord > borderScreenLeft * glutGet(GLUT_WINDOW_WIDTH) / 1920) {
						xWingCoord -= velPlayer;
						zWingRot += 1;
						if (zWingRot > 45) zWingRot = 45;
						flagz = 0;
					}
					glutPostRedisplay();
				}
				else {
					if (xWingCoordBack > borderScreenLeft * glutGet(GLUT_WINDOW_WIDTH) / 1920) xWingCoordBack -= velPlayer;
				}
			}
		}
		break;

	case GLUT_KEY_RIGHT:
		if (start == 1 || start == 7) {
			if (pause == false) {
				if (flagedit == 0) {
					if (xWingCoord < borderScreenRight * glutGet(GLUT_WINDOW_WIDTH) / 1920) {
						xWingCoord += velPlayer;
						zWingRot -= 1;
						if (zWingRot < -45) zWingRot = -45;
						flagz = 0;
					}
					glutPostRedisplay();
				}
				else {
					if (xWingCoordBack < borderScreenRight * glutGet(GLUT_WINDOW_WIDTH) / 1920) xWingCoordBack += velPlayer;
				}
			}
		}
		break;
	default:
		LastChar = '?';
		break;
	}
}

void keyboard_up(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		if (start == 1 || start == 7) {
			if (pause == false) {
				flagz = 1;
			}
		}
		break;
	case 'd':
		if (start == 1 || start == 7) {
			if (pause == false) {
				flagz = 1;
			}
		}
		break;
	case 'A':
		if (start == 1 || start == 7) {
			if (pause == false) {
				flagz = 1;
			}
		}
		break;
	case 'D':
		if (start == 1 || start == 7) {
			if (pause == false) {
				flagz = 1;
			}
		}
		break;
	default:
		break;
	}
}

void SpecialInput_up(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		if (start == 1 || start == 7) {
			if (pause == false) {
				flagz = 1;
			}
		}
		break;
	case GLUT_KEY_RIGHT:
		if (start == 1 || start == 7) {
			if (pause == false) {
				flagz = 1;
			}
		}
		break;
	default:
		break;
	}
}

void Mouse(int button, int state, int x, int y) {
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (start == 0 && state == GLUT_UP) {	//PRINCIPAL MENU

			if (x >= (635 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (892 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (488 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (550 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {			//START GAME
				SoundEngine->stopAllSoundsOfSoundSource(menuBack);
				SoundEngine->play2D(backMusic, GL_TRUE);
				start = 1;
				resetStart();
			}

			if (x >= (1027 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (1284 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (488 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (550 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {		//HIGH SCORE
				start = 4;
			}

			if (x >= (834 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (1085 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (618 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (677 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {		//TUTORIAL
				start = 5;
			}

			if (x >= (649 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (896 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (737 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (795 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {			//CREDITS
				start = 6;
			}

			if (x >= (1024 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (1270 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (737 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (795 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {		//EXIT
				glutDestroyWindow(window);
				exit(0);
			}
		}
		if (start == 2 && state == GLUT_UP) {	//PAUSE MENU
			if (x >= (635 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (892 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (488 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (550 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {			//RESUME
				start = 1;
			}

			if (x >= (1027 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (1284 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (488 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (550 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {		//RESTART
				SoundEngine->stopAllSoundsOfSoundSource(backMusic);
				SoundEngine->play2D(backMusic, GL_TRUE);
				start = 1;
				resetStart();
			}

			if (x >= (836 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (1083 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (738 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (794 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {		//EXIT
				SoundEngine->stopAllSoundsOfSoundSource(backMusic);
				SoundEngine->play2D(menuBack, GL_TRUE);
				start = 0;
			}
		}
		if (start == 3 && state == GLUT_UP) {	//LOSE MENU
			if (x >= (831 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (1088 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (488 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (550 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {		//RESTART
				SoundEngine->stopAllSoundsOfSoundSource(game_over);
				SoundEngine->play2D(backMusic, GL_TRUE);
				start = 1;
				resetStart();
			}

			if (x >= (836 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (1083 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (738 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (794 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {		//EXIT
				SoundEngine->stopAllSoundsOfSoundSource(game_over);
				SoundEngine->play2D(menuBack, GL_TRUE);
				start = 0;
			}
		}

		if (start == 4 && state == GLUT_UP) {	//HIGH SCORE PAGE
			if (x >= (842 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (1084 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (840 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (895 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {		//EXIT
				start = 0;
			}
		}
		if (start == 5 && state == GLUT_UP) {	//TUTORIAL PAGE
			if (x >= (842 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (1084 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (840 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (895 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {		//EXIT
				start = 0;
			}
		}
		if (start == 6 && state == GLUT_UP) {	//CREDITS PAGE
			if (x >= (842 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && x <= (1084 * glutGet(GLUT_WINDOW_WIDTH) / 1920) && y >= (840 * glutGet(GLUT_WINDOW_HEIGHT) / 1080) && y <= (895 * glutGet(GLUT_WINDOW_HEIGHT) / 1080)) {		//EXIT
				start = 0;
			}
		}
	default:
		break;
	}
}

// ----------------------------------------------------------//
// ------------------------ IDLE FUNCTION -------------------//
// ----------------------------------------------------------//
void idleFunc(void)
{
	if (start == 1) {
		if (pause == false) {
			levelUp = point / 500;
			xPauseCoord = 1500;
			zPauseCoord = 1500;
			xPauseRot = 0;

			if (counterblink == 0) {
				timetoblink = false;
				counterblink = 50;
			}
			if (timetoblink == true) {
				if (counterblink % 2 == 0) {
					counterblink--;
					xWingCoordBack = xWingCoord;
					zWingCoordBack = zWingCoord;
					xWingCoord = 1500;
					zWingCoord = 1500;
					flagedit = 1;
				}
				else {
					counterblink--;
					xWingCoord = xWingCoordBack;
					zWingCoord = zWingCoordBack;
					flagedit = 0;
				}
			}

			if (flagz == 1) {
				if (zWingRot < 0) zWingRot += 1;			//ROTATION OF xWING
				if (zWingRot > 0) zWingRot -= 1;
			}

			for (int i = 0; i < 5; i++) {
				ztFightIntCoord[i] += enemyVelocity + levelUp;
				if (ztFightIntCoord[i] > 200) {
					ztFightIntCoord[i] = randomizeZ();
					xtFightIntCoord[i] = randomizeX();
				}
			}
			collision = checkCollisionInterceptor();
			if (collision == 0) {
				SoundEngine->stopAllSoundsOfSoundSource(backMusic);
				SoundEngine->play2D(game_over, GL_FALSE);
				start = 3;
				if (point >= HighScore[0]) {
					HighScore[2] = HighScore[1];
					HighScore[1] = HighScore[0];
					HighScore[0] = point;
				}
				else if (point >= HighScore[1] && point < HighScore[0]) {
					HighScore[2] = HighScore[1];
					HighScore[1] = point;
				}
				else if (point >= HighScore[2] && point < HighScore[1]) {
					HighScore[2] = point;
				}
				fstream myfile;
				myfile.open("./scoreboard.txt");
				myfile.clear();
				myfile << HighScore[0];
				myfile << '\n';
				myfile << HighScore[1];
				myfile << '\n';
				myfile << HighScore[2];
				myfile.close();
				sprintf_s(HighScoreMsg2, "FIRST: %d", HighScore[0]);
				sprintf_s(HighScoreMsg3, "SECOND: %d", HighScore[1]);
				sprintf_s(HighScoreMsg4, "THIRD: %d", HighScore[2]);
				sprintf_s(LastGame, "LAST GAME: %d", point);
			}
			else if (collision == 2) {
				SoundEngine->play2D("audio/hit.wav", GL_FALSE);
				timetoblink = true;
				sprintf_s(WhatPowerUp, "SYSTEM MESSAGE: SHIELD NUMBER -1");
				PowerUpPers = 300;
			}

			zShieldCoord += enemyVelocity + levelUp;
			ShieldRot += 1;
			if (zShieldCoord > 32165) {
				zShieldCoord = randomizeZ();
				xShieldCoord = randomizeX();
				ShieldRot = 0;
			}

			if (checkCollisionPowerUp(xShieldCoord, zShieldCoord) == 1) {
				zShieldCoord = 200;
				sprintf_s(WhatPowerUp, "SYSTEM MESSAGE: SHIELD NUMBER +1");
				PowerUpPers = 300;
				shield++;
				SoundEngine->play2D(power_up, GL_FALSE);
			}

			zEnemyUpCoord += enemyVelocity + levelUp;
			EnemyUpRot += 1;

			if (zEnemyUpCoord > 2345) {
				zEnemyUpCoord = randomizeZ();
				xEnemyUpCoord = randomizeX();
				EnemyUpRot = 0;
			}

			if (checkCollisionPowerUp(xEnemyUpCoord, zEnemyUpCoord) == 1) {
				SoundEngine->play2D(power_down, GL_FALSE);
				zEnemyUpCoord = 200;
				sprintf_s(WhatPowerUp, "SYSTEM MESSAGE: ENEMY VELOCITY UP");
				PowerUpPers = 300;
				enemyVelocity += 3;
			}

			zEnemyDownCoord += enemyVelocity + levelUp;
			EnemyDownRot += 1;
			if (zEnemyDownCoord > 8978) {
				zEnemyDownCoord = randomizeZ();
				xEnemyDownCoord = randomizeX();
				EnemyDownRot = 0;
			}

			if (checkCollisionPowerUp(xEnemyDownCoord, zEnemyDownCoord) == 1) {
				SoundEngine->play2D(power_up, GL_FALSE);
				zEnemyDownCoord = 200;
				sprintf_s(WhatPowerUp, "SYSTEM MESSAGE: ENEMY VELOCITY DOWN");
				PowerUpPers = 300;
				enemyVelocity -= 3;
				if (enemyVelocity <= 0) {
					enemyVelocity = 1;
				}
			}

			zPlayerUpCoord += enemyVelocity + levelUp;
			PlayerUpRot += 1;
			if (zPlayerUpCoord > 12345) {
				zPlayerUpCoord = randomizeZ();
				xPlayerUpCoord = randomizeX();
				PlayerUpRot = 0;
			}

			if (checkCollisionPowerUp(xPlayerUpCoord, zPlayerUpCoord) == 1) {
				SoundEngine->play2D(power_up, GL_FALSE);
				zPlayerUpCoord = 200;
				sprintf_s(WhatPowerUp, "SYSTEM MESSAGE: PLAYER VELOCITY UP");
				PowerUpPers = 300;
				velPlayer += 2;
			}

			zPlayerDownCoord += enemyVelocity + levelUp;
			PlayerDownRot += 1;
			if (zPlayerDownCoord > 3456) {
				zPlayerDownCoord = randomizeZ();
				xPlayerDownCoord = randomizeX();
				PlayerDownRot = 0;
			}

			if (checkCollisionPowerUp(xPlayerDownCoord, zPlayerDownCoord) == 1) {
				SoundEngine->play2D(power_down, GL_FALSE);
				zPlayerDownCoord = 200;
				sprintf_s(WhatPowerUp, "SYSTEM MESSAGE: PLAYER VELOCITY DOWN");
				PowerUpPers = 300;
				velPlayer -= 2;
				if (velPlayer <= 0) {
					velPlayer = 1;
				}
			}
			point++;
			sprintf_s(ShieldMsg, "SHIELD NUMBER: %d", shield);
			sprintf_s(PointMsg, "POINTS: %d", point);

			if (PowerUpPers > 0) {
				PowerUpPers--;
			}
			else {
				sprintf_s(WhatPowerUp, "");
			}
		}

		if (pause == true) {
			xPauseCoord = 0;
			zPauseCoord = -250;
			xPauseRot += 1;
		}
		glutPostRedisplay();
	}
	if (start == 7) {
		levelUp = point / 500;

		if (flagz == 1) {
			if (zWingRot < 0) zWingRot += 1;			//ROTATION OF xWING
			if (zWingRot > 0) zWingRot -= 1;
		}

		for (int i = 0; i < 5; i++) {
			ztFightIntCoord[i] += enemyVelocity + levelUp;
			TfightRot[i] += 1;
			if (ztFightIntCoord[i] > 200) {
				ztFightIntCoord[i] = randomizeZ();
				xtFightIntCoord[i] = randomizeX();
				TfightRot[i] = 0;
			}
			if (checkCollisionPowerUp(xtFightIntCoord[i], ztFightIntCoord[i]) == 1) {
				SoundEngine->play2D(power_up, GL_FALSE);
				ztFightIntCoord[i] = 200;
				sprintf_s(WhatPowerUp, "SYSTEM MESSAGE: SHIELD NUMBER +1");
				PowerUpPers = 300;
				shield++;
			}
		}

		zShieldCoord += enemyVelocity + levelUp;
		ShieldRot += 1;
		if (zShieldCoord > 32165) {
			zShieldCoord = randomizeZ();
			xShieldCoord = randomizeX();
			ShieldRot = 0;
		}
		if (checkCollisionPowerUp(xShieldCoord, zShieldCoord) == 1) {
			SoundEngine->play2D(power_up, GL_FALSE);
			zShieldCoord = 200;
			sprintf_s(WhatPowerUp, "SYSTEM MESSAGE: SHIELD NUMBER +1");
			PowerUpPers = 300;
			shield++;
		}

		zEnemyUpCoord += enemyVelocity + levelUp;
		EnemyUpRot += 1;
		if (zEnemyUpCoord > 2345) {
			zEnemyUpCoord = randomizeZ();
			xEnemyUpCoord = randomizeX();
			EnemyUpRot = 0;
		}
		if (checkCollisionPowerUp(xEnemyUpCoord, zEnemyUpCoord) == 1) {
			SoundEngine->play2D(power_up, GL_FALSE);
			zEnemyUpCoord = 200;
			sprintf_s(WhatPowerUp, "SYSTEM MESSAGE: SHIELD NUMBER +1");
			PowerUpPers = 300;
			shield++;
		}

		zEnemyDownCoord += enemyVelocity + levelUp;
		EnemyDownRot += 1;
		if (zEnemyDownCoord > 8978) {
			zEnemyDownCoord = randomizeZ();
			xEnemyDownCoord = randomizeX();
			EnemyDownRot = 0;
		}
		if (checkCollisionPowerUp(xEnemyDownCoord, zEnemyDownCoord) == 1) {
			SoundEngine->play2D(power_up, GL_FALSE);
			zEnemyDownCoord = 200;
			sprintf_s(WhatPowerUp, "SYSTEM MESSAGE: SHIELD NUMBER +1");
			PowerUpPers = 300;
			shield++;
		}

		zPlayerUpCoord += enemyVelocity + levelUp;
		PlayerUpRot += 1;
		if (zPlayerUpCoord > 12345) {
			zPlayerUpCoord = randomizeZ();
			xPlayerUpCoord = randomizeX();
			PlayerUpRot = 0;
		}
		if (checkCollisionPowerUp(xPlayerUpCoord, zPlayerUpCoord) == 1) {
			SoundEngine->play2D(power_up, GL_FALSE);
			zPlayerUpCoord = 200;
			sprintf_s(WhatPowerUp, "SYSTEM MESSAGE: SHIELD NUMBER +1");
			PowerUpPers = 300;
			shield++;
		}

		zPlayerDownCoord += enemyVelocity + levelUp;
		PlayerDownRot += 1;
		if (zPlayerDownCoord > 3456) {
			zPlayerDownCoord = randomizeZ();
			xPlayerDownCoord = randomizeX();
			PlayerDownRot = 0;
		}
		if (checkCollisionPowerUp(xPlayerDownCoord, zPlayerDownCoord) == 1) {
			SoundEngine->play2D(power_up, GL_FALSE);
			zPlayerDownCoord = 200;
			sprintf_s(WhatPowerUp, "SYSTEM MESSAGE: SHIELD NUMBER +1");
			PowerUpPers = 300;
			shield++;
		}

		FramesEgg--;
		if (FramesEgg <= 0) {
			start = 1;
			//sprintf_s(WhatPowerUp, "");
		}

		sprintf_s(ShieldMsg, "SHIELD NUMBER: %d", shield);
		sprintf_s(PointMsg, "POINTS: %d", point);

		if (PowerUpPers > 0) {
			PowerUpPers--;
		}
		else {
			sprintf_s(WhatPowerUp, "");
		}

		glutPostRedisplay();
	}
}


//Don't touch this function!
int loadasset(const char* path)
{
	// we are taking one of the postprocessing presets to avoid
	// writing 20 single postprocessing flagzs here.
	scene = aiImportFile(path, aiProcessPreset_TargetRealtime_Quality);
	if (scene) {
		get_bounding_box(&scene_min, &scene_max);
		scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
		scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
		scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
		return 0;
	}
	return 1;
}



//Don't touch this function!
int LoadGLTextures(const aiScene* scene)
{
	ILboolean success;
	/* Before calling ilInit() version should be checked. */

	{
		ILint test = ilGetInteger(IL_VERSION_NUM);
		/// wrong DevIL version ///
		std::string err_msg = "Wrong DevIL version. Old devil.dll in system32/SysWow64?";
		char* cErr_msg = (char*)err_msg.c_str();
		return -1;
	}
	ilInit(); /* Initialization of DevIL */
			  //if (scene->HasTextures()) abortGLInit("Support for meshes with embedded textures is not implemented");
			  /* getTexture Filenames and Numb of Textures */
	for (unsigned int m = 0; m < scene->mNumMaterials; m++)
	{
		int texIndex = 0;
		aiReturn texFound = AI_SUCCESS;
		aiString path;	// filename
		while (texFound == AI_SUCCESS)
		{
			texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
			textureIdMap[path.data] = NULL; //fill map with textures, pointers still NULL yet
			texIndex++;
		}
	}
	int numTextures = textureIdMap.size();
	/* array with DevIL image IDs */
	ILuint* imageIds = NULL;
	imageIds = new ILuint[numTextures];
	/* generate DevIL Image IDs */
	ilGenImages(numTextures, imageIds); /* Generation of numTextures image names */
	/* create and fill array with GL texture ids */
	textureIds = new GLuint[numTextures];
	glGenTextures(numTextures, textureIds); /* Texture name generation */
	/* define texture path */
	//std::string texturepath = "../../../test/models/Obj/";
	/* get iterator */
	std::map<std::string, GLuint*>::iterator itr = textureIdMap.begin();
	for (int i = 0; i < numTextures; i++)
	{
		//save IL image ID
		std::string filename = (*itr).first;  // get filename
		(*itr).second = &textureIds[i];	  // save texture id for filename in map
		itr++;								  // next texture
		ilBindImage(imageIds[i]); /* Binding of DevIL image name */
		std::string fileloc = basepath + filename;	/* Loading of image */
		success = ilLoadImage((const wchar_t*)fileloc.c_str());
		fprintf(stdout, "Loading Image: %s\n", fileloc.data());
		if (success) /* If no error occured: */
		{
			success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); /* Convert every colour component into
																unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
			if (!success)
			{
				/* Error occured */
				fprintf(stderr, "Couldn't convert image");
				return -1;
			}
			//glGenTextures(numTextures, &textureIds[i]); /* Texture name generation */
			glBindTexture(GL_TEXTURE_2D, textureIds[i]); /* Binding of texture name */

			//redefine standard texture values
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear interpolation for magnification filter */
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear interpolation for minifying filter */
			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
				ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
				ilGetData()); /* Texture specification */
		}
		else
		{
			/* Error occured */
			fprintf(stderr, "Couldn't load Image: %s\n", fileloc.data());
		}
	}
	ilDeleteImages(numTextures, imageIds); /* Because we have already copied image data into texture data we can release memory used by image. */
	//Cleanup
	delete[] imageIds;
	imageIds = NULL;
	//return success;
	return TRUE;
}

// ------------------------------------------------//
// --------------------- INITGL -------------------//
// ------------------------------------------------//
int InitGL()
{
	SoundEngine->play2D(menuBack, GL_TRUE);

	srand(time(NULL));

	resetStart();

	if (!LoadGLTextures(scene))
		return FALSE;
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);		 // Enables Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);				// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);		// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);			// The Type Of Depth Test To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculation
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);    // Uses default lighting parameters
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_NORMALIZE);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	return TRUE;					// Initialization Went OK
}


// ----------------------------------------------------------//
// --------------------------- MAIN -------------------------//
// ----------------------------------------------------------//
int main(int argc, char** argv)
{
	struct aiLogStream stream;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // enabling double buffering and RGBA
	//glutInitWindowSize(1920, 1080);
	window = glutCreateWindow("aVoid"); // creating the window
	glutFullScreen();           // making the window full screen

	string s;
	fstream myfile;
	myfile.open("./scoreboard.txt");
	getline(myfile, s);
	HighScore[0] = atoi(s.data());
	getline(myfile, s);
	HighScore[1] = atoi(s.data());
	getline(myfile, s);
	HighScore[2] = atoi(s.data());
	myfile.close();
	sprintf_s(HighScoreMsg2, "FIRST: %d", HighScore[0]);
	sprintf_s(HighScoreMsg3, "SECOND: %d", HighScore[1]);
	sprintf_s(HighScoreMsg4, "THIRD: %d", HighScore[2]);

	//glut main routine
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboard_up);
	glutSpecialFunc(SpecialInput);
	glutSpecialUpFunc(SpecialInput_up);
	glutMouseFunc(Mouse);
	glutIdleFunc(idleFunc);
	glutReshapeFunc(reshape);

	// get a handle to the predefined STDOUT log stream and attach it to the logging system. 
	//It will be active for all further calls to aiImportFile(Ex) and aiApplyPostProcessing.
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
	aiAttachLogStream(&stream);

	// ... exactly the same, but this stream will now write the log file to assimp_log.txt
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE, "assimp_log.txt");
	aiAttachLogStream(&stream);

	loadasset("models\\aVoid_AllModels.obj");
	/*
	// the model name can be specified on the command line.
	if(argc>=2)
	loadasset( argv[1] );
	else
	{
	fprintf(stderr,"usage: >>SimpleOpenGLTexturedLoader <modelpath\\modelname>");
	exit(1);
	}
	*/
	if (!InitGL())
	{
		fprintf(stderr, "Initialization failed");
		return FALSE;
	}

	glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();

	// cleanup - calling 'aiReleaseImport' is important, as the library 
	// keeps internal resources until the scene is freed again. Not 
	// doing so can cause severe resource leaking.
	aiReleaseImport(scene);

	// We added a log stream to the library, it's our job to disable it
	// again. This will definitely release the last resources allocated
	// by Assimp.
	aiDetachAllLogStreams();
	SoundEngine->drop();
	return 0;
}