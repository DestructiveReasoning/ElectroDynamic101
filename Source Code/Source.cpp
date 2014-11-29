#include "destructive_reasoning.h"
#include "ChargedParticle.h"
#include "Wnd32.h"
#include "AddParticleWindow.h"
#include "AddFieldWindow.h"
#include "TimedSplashScreen.h"

#define ASPECT_RATIO (4.0 / 3.0)
#define WIDTH  1200.0
#define HEIGHT (WIDTH / ASPECT_RATIO)

#define PromptCMD printf("[Command Mode]> ")
#define Prompt printf("> ")
#define Invalid printf("Invalid Entry\n")
#define Divider printf("*************\n")

float angle = 0.0;
float depth = -5.0;
const int triangle = 1;

Vector3 cameraPos;

void Update();
void printKeyBindings();

int loadObject(const char* fileName)
{
	std::vector<std::string*> coord;
	std::vector<coordinate*> vertices;
	std::vector<face*> faces;
	std::vector<coordinate*> normals;
	std::ifstream in(fileName);
	if(!in.is_open())
	{
		std::cout << "WILTZ: Cannot Open File" << std::endl;
		return -1;
	}

	char buffer[256];
	while(!in.eof())
	{
		//Storing the lines of the obj file
		in.getline(buffer, 256);
		coord.push_back(new std::string(buffer));
	}

	for(int c = 0; c < coord.size(); c++)
	{
		//If it is a comment, skip to next line
		if((*coord[c])[0] == '#') continue;

		//If the line is describing a vertex...
		else if((*coord[c])[0] == 'v' && (*coord[c])[1] == ' ')
		{
			float tmpx, tmpy, tmpz;
			//Format the string to v + 3 floats, store 3 floats to tmp
			sscanf_s(coord[c]->c_str(), "v %f %f %f", &tmpx, &tmpy, &tmpz);
			//Create Vertex
			vertices.push_back(new coordinate(tmpx, tmpy, tmpz));
		}
		//If the line is describing a normal...
		else if((*coord[c])[0] == 'v' && (*coord[c])[1] == 'n')
		{
			float tmpx, tmpy, tmpz;
			sscanf_s(coord[c]->c_str(), "vn %f %f %f", &tmpx, &tmpy, &tmpz);
			normals.push_back(new coordinate(tmpx, tmpy, tmpz));
		}
		//If the line is describing a face...
		else if((*coord[c])[0] == 'f')
		{
			//Variables to store the face number and the face vertices
			int fn, v1, v2, v3, v4;
			if(count(coord[c]->begin(),coord[c]->end(), ' ') == 4)
			{
				///OBJ File format for describing faces
				sscanf_s(coord[c]->c_str(), "f %d//%d %d//%d %d//%d %d//%d", &v1,&fn, &v2,&fn, &v3,&fn, &v4,&fn);
				faces.push_back(new face(fn,v1,v2,v3,v4));
			}
			else //If it's a triangle...
			{
				sscanf_s(coord[c]->c_str(), "f %d//%d %d//%d %d//%d", &v1,&fn, &v2,&fn, &v3,&fn);
				faces.push_back(new face(fn,v1,v2,v3));
			}
		}
	}

	int num;
	num = glGenLists(1);
	//Create a new OpenGL DisplayList for OBJ, store it in num. The function will return num
	glNewList(num,GL_COMPILE);
	for(int c = 0; c < faces.size(); c++)
	{
		if(faces[c]->quad == true)
		{
			glBegin(GL_QUADS);
				glNormal3f(normals[faces[c]->facenum - 1]->x,normals[faces[c]->facenum - 1]->y, normals[faces[c]->facenum - 1]->z);
				glVertex3f(vertices[faces[c]->faces[0]-1]->x,vertices[faces[c]->faces[0]-1]->y,vertices[faces[c]->faces[0]-1]->z);
				glVertex3f(vertices[faces[c]->faces[1]-1]->x,vertices[faces[c]->faces[1]-1]->y,vertices[faces[c]->faces[1]-1]->z);
				glVertex3f(vertices[faces[c]->faces[2]-1]->x,vertices[faces[c]->faces[2]-1]->y,vertices[faces[c]->faces[2]-1]->z);
				glVertex3f(vertices[faces[c]->faces[3]-1]->x,vertices[faces[c]->faces[3]-1]->y,vertices[faces[c]->faces[3]-1]->z);
			glEnd();
		}else
		{
			glBegin(GL_TRIANGLES);
				glNormal3f(normals[faces[c]->facenum - 1]->x, normals[faces[c]->facenum - 1]->y, normals[faces[c]->facenum - 1]->z);
				glVertex3f(vertices[faces[c]->faces[0]-1]->x,vertices[faces[c]->faces[0]-1]->y,vertices[faces[c]->faces[0]-1]->z);
				glVertex3f(vertices[faces[c]->faces[1]-1]->x,vertices[faces[c]->faces[1]-1]->y,vertices[faces[c]->faces[1]-1]->z);
				glVertex3f(vertices[faces[c]->faces[2]-1]->x,vertices[faces[c]->faces[2]-1]->y,vertices[faces[c]->faces[2]-1]->z);
			glEnd();
		}
	}
	glEndList();

	for(int c = 0; c < coord.size(); c++)
	{
		delete coord[c];
	}

	for(int c = 0; c < vertices.size(); c++)
	{
		delete vertices[c];
	}

	for(int c = 0; c < normals.size(); c++)
	{
		delete normals[c];
	}

	for(int c = 0; c < faces.size(); c++)
	{
		delete faces[c];
	}

	return num;
}

unsigned int loadTexture(const char* fileName)
{
	SDL_Surface* img = SDL_LoadBMP(fileName);
	unsigned int id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D,id);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,img->w,img->h,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,img->pixels);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	SDL_FreeSurface(img);
	return id;
}

unsigned int blueCheckerTexture;
int redtexture, greentexture, graytexture, granitetexture, yellowtexture;
int cubeModel;
int wiltzLogo;
bool select = true;
bool command = false;
Entity *logo;
ChargedParticle *protonEntity;
ChargedParticle *protonEntity2;
ChargedParticle *electronEntity;
ChargedParticle *electronEntity2;
ChargedParticle* p = nullptr;
Uint32 lastButtonTime = 0;
std::vector<ChargedParticle *> deletedParticles;
SDL_Renderer *renderer;

void init()
{
	glClearColor(0.0f,0.f,0.0f,1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,WIDTH/HEIGHT,1.0,500.0);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	ElectroDynamic::redtexture = loadTexture("RedTexture.bmp");
	redtexture = ElectroDynamic::redtexture;
	greentexture = loadTexture("GreenTexture.bmp");
	graytexture = loadTexture("GrayTexture.bmp");
	granitetexture = loadTexture("GraniteTexture.bmp");
	ElectroDynamic::yellowtexture = loadTexture("YellowTexture.bmp");
	yellowtexture = ElectroDynamic::yellowtexture;
	ElectroDynamic::addParticleTexture = loadTexture("AddParticleLabel.bmp");
	ElectroDynamic::arrowTexture = graytexture;
	ElectroDynamic::arrowTextureSelected = yellowtexture;
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	float dif[] = {0.6,0.6,0.6,-2.0};
	glLightfv(GL_LIGHT0,GL_DIFFUSE,dif);
	float amb[] = {0.2,0.2,0.2,1.0};
	glLightfv(GL_LIGHT0,GL_AMBIENT,amb);
	cubeModel = loadObject("cubeObject.obj");
	wiltzLogo = loadObject("wiltzLogo.obj");
	glEnable(GL_MULTISAMPLE);
}

void drawSquarePyramid(float size, float x, float y, float z)
{
	glBegin(GL_TRIANGLES);
	//Front Face
		glColor3f(1.0,0.0,0.0);
		glVertex3f(x, y + size/2, z);
		glVertex3f(x - size/2, y - size/2, z + size/2);
		glVertex3f(x + size/2, y - size/2, z + size/2);
	//Left Face
		glColor3f(0.0,1.0,0.0);
		glVertex3f(x, y + size/2, z);
		glVertex3f(x - size/2, y - size/2, z - size/2);
		glVertex3f(x - size/2, y - size/2, z + size/2);
	//Back Face
		glColor3f(0.0,0.0,1.0);
		glVertex3f(x, y + size/2, z);
		glVertex3f(x - size/2, y - size/2, z - size/2);
		glVertex3f(x + size/2, y - size/2, z - size/2);
	//Right Face
		glColor3f(1.0,0.4,0.0);
		glVertex3f(x, y + size/2, z);
		glVertex3f(x + size/2, y - size/2, z + size/2);
		glVertex3f(x + size/2, y - size/2, z - size/2);
	glEnd();
	glBegin(GL_QUADS);
		glColor3f(1.0,0.0,1.0);
		glVertex3f(x - size/2, y - size/2, z - size/2);
		glVertex3f(x + size/2, y - size/2, z - size/2);
		glVertex3f(x + size/2, y - size/2, z + size/2);
		glVertex3f(x - size/2, y - size/2, z + size/2);
	glEnd();
}

void drawCube(float size, float x, float y, float z, unsigned int texture)
{
	float difamb[] = {1.0, 0.5, 0.0, 1.0};
	glBindTexture(GL_TEXTURE_2D,texture);
	glBegin(GL_QUADS);
	//Front Face
		//glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,difamb);
		glNormal3f(0.0,0.0,1.0);
		//glColor3f(1.0,0.0,0.0);
		glTexCoord2f(0.0,1.0);
		glVertex3f(x - size/2, y + size/2, z + size/2);
		glTexCoord2f(1.0,1.0);
		glVertex3f(x + size/2, y + size/2, z + size/2);
		glTexCoord2f(1.0,0.0);
		glVertex3f(x + size/2, y - size/2, z + size/2);
		glTexCoord2f(0.0,0.0);
		glVertex3f(x - size/2, y - size/2, z + size/2);
	//Left Face
		glNormal3f(-1.0, 0.0, 0.0);
		//glColor3f(0.0,1.0,0.0);
		glTexCoord2f(0.0,1.0);
		glVertex3f(x - size/2, y + size/2, z - size/2);
		glTexCoord2f(1.0,1.0);
		glVertex3f(x - size/2, y + size/2, z + size/2);
		glTexCoord2f(1.0,0.0);
		glVertex3f(x - size/2, y - size/2, z + size/2);
		glTexCoord2f(0.0,0.0);
		glVertex3f(x - size/2, y - size/2, z - size/2);
	//Back Face
		glNormal3f(0.0,0.0,-1.0);
		//glColor3f(0.0,0.0,1.0);
		glTexCoord2f(0.0,1.0);
		glVertex3f(x - size/2, y + size/2, z - size/2);
		glTexCoord2f(1.0,1.0);
		glVertex3f(x + size/2, y + size/2, z - size/2);
		glTexCoord2f(1.0,0.0);
		glVertex3f(x + size/2, y - size/2, z - size/2);
		glTexCoord2f(0.0,0.0);
		glVertex3f(x - size/2, y - size/2, z - size/2);
	//Right Face
		float back[] = {0.5,0.0,1.0,1.0};
		//glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,back);
		glNormal3f(1.0,0.0,0.0);
		//glColor3f(1.0,1.0,0.0);
		glTexCoord2f(0.0,1.0);
		glVertex3f(x + size/2, y + size/2, z + size/2);
		glTexCoord2f(1.0,1.0);
		glVertex3f(x + size/2, y + size/2, z - size/2);
		glTexCoord2f(1.0,0.0);
		glVertex3f(x + size/2, y - size/2, z - size/2);
		glTexCoord2f(0.0,0.0);
		glVertex3f(x + size/2, y - size/2, z + size/2);
	//Top Face
		glNormal3f(0.0,1.0,0.0);
		//glColor3f(1.0,0.0,1.0);
		glTexCoord2f(0.0,1.0);
		glVertex3f(x - size/2, y + size/2, z - size/2);
		glTexCoord2f(1.0,1.0);
		glVertex3f(x + size/2, y + size/2, z - size/2);
		glTexCoord2f(1.0,0.0);
		glVertex3f(x + size/2, y + size/2, z + size/2);
		glTexCoord2f(0.0,0.0);
		glVertex3f(x - size/2, y + size/2, z + size/2);
	//Bottom Face
		glNormal3f(0.0,-1.0,0.0);
		//glColor3f(1.0,1.0,1.0);
		glTexCoord2f(0.0,1.0);
		glVertex3f(x - size/2, y - size/2, z - size/2);
		glTexCoord2f(1.0,1.0);
		glVertex3f(x + size/2, y - size/2, z - size/2);
		glTexCoord2f(1.0,0.0);
		glVertex3f(x + size/2, y - size/2, z + size/2);
		glTexCoord2f(0.0,0.0);
		glVertex3f(x - size/2, y - size/2, z + size/2);
	glEnd();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	float pos[] = {0.0,10.0,_zdefault + 4,1.0};
	glLightfv(GL_LIGHT0,GL_POSITION,pos);

			/*Draw Axis*/
		glBindTexture(GL_TEXTURE_2D,yellowtexture);
		glTranslatef(0.0 + cameraPos.getX(),0.0 + cameraPos.getY(),_zdefault + cameraPos.getZ());
		glRotatef(Entity::yangle,0.0,1.0,0.0);
		glBegin(GL_LINES);
		glColor3f(0.5,0.0,0.0);
			glVertex3f(-1024.0f,0.0f,0.0f);
			glVertex3f(1024.0f,0.0f,0.0f);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(0.0f,-1024.0f,0.0f);
			glVertex3f(0.0f,1024.0f,0.0f);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(0.0f,0.0f,-1024.0f);
			glVertex3f(0.0f,0.0f,1024.0f);
		glEnd();

		glLoadIdentity();

		glColor3f(1.0,1.0,1.0);

	for(int c = 0; c < ElectroDynamic::particles.size(); c++)
	{
		if(select)
		{
			(c == ElectroDynamic::index) ? ElectroDynamic::particles[c]->Render(false) : ElectroDynamic::particles[c]->Render(true);
		}
		else
		{
			ElectroDynamic::particles[c]->Render(false);
		}
	}
	if(select)
	{
		for(int c = 0; c < ElectroDynamic::fields.size(); c++)
		{
			(c == ElectroDynamic::fieldIndex) ? glBindTexture(GL_TEXTURE_2D,ElectroDynamic::arrowTextureSelected) : glBindTexture(GL_TEXTURE_2D,ElectroDynamic::arrowTexture);
			(c == ElectroDynamic::fieldIndex) ? ElectroDynamic::fields[c]->Render(cameraPos,true) : ElectroDynamic::fields[c]->Render(cameraPos,false);
		}
		//addParticlePlane->Render(false);
	}
}

void handleInput2(SDL_Event *mainEvent)
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	if(state[_Select])
	{
		if(SDL_GetTicks() - lastButtonTime > _buttonspeed)
		{
			select = !select;
			lastButtonTime = SDL_GetTicks();
			(select) ? printf("Select\n") : printf("Unselect\n");
		}
	}

	if(state[_Command] && (SDL_GetTicks() - lastButtonTime > _buttonspeed))
	{
		command = true; 
		lastButtonTime = SDL_GetTicks();
	}
	if(command) return;

	if(select)
	{
		using namespace ElectroDynamic;

		if(state[_U] || state[_Alt])
		{
			if(deletedParticles.size() == 0) return;
			if(SDL_GetTicks() - lastButtonTime > 2 * _buttonspeed)
			{
				particles.push_back(deletedParticles.back());
				deletedParticles.pop_back();
				lastButtonTime = SDL_GetTicks();
				(index > 0) ? index-- : index = 0;
			}
			return;
		}

		if(state[_0] && SDL_GetTicks() - lastButtonTime > _buttonspeed)		
		{
			particles[index]->reverseCharge();
			lastButtonTime = SDL_GetTicks();
		}

		if(state[_PanLeft])		cameraPos.increment(_cameraSpeed,1.0,0.0,0.0);
		if(state[_PanRight])	cameraPos.increment(_cameraSpeed,-1.0,0.0,0.0);
		if(state[_PanUp])		cameraPos.increment(_cameraSpeed,0.0,-1.0,0.0);
		if(state[_PanDown])		cameraPos.increment(_cameraSpeed,0.0,1.0,0.0);
		if(state[_ZoomIn])		cameraPos.increment(_cameraSpeed,0.0,0.0,1.0);
		if(state[_ZoomOut])		cameraPos.increment(_cameraSpeed,0.0,0.0,-1.0);
		if(state[SDL_SCANCODE_1]) Entity::yangle += _rotationSpeed;
		if(state[SDL_SCANCODE_2]) Entity::yangle -= _rotationSpeed;
		if(state[_ResetCamera])	
		{
			cameraPos.set(0.0f,0.0f,0.0f);
			Entity::xangle = Entity::yangle = Entity::zangle = 0.0f;
		}
		if(state[_MoveLeft])	(state[_FineAdjustment]) ?	particles[index]->getPositionByReference()->increment(_cameraSpeed/16,-1.0,0.0,0.0) : 
															particles[index]->getPositionByReference()->increment(_cameraSpeed/4,-1.0,0.0,0.0);
		if(state[_MoveRight])	(state[_FineAdjustment]) ?	particles[index]->getPositionByReference()->increment(_cameraSpeed/16,1.0,0.0,0.0) : 
															particles[index]->getPositionByReference()->increment(_cameraSpeed/4,1.0,0.0,0.0);
		if(state[_MoveUp])		(state[_FineAdjustment]) ?	particles[index]->getPositionByReference()->increment(_cameraSpeed/16,0.0,1.0,0.0) : 
															particles[index]->getPositionByReference()->increment(_cameraSpeed/4,0.0,1.0,0.0);
		if(state[_MoveDown])	(state[_FineAdjustment]) ?	particles[index]->getPositionByReference()->increment(_cameraSpeed/16,0.0,-1.0,0.0) : 
															particles[index]->getPositionByReference()->increment(_cameraSpeed/4,0.0,-1.0,0.0);
		if(state[_MoveIn])		(state[_FineAdjustment]) ?	particles[index]->getPositionByReference()->increment(_cameraSpeed/16,0.0,0.0,-1.0) : 
															particles[index]->getPositionByReference()->increment(_cameraSpeed/4,0.0,0.0,-1.0);
		if(state[_MoveOut])		(state[_FineAdjustment]) ?	particles[index]->getPositionByReference()->increment(_cameraSpeed/16,0.0,0.0,-1.0) : 
															particles[index]->getPositionByReference()->increment(_cameraSpeed/4,0.0,0.0,1.0);
	
		if(state[_Remove])
		{
			if(SDL_GetTicks() - lastButtonTime > 2 * _buttonspeed)
			{
				deletedParticles.push_back(particles[index]);
				particles.erase(particles.begin() + index);
				lastButtonTime = SDL_GetTicks();
				(index > 0) ? index-- : index = 0;
			}
		}
		if(state[_RControl])
		{
			if(SDL_GetTicks() - lastButtonTime > 2 * _buttonspeed)
			{
				deletedParticles.push_back(particles.back());
				particles.pop_back();
				lastButtonTime = SDL_GetTicks();
				(index > 0) ? index-- : index = 0;
			}
		}
		if(state[_MoveLeft] || state[_MoveRight] || state[_MoveUp] || state[_MoveDown] || state[_MoveIn] || state[_MoveOut])
		{
			particles[index]->setVelocity(Vector3(0,0,0));
			particles[index]->setAcceleration(Vector3(0,0,0));
		}
		if(state[_ScrollForward])
		{
			if(SDL_GetTicks() - lastButtonTime > _buttonspeed)
			{
				Divider;
				recalculateForces();
				(index + 1 >= particles.size()) ? index = 0 : index++;
				lastButtonTime = SDL_GetTicks();
				printf("Current particle position: ");
				particles[index]->getPosition().print(Vector3::Position);
				printf("Current particle charge: %.3e C\n", particles[index]->getCharge());
				printf("Current particle mass: %.3e kg\n", particles[index]->getMass());
				printf("Force exerted on current particle: ");
				particles[index]->getFnet().print(Vector3::Force);
				Divider;
			}
		}
		if(state[_ScrollBackward])
		{
			if(SDL_GetTicks() - lastButtonTime > _buttonspeed)
			{
				Divider;
				recalculateForces();
				(index - 1 < 0) ? index = particles.size() - 1 : index--;
				lastButtonTime = SDL_GetTicks();
				printf("Current particle position: ");
				particles[index]->getPosition().print(Vector3::Position);
				printf("Current particle charge: %.3e C\n", particles[index]->getCharge());
				printf("Current particle mass: %.3e kg\n", particles[index]->getMass());
				printf("Force exerted on current particle: ");
				particles[index]->getFnet().print(Vector3::Force);
				Divider;
			}
		}
		if(state[_FieldForward])
		{
			if(SDL_GetTicks() - lastButtonTime > _buttonspeed)
			{
				(fieldIndex + 1 >= fields.size()) ? fieldIndex = 0 : fieldIndex++;
				lastButtonTime = SDL_GetTicks();
				fields[fieldIndex]->Update();
				if(fieldIndex < fields.size()) {printf("Current electric field detected: "); fields[fieldIndex]->field.print(Vector3::Electric_Field);}
			}
		}
		if(state[_FieldBackward])
		{
			if(SDL_GetTicks() - lastButtonTime > _buttonspeed)
			{
				(fieldIndex - 1 < 0) ? fieldIndex = fields.size() - 1 : fieldIndex--;
				lastButtonTime = SDL_GetTicks();
				fields[fieldIndex]->Update();
				if(fields.size() > 0) {printf("Current electric field detected: "); fields[fieldIndex]->field.print(Vector3::Electric_Field);}
			}
		}
		if(state[_Attributes] && SDL_GetTicks() - lastButtonTime > _buttonspeed)
		{
			Divider;
			recalculateForces();
			printf("Current Particle Charge: ");
			printf("%.3e C\n", particles[index]->getCharge());
			printf("Current Particle Mass: ");
			printf("%.3e kg\n", particles[index]->getMass());
			printf("Current Particle Position: ");
			particles[index]->getPosition().print(Vector3::Position);
			printf("Current Particle Velocity: ");
			(particles[index]->isMobile()) ? particles[index]->getVelocity().print(Vector3::Velocity) : printf("0 - Fixed Particle\n");
			printf("Current Particle Acceleration: ");
			(particles[index]->isMobile()) ? particles[index]->getAcceleration().print(Vector3::Acceleration) : printf("0 - Fixed Particle\n");
			printf("Force exerted on particle: ");
			particles[index]->getFnet().print(Vector3::Force);
			Divider;
			lastButtonTime = SDL_GetTicks();
		}
		//recalculateForces();
	}
	else
	{
		using namespace ElectroDynamic;
		if(state[_PanLeft])		cameraPos.increment(_cameraSpeed,1.0,0.0,0.0);
		if(state[_PanRight])	cameraPos.increment(_cameraSpeed,-1.0,0.0,0.0);
		if(state[_PanUp])		cameraPos.increment(_cameraSpeed,0.0,-1.0,0.0);
		if(state[_PanDown])		cameraPos.increment(_cameraSpeed,0.0,1.0,0.0);
		if(state[_ZoomIn])		cameraPos.increment(_cameraSpeed,0.0,0.0,1.0);
		if(state[_ZoomOut])		cameraPos.increment(_cameraSpeed,0.0,0.0,-1.0);
		if(state[_ResetCamera])	
		{
			cameraPos.set(0.0f,0.0f,0.0f);
			Entity::xangle = Entity::yangle = Entity::zangle = 0.0f;
		}
	}

	if(mainEvent->type == SDL_MOUSEBUTTONDOWN)
	{
		if(SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1))
		{
			AddParticleWindow *wnd = new AddParticleWindow(300,400,"Add Particle","APW",&cameraPos);
			delete wnd;
			
			(AddParticleWindow::Charge < 0) ? ElectroDynamic::particles.push_back(	new ChargedParticle(ElectroDynamic::electron,
																					redtexture,
																					AddParticleWindow::tempVector,
																					AddParticleWindow::Mass,
																					AddParticleWindow::Charge,
																					AddParticleWindow::Fixed,
																					&cameraPos)) :
											  ElectroDynamic::particles.push_back(	new ChargedParticle(ElectroDynamic::electron,
																					yellowtexture,
																					AddParticleWindow::tempVector,
																					AddParticleWindow::Mass,
																					AddParticleWindow::Charge,
																					AddParticleWindow::Fixed,
																					&cameraPos));
		}
		if(SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(3))
		{
			AddFieldWindow *wnd = new AddFieldWindow(300,400,"Add Field Sensor","AFS",&cameraPos);
			delete wnd;
			ElectroDynamic::fields.push_back(new ElectroDynamic::FieldSensor(AddFieldWindow::tempVector,Vector3(0,0,0)));
		}
	}

	if(mainEvent->type == SDL_MOUSEMOTION)
	{
		/*
			If motion.xrel or motion.yrel are 1, the mouse isn't moving.
		*/
		bool dragging = false;
		int mouseX, mouseY;
		/*LEFT MOUSE BUTTON*/
		if(SDL_GetMouseState(&mouseX,&mouseY)&SDL_BUTTON(2))
		{
			float minLength = 1000000000;
			Vector3 mousePos = Vector3((float) mouseX - cameraPos.getX() - WIDTH/2, (float) mouseY - cameraPos.getY() - HEIGHT/2, -cameraPos.getZ() + _zdefault);
			for(int c = 0; c < ElectroDynamic::particles.size(); c++)
			{
				if(mousePos.distanceFrom(ElectroDynamic::particles[c]->getPosition()) <= minLength) 
				{
					minLength = mousePos.distanceFrom(ElectroDynamic::particles[c]->getPosition() + cameraPos);
					if(!dragging)
					{
						p = ElectroDynamic::particles[ElectroDynamic::index];
						dragging = true;
					}
				}
			}
			if(minLength > _maxDraggingLength) dragging = false;
			else
			{
				if(select) p->getPositionByReference()->increment(_dragSpeed,mainEvent->motion.xrel,-mainEvent->motion.yrel,0);
			}
		}else
		{
			dragging = false;
		}
	}
}

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TimedSplashScreen splash = TimedSplashScreen(640,480,3000,"PoweredByDestructiveReasoning.bmp");
	SDL_Window* screen = SDL_CreateWindow("Harley Wiltzer's ElectroDynamic", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(screen,-1,SDL_RENDERER_ACCELERATED);
	SDL_GL_CreateContext(screen);
	bool running = true;
	Uint32 start;
	SDL_Event event;
	cameraPos = Vector3(0,0,0);
	init();
	logo = new Entity(wiltzLogo,graytexture,Vector3(0,0,_zdefault),Vector3(0,0,0),Vector3(0,0,0),&cameraPos);

	printf("Welcome to ElectroDynamic!\nPress ESC to start simulation or press F1 to enter COMMAND MODE\n\n");

	//ElectroDynamic::proton = loadObject("sphere0500.obj");
	ElectroDynamic::proton = loadObject("sphere0250.obj");
	ElectroDynamic::electron = loadObject("sphere0250.obj");
	ElectroDynamic::arrow = loadObject("arrow.obj");
	ElectroDynamic::plane = loadObject("Plane.obj");

	protonEntity = new ChargedParticle(ElectroDynamic::proton,yellowtexture,Vector3(1,0,_zdefault),1000.0f,1.602 * pow(10,-3), false, &cameraPos);
	protonEntity2 = new ChargedParticle(ElectroDynamic::proton,yellowtexture,Vector3(-1,0,_zdefault),1000.0f,1.602 * pow(10,-3),false, &cameraPos);

	ElectroDynamic::particles.push_back(protonEntity);
	ElectroDynamic::particles.push_back(protonEntity2);

	while(running)
	{
		start = SDL_GetTicks();
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			}
		}


		logo->Update();
		
		if(!select && !command) Update();
		for(int c = 0; c < ElectroDynamic::fields.size(); c++)
		{
			ElectroDynamic::fields[c]->Update();
		}

		handleInput2(&event);

		if(command)
		{
			std::cin.clear();
			std::string ans;
			PromptCMD;
			std::cin.clear();
			std::cin >> ans;
			std::cin.clear();
			if(ans == "done" || ans == "exit" || ans == "quit") command = false;
			else 
			if(ans == "add")
			{
				std::cin.clear();
				std::string ans;
				float x, y, z;
				float mass,charge;
				bool mobile;
				printf("Enter the x position of the new point charge: ");
				std::cin >> ans;
				try {x = atof(ans.c_str());} catch(...) {printf("Invalid Entry\n"); continue;}
				printf("Enter the y position of the new point charge: ");
				ans.clear();
				std::cin >> ans;
				try {y = atof(ans.c_str());} catch(...) {printf("Invalid Entry\n"); continue;}
				printf("Enter the z position of the new point charge: ");
				std::cin >> ans;
				try {z = atof(ans.c_str()) + _zdefault;} catch(...) {printf("Invalid Entry\n"); continue;}
				printf("Enter the mass of the new point charge in g (Recommended to be at least 0.001): ");
				std::cin >> ans;
				mass = atof(ans.c_str()) * 0.001;
				printf("Enter the charge of the new point charge in nC: ");
				std::cin >> ans;
				try {charge = atof(ans.c_str()) / 1000000000;} catch(...) {printf("Invalid Entry\n"); continue;}
				ans.clear();
				while(ans != "y" && ans != "Y" && ans != "n" && ans != "N")
				{
					printf("Is the new particle fixed in position? [y/n]: ");
					std::cin >> ans;
				}
				(ans == "Y" || ans == "y") ? mobile = false : mobile = true;
				if(charge < 0) ElectroDynamic::particles.push_back(new ChargedParticle(ElectroDynamic::electron,redtexture,Vector3(x,y,z),mass,charge,mobile,&cameraPos));
				else ElectroDynamic::particles.push_back(new ChargedParticle(ElectroDynamic::proton,yellowtexture,Vector3(x,y,z),mass,charge,mobile,&cameraPos));
				ElectroDynamic::recalculateForces();
			}else
			if(ans == "pop")
			{
				deletedParticles.push_back(ElectroDynamic::particles.back());
				ElectroDynamic::particles.pop_back();
				(ElectroDynamic::index > 0) ? ElectroDynamic::index-- : ElectroDynamic::index = 0;
				ElectroDynamic::recalculateForces();
			}else
			if(ans == "move")
			{
				float x, y, z;
				std::cin.clear();
				std::string ans;
				if(ElectroDynamic::particles.size() == 0) continue;
				ChargedParticle *p = ElectroDynamic::particles[ElectroDynamic::index];	
				printf("Enter new x coordinate for the selected particle: ");
				std::cin >> ans;
				if(ans == "q") continue;
				try {x = atof(ans.c_str());} catch(...) {printf("Invalid Entry\n"); continue;}
				ans.clear();
				printf("Enter new y coordinate for the selected particle: ");
				std::cin >> ans;
				if(ans == "q") continue;
				try {y = atof(ans.c_str());} catch(...) {printf("Invalid Entry\n"); continue;}
				ans.clear();
				printf("Enter new z coordinate for the selected particle: ");
				std::cin >> ans;
				if(ans == "q") continue;
				try {z = atof(ans.c_str()) + _zdefault;} catch(...) {printf("Invalid Entry\n"); continue;}
				ans.clear();
				p->setPosition(Vector3(x,y,z));
				p->setVelocity(Vector3(0,0,0));
				p->setAcceleration(Vector3(0,0,0));
				ElectroDynamic::recalculateForces();
			}else if(ans == "keys")
			{
				printKeyBindings();
			}else if(ans == "addfield" || ans.substr(0,4) == "addf")
			{
				float x, y, z;
				std::cin.clear();
				std::string ans;
				printf("Enter x position for electric field sensor: ");
				std::cin >> ans;
				if(ans == "q") continue;
				try {x = atof(ans.c_str());} catch(...) {Invalid; continue;}
				ans.clear();
				printf("Enter y position for electric field sensor: ");
				std::cin >> ans;
				if(ans == "q") continue;
				try {y = atof(ans.c_str());} catch(...) {Invalid; continue;}
				ans.clear();
				printf("Enter z position for electric field sensor: ");
				std::cin >> ans;
				if(ans == "q") continue;
				try {z = atof(ans.c_str()) + _zdefault;} catch(...) {Invalid; continue;}
				ans.clear();
				ElectroDynamic::fields.push_back(new ElectroDynamic::FieldSensor(Vector3(x,y,z), Vector3(0,0,0)));
			}else if(ans == "deletefield")
			{
				if(ElectroDynamic::fields.size() > 0)
				{
					ElectroDynamic::fields.erase(ElectroDynamic::fields.begin() + ElectroDynamic::fieldIndex);
					(ElectroDynamic::fieldIndex - 1 > 0) ? ElectroDynamic::fieldIndex-- : ElectroDynamic::fieldIndex = 0;
				}
			}else if(ans == "popfield")
			{
				if(ElectroDynamic::fields.size() > 0) ElectroDynamic::fields.pop_back();
				(ElectroDynamic::fieldIndex - 1 > 0) ? ElectroDynamic::fieldIndex-- : ElectroDynamic::fieldIndex = 0;
			}else
			{
				printf("Please enter one of the following commands:\n");
				printf("add\t\tAdd a new particle to the scene\n");					/*Alternative: Click button, GUI*/
				printf("pop\t\tRemove the last added particle from the scene\n");	/*Alternative: None*/
				printf("move\t\tChange the position of the current particle\n");	/*Alternative: Click and drag*/
				printf("addfield\tAdd a new electric field sensor\n");				/*Alternative: Click button, GUI*/
				printf("deletefield\tDelete selected electric field sensor\n");		/*Alternative: None*/
				printf("popfield\tRemove last added electric field sensor\n");		/*Alternative: None*/
				printf("keys\t\tView the keybindings\n");
				printf("exit\t\tExit Command Mode\n");
			}
		}

		display();

		SDL_GL_SwapWindow(screen);
		if(1000/60 > (SDL_GetTicks() - start)) SDL_Delay(1000/60 - (SDL_GetTicks() - start));
	}

	for(int c = 0; c < ElectroDynamic::particles.size(); c++)
	{
		delete ElectroDynamic::particles[c];
	}

	for(int c = 0; c < deletedParticles.size(); c++)
	{
		delete deletedParticles[c];
	}

	for(int c = 0; c < ElectroDynamic::fields.size(); c++)
	{
		delete ElectroDynamic::fields[c];
	}

	SDL_Quit();
	return 0;
}

void Update()
{
	for(int i = 0; i < ElectroDynamic::particles.size(); i++)
	{
		for(int j = i + 1; j < ElectroDynamic::particles.size(); j++)
		{
			ElectroDynamic::calculateElectricForce(ElectroDynamic::particles[i],ElectroDynamic::particles[j]);
		}
	}
	
	for(int c = 0; c < ElectroDynamic::particles.size(); c++)
	{
		ElectroDynamic::particles[c]->Update();
	}
}

void printKeyBindings()
{
	printf("****SELECT MODE****\n");
	printf("Esc\t\tResume/Start simulation\n");
	printf("F1\t\tEnter Command Mode\n");
	printf("Arrow Keys\tMove camera in the xy plane\n");
	printf("Minus\t\tZoom out\n");
	printf("Plus\t\tZoom in\n");
	printf("Home\t\tReset camera position\n");
	printf("Tab\t\tCycle through particles\n");
	printf("Tilde\t\tCycle through particles backwards\n");
	printf("Space\t\tCycle through electric field sensors forward, show field vector\n");
	printf("Backspace\tCycle through electric field sensors backward, show field vector\n");
	printf("WASD\t\tReposition selected particle in the xy plane\n");
	printf("Ctrl\t\tReposition selected particle towards the positive z axis (out of screen)\n");
	printf("Shift\t\tReposition selected particle towards the negative z axis (into screen)\n");
	printf("P\t\tDisplay position, velocity and acceleration vectors of the current particle\n");
	printf("Right Ctrl\tRemove last added particle from scene\n");
	printf("Del\t\tRemove selected particle from scene\n");
	printf("U\t\tAdd last deleted particle to scene\n");
	printf("\n****SIMULATION MODE****\n");
	printf("Esc\t\tPause simulation, enter Select Mode\n");
	printf("F1\t\tEnter command mode\n");
	printf("Arrow Keys\tMove camera in the xy plane\n");
	printf("Minus\t\tZoom out\n");
	printf("Plus\t\tZoom in\n");
	printf("Home\t\tReset camera position\n");
}