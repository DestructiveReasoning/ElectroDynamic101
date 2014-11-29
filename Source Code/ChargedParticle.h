#pragma once

#include "destructive_reasoning.h"
#include "Entity.h"

#define ArrowLength (1.0/4000000.0)

class ChargedParticle : public Entity
{
public:
	ChargedParticle(int model, int texture, Vector3 pos, float mass, float charge, bool isFixed, Vector3 *cameraPos);
	~ChargedParticle(void);

	inline float getCharge(void) { return charge; };
	inline void reverseCharge(void) { charge *= -1; customUpdate(); };
	inline bool  isMobile(void) { return mobile; };

	virtual void customUpdate();

	static void drawArrow(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, bool);

private:
	float charge;
};

namespace ElectroDynamic
{
	static const float k = 8.998 * pow(10,9);
	static GLuint proton;
	static GLuint electron;
	static GLuint arrow;
	static GLuint plane;
	static GLuint addParticleTexture;
	static GLuint arrowTexture;
	static GLuint arrowTextureSelected;
	static GLuint redtexture;
	static GLuint yellowtexture;

	static Vector3 tempVector;
	static float tempCharge;
	static float tempMass;
	static bool tempFixed;

	static int index = 0;
	static int fieldIndex = 0;

	static std::vector<ChargedParticle*> particles;

	static void calculateElectricForce(ChargedParticle *p1, ChargedParticle *p2)
	{
		float q1, q2;
		float r;
		float theta;
		long double fe;
		q1 = p1->getCharge();
		q2 = p2->getCharge();
		r = p1->getPosition().distanceFrom(p2->getPosition());
		//theta = acos(p1->getPosition().dot(p2->getPosition()) / (p1->getPosition().length() * p2->getPosition().length()));
		Vector3 rhat = p2->getPosition() - p1->getPosition();
		rhat.normalize();
		if(r < 1) r = 1;
		fe = k  *  q1 * q2 / (r * r);
		p1->appendForce(rhat * -fe * _animationspeed);
		p2->appendForce(rhat * fe * _animationspeed);
	}


	static Vector3 calculateElectricField(Vector3 pos)
	{
		Vector3 net = Vector3(0,0,0);
		for(int c = 0; c < particles.size(); c++)
		{
			float q;
			float r;
			Vector3 rhat;
			q = particles[c]->getCharge();
			r = particles[c]->getPosition().distanceFrom(pos);
			rhat = pos - particles[c]->getPosition();
			rhat.normalize();
			if(r < 0.5) r = 0.5;
			net += (rhat * (k * q / (r * r)));
		}
		return net;
	}

	static void clearAllForces(void)
	{
		for(int c = 0; c < particles.size(); c++)
		{
			particles[c]->getForces()->clear();
			particles[c]->setFnet(Vector3(0,0,0));
		}
	}

	static void recalculateForces(void)
	{
		clearAllForces();
		for(int i = 0; i < ElectroDynamic::particles.size(); i++)
		{
			for(int j = i + 1; j < ElectroDynamic::particles.size(); j++)
			{
				ElectroDynamic::calculateElectricForce(ElectroDynamic::particles[i],ElectroDynamic::particles[j]);
			}
		}

		for(int c = 0; c < particles.size(); c++)
		{
			for(int j = 0; j < particles[c]->getForces()->size(); j++)
			{
				*(particles[c]->getFnetByReference()) += particles[c]->getForceAt(j);
			}
			*(particles[c]->getFnetByReference()) = particles[c]->getFnet()*100000;
			particles[c]->setAcceleration(particles[c]->getFnet() / particles[c]->getMass());
			//particles[c]->getFnet().print();
		}
	}

	struct FieldSensor
	{
		Vector3 pos;
		Vector3 field;
		FieldSensor(Vector3 _pos, Vector3 _field) : pos(_pos), field(_field) {};
		
		void Update()
		{
			field = calculateElectricField(pos);
		};

		void Render(Vector3 cameraPos, bool selected)
		{
			glTranslatef(cameraPos.getX(),cameraPos.getY(),cameraPos.getZ() + _zdefault);
			glRotatef(Entity::yangle,0,1,0);

			glTranslatef(pos.getX(), pos.getY(), pos.getZ() - _zdefault);

			ChargedParticle::drawArrow(0,0,0,field.getX() * ArrowLength, field.getY() * ArrowLength, field.getZ() * ArrowLength, 0.1, selected);

			glBindTexture(GL_TEXTURE_2D,arrowTexture);

			glLoadIdentity();
		};
	};

	static std::vector<FieldSensor *> fields;
};

