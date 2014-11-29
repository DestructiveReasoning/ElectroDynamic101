#pragma once

#include "destructive_reasoning.h"
#include "Vector3.h"

class Entity
{
public:
	Entity(const GLuint _model, Vector3 pos, Vector3 vel, Vector3 accel, Vector3 *cameraPos);
	Entity(const GLuint _model, int texture, Vector3 pos, Vector3 vel, Vector3 accel, Vector3 *cameraPos);

	~Entity(void);

	virtual void Update();
	virtual void Render(bool wireframe);
	virtual void customUpdate() {};

	void setPosition(Vector3 pos);
	void setVelocity(Vector3 vel);
	void setAcceleration(Vector3 accel);
	void setModel(int mod);
	
	void appendForce(Vector3 force);

	Vector3 getPosition();
	Vector3* getPositionByReference();
	Vector3 getVelocity();
	Vector3 getAcceleration();
	inline Vector3 getFnet() { return fnet; };
	inline Vector3 *getFnetByReference() { return &fnet; };
	inline void setFnet(Vector3 _fnet) { fnet = _fnet; };
	GLuint getModel() const;

	inline std::vector<Vector3> *getForces(void) { return &forces; };
	inline Vector3 getForceAt(int index) { return forces[index]; };

	inline float getMass(void) { return mass; };

	static float xangle,yangle,zangle;

protected:
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 *cameraPos;
	std::vector<Vector3> forces; //List of all forces exerted on this entity
	Vector3 fnet;
	float mass;
	float angle;

	bool mobile;

	GLuint model;
	int texture;
	std::string texturepath;
};

