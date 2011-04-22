/*
** Cloth Simulation
** Author: Arturo Nereu
*/

#include <stdio.h>
#include "Particle.h"

#ifndef TRIANGLE_H
#define TRIANGLE_H


class Triangle
{
    public:
        Particle *particles[3];
        Vector *normal;

        Triangle();
        virtual ~Triangle();

        void setTriangle(Particle *p1, Particle *p2, Particle *p3);
        void calculateNormal();
        void render(int resolution, bool showWireframe);
    protected:
    private:
};

#endif // TRIANGLE_H
