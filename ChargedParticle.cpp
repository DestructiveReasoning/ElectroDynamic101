#include "ChargedParticle.h"

ChargedParticle::ChargedParticle(int model, int texture, Vector3 pos, float _mass, float _charge, bool isMobile, Vector3 *cameraPos) :
	Entity(model,texture,pos,Vector3(0,0,0),Vector3(0,0,0),cameraPos),
	charge(_charge)
{
	mobile = isMobile;
	mass = _mass;
}

void ChargedParticle::customUpdate()
{
	(charge < 0) ? texture = 1 : texture = 5;
}

void ChargedParticle::drawArrow(GLdouble x1,GLdouble y1,GLdouble z1,GLdouble x2,GLdouble y2,GLdouble z2,GLdouble D, bool selected)
{
	double dx=x2-x1; //X length of arrow vector
	double dy=y2-y1; //Y length of arrow vector
	double dz=z2-z1; //Z length of arrow vector
	double L=sqrt(dx*dx+dy*dy+dz*dz); //Total length of arrow vector
	int gluFillMode = GLU_FILL;
	//selected ? glBindTexture(GL_TEXTURE_2D,ElectroDynamic::arrowTextureSelected) : glBindTexture(GL_TEXTURE_2D,ElectroDynamic::arrowTexture);

	GLUquadricObj *quadObj;

	glPushMatrix (); //New matrix, so scene matrix doesn't change
		glTranslated(x1,y1,z1); //Translate to initial point of arrow

		//If vector has portion in the xy plane
		if((dx!=0.)||(dy!=0.)) {
			glRotated(atan2(dy,dx) * 180 / M_PI,0.,0.,1.);
			glRotated(atan2(sqrt(dx*dx+dy*dy),dz) * 180 / M_PI,0.,1.,0.);
		} else if (dz<0){
			glRotated(180,1.,0.,0.);
		}

		/*Translate to base of arrow head*/
		glTranslatef(0,0,L-4*D);

		/*Draw Arrow Head*/
		quadObj = gluNewQuadric ();
		gluQuadricDrawStyle (quadObj, gluFillMode);
		gluQuadricNormals (quadObj, GLU_SMOOTH);
		gluCylinder(quadObj, 2*D, 0.0, 4*D, 32, 1);
		gluDeleteQuadric(quadObj);

		/*Draw base of Arrow Head*/
		quadObj = gluNewQuadric ();
		gluQuadricDrawStyle (quadObj, gluFillMode);
		gluQuadricNormals (quadObj, GLU_SMOOTH);
		gluDisk(quadObj, 0.0, 2*D, 32, 1);
		gluDeleteQuadric(quadObj);

		/*Translate to initial point of arrow vector*/
		glTranslatef(0,0,-L+4*D);

		/*Draw arrow line*/
		quadObj = gluNewQuadric ();
		gluQuadricDrawStyle (quadObj, gluFillMode);
		gluQuadricNormals (quadObj, GLU_SMOOTH);
		gluCylinder(quadObj, D, D, L-4*D, 32, 1);
		gluDeleteQuadric(quadObj);

		/*Draw base area of arrow line*/
		quadObj = gluNewQuadric ();
		gluQuadricDrawStyle (quadObj, gluFillMode);
		gluQuadricNormals (quadObj, GLU_SMOOTH);
		gluDisk(quadObj, 0.0, D, 32, 1);
		gluDeleteQuadric(quadObj);

	glPopMatrix ();
}

ChargedParticle::~ChargedParticle(void)
{
	using namespace ElectroDynamic;
	for(int c = 0; c < particles.size(); c++)
	{
		if(particles[c] == this) particles[c] = particles.back();
		particles.pop_back();
	}
}
