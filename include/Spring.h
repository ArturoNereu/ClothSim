/*
** Cloth Simulation
** Author: Arturo Nereu
*/

#ifndef SPRING_H
#define SPRING_H

#include "Particle.h"
#include "Vector.h"

class Spring
{
    public:
        Particle *p1; //Particles connected by this spring
        Particle *p2;
        float ks; //Hook coef
        float kd; //elasticity coef
        float normalLength; //resting distance of the spring

        Spring();
        Spring(Particle *p1, Particle *p2, float normalLength);
        virtual ~Spring();

        void applyForce();
        void update(float ks, float kd);
        void render();
        void applySpringForce();
        float getLength();

    protected:
    private:
};

#endif // SPRING_H
