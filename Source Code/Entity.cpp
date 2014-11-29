#include "Entity.h"

Entity::Entity(const GLuint _model, Vector3 pos, Vector3 vel, Vector3 accel, Vector3 *_cameraPos) :
	fnet(Vector3()),
	cameraPos(_cameraPos)
{
	model = _model;
	position = pos;
	velocity = vel;
	acceleration = accel;
	texture = -1;
}

Entity::Entity(const GLuint _model, int _texture, Vector3 pos, Vector3 vel, Vector3 accel, Vector3 *_cameraPos) :
	fnet(Vector3()),
	cameraPos(_cameraPos)
{
	model = _model;
	position = pos;
	velocity = vel;
	acceleration = accel;
	texture = _texture;
}

void Entity::Update()
{
	acceleration = fnet = Vector3(0,0,0);
	for(int c = 0; c < forces.size(); c++)
	{
		fnet += forces[c];
	}
	if(!mobile) return;
	acceleration = fnet/mass;
	velocity += acceleration;
	position += velocity;

	fnet = fnet/_animationspeed;
	acceleration = fnet/mass;

	forces.clear();
}

void Entity::Render(bool wireframe)
{
	if(wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	}

	/*Rotate Scene*/
	glTranslatef(cameraPos->getX(),cameraPos->getY(),cameraPos->getZ() + _zdefault);
	glRotatef(xangle,1,0,0);
	glRotatef(yangle,0,1,0);
	glRotatef(zangle,0,0,1);

	/*Display Particle*/
	glTranslatef(position.getX(),position.getY(),position.getZ() - _zdefault);

	if(texture != -1) glBindTexture(GL_TEXTURE_2D,texture);
	glCallList(model);
	glLoadIdentity();

	//printf("%.2f\t%.2f\t%.2f\tModel: %d\n", position.getX(), position.getY(), position.getZ(), model);

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

void Entity::setPosition(Vector3 pos)
{
	position = pos;
}

void Entity::setVelocity(Vector3 vel)
{
	velocity = vel;
}

void Entity::setAcceleration(Vector3 accel)
{
	acceleration = accel;
}

void Entity::setModel(int _model)
{
	model = _model;
}

void Entity::appendForce(Vector3 force)
{
	forces.push_back(force);
}

Vector3 Entity::getPosition()
{
	return position;
}

Vector3* Entity::getPositionByReference()
{
	return &position;
}

Vector3 Entity::getVelocity()
{
	return velocity;
}

Vector3 Entity::getAcceleration()
{
	return acceleration;
}

GLuint Entity::getModel() const 
{
	return model;
}

Entity::~Entity(void)
{
}

float Entity::xangle;
float Entity::yangle;
float Entity::zangle;