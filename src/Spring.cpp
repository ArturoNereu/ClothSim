/*
** Cloth Simulation
** Author: Arturo Nereu
*/

#include "Spring.h"

Spring::Spring(Particle *p1, Particle *p2, float normalLength)
{
    //ctor
    this->p1 = p1;
    this->p2 = p2;

    this->ks = 20;
    this->kd = 0.2f;

    this->normalLength = normalLength;
}

Spring::~Spring()
{
    //dtor
}

void Spring::update()
{
    applySpringForce();
}

void Spring::render()
{
    glColor3f(0, 1, 1);
    glBegin(GL_LINES);
        glVertex3f(p1->position->x, p1->position->y, p1->position->z);
        glVertex3f(p2->position->x, p2->position->y, p2->position->z);
    glEnd();
}

void Spring::applySpringForce()
{
    //The distance between both particles in the spring
    float ex = p2->position->x - p1->position->x;
    float ey = p2->position->y - p1->position->y;
    float ez = p2->position->z - p1->position->z;

    //Vector *springVector = new Vector();
    //Vector::substract(springVector, *(p1->position), *(p2->position));

    float length = sqrt(pow(ex, 2) + pow(ey, 2) + pow(ez, 2));

    //Normalize the vector
    //springVector->normalize();
    ex = ex / length;
    ey = ey / length;
    ez = ez / length;

    //Calculate Vi scalars
    //float v1 = Vector::dotProduct(*(p1->velocity), *(springVector));
    //float v2 = Vector::dotProduct(*(p2->velocity), *(springVector));
    float v1 = ex * p1->velocity->x + ey * p1->velocity->y + ez * p1->velocity->z;
    float v2 = ex * p2->velocity->x + ey * p2->velocity->y + ez * p2->velocity->z;

    //calculate Fsd
    //printf("normalLength(%f) - length(%f) = %f\n", normalLength, this->getLength(), normalLength-this->getLength());
    float fSD = -ks * (normalLength - this->getLength()) - kd * (v1 - v2);

    //The vector to apply
    //Vector *forceResult = new Vector();
    //forceResult->x = springVector->x * fSD;
    //forceResult->y = springVector->y * fSD;
    //forceResult->z = springVector->z * fSD;

    //printf("Force x: %f, y: %f, z: %f", forceResult->x, forceResult->y, forceResult->z);
    float fx = fSD * ex;
    float fy = fSD * ey;
    float fz = fSD * ez;

    p1->addForce(fx, fy, fz);
    p2->addForce(-fx, -fy, -fz);
    //p1->addForce(forceResult->x, forceResult->y, forceResult->z);
    //p2->addForce(-forceResult->x, -forceResult->y, -forceResult->z);
}

float Spring::getLength()
{
    Vector *dist = new Vector();
    Vector::substract(dist, *(p1->position), *(p2->position));
    return dist->getLength();
}
