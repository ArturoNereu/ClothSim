/*
** Cloth Simulation
** Author: Arturo Nereu
*/

#ifndef WIRE_H
#define WIRE_H

#include <windows.h>
#include <GL/glut.h>
#include "Particle.h"
#include "Triangle.h"
#include "Spring.h"

class Wire
{
    public:
        Particle **particles;
        int numberOfParticles;

        Triangle **triangles;
        int numberOfTriangles;

        Spring **springs;
        int numberOfSprings;

        float delta;
        int resolution;

        Wire(int resolution, int size);
        virtual ~Wire();

        void addParticle(Particle *particle, int i, int j);
        void generateTriangles(int resolution);
        void renderParticles();
        void renderTriangles(bool showWireframe, bool flatShade);
        void renderSprings();
        void update(int sphereRadius);
        void generateSprings();
        void calculateParticleNormal(int c, int r);
    protected:
    private:
};

#endif // WIRE_H
