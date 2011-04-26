/*
** Cloth Simulation
** Author: Arturo Nereu
*/

#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include "Vector.h"

#ifndef PARTICLE_H
#define PARTICLE_H

class Particle
{
    public:
        float mass;

        int posI, posJ;
        float radius;

        int numberOfAdjTriangles;
        int adjTriangles[6];

        Vector *position;
        Vector *prevPosition;
        Vector *forceAcum;
        Vector *velocity;
        bool isFixed;
        Vector *normal;

        Particle();
        virtual ~Particle();

        void setPosition(float pX, float pY, float pZ, int posI, int posJ);
        void clearForces();
        void addForce(float x, float y, float z);
        void render();
        Vector *verletIntegrationToVelocity();
        Vector *verletIntegrationToPosition();
        void update(int sphereRadius, float *objPos);
        void checkCollisionWithSphere(int radius, float *objPos);
        void checkCollisionWithCapsule(Vector p0, Vector p1, float radius);

    protected:
    private:
};

#endif // PARTICLE_H
