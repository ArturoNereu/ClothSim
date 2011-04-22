/*
** Cloth Simulation
** Author: Arturo Nereu
*/

#include "Particle.h"

Particle::Particle()
{
    //ctor
    forceAcum = new Vector();
    velocity = new Vector();
    this->radius = 0.1;
}

Particle::~Particle()
{
    //dtor
}

void Particle::setPosition(float pX, float pY, float pZ, int posI, int posJ)
{
    //The position relative to the index in the mesh
    this->posI = posI;
    this->posJ = posJ;

    //position variables
    position = new Vector(pX, pY, pZ);
    prevPosition = new Vector(pX, pY, pZ);
}

void Particle::clearForces()
{
    forceAcum->x = 0.0f;
    forceAcum->y = 0.0f;
    forceAcum->z = 0.0f;
}

void Particle::addForce(float x, float y, float z)
{
    forceAcum->x += x;
    forceAcum->y += y;
    forceAcum->z += z;
}

void Particle::render()
{
    glPointSize(3.0f);

    if(isFixed)
        glColor3f(0, 0, 1);
    else
        glColor3f(1, 1, 1);

    glBegin(GL_POINTS);
        glVertex3f(position->x, position->y, position->z);
    glEnd();
}

Vector * Particle::verletIntegrationToPosition()
{
    Vector *nextPos = new Vector();

    //delta time missing

    //Verlet corregido
    //nextPos->x = position->x + (position->x - prevPosition->x) + forceAcum->x * 0.1f * 0.1f;
    //nextPos->y = position->y + (position->y - prevPosition->y) + forceAcum->y * 0.1f * 0.1f;
    //nextPos->z = position->z + (position->z - prevPosition->z) + forceAcum->z * 0.1f * 0.1f;

    //Verlet sin corregir
    nextPos->x = (2 * position->x) - prevPosition->x + forceAcum->x * 0.1f * 0.1f;
    nextPos->y = (2 * position->y) - prevPosition->y + forceAcum->y * 0.1f * 0.1f;
    nextPos->z = (2 * position->z) - prevPosition->z + forceAcum->z * 0.1f * 0.1f;

    return nextPos;
}

void Particle::update()
{
    velocity->x = forceAcum->x * 0.1f;
    velocity->y = forceAcum->y * 0.1f;
    velocity->z = forceAcum->z * 0.1f;

    float tempX = position->x;
    float tempY = position->y;
    float tempZ = position->z;

    Vector *nextPos = verletIntegrationToPosition();

    if(!isFixed)
    {
        position->x = nextPos->x;
        position->y = nextPos->y;
        position->z = nextPos->z;
    }

    prevPosition->x = tempX;
    prevPosition->y = tempY;
    prevPosition->z = tempZ;

    checkCollision();
}

void Particle::checkCollision()
{
    float penDist;
    Vector *spherePos = new Vector(0, -250, 0);
    Vector *dist = new Vector();
    Vector::substract(dist, *position, *spherePos);
    penDist = dist->getLength() - (100 + this->radius);

    dist->normalize();

    Vector *P = new Vector();
    Vector::multiplicationByScalar(P, *dist, penDist);

    if(penDist < 0)
    {
        this->position->x += P->x;
        this->position->y += P->y;
        this->position->z += P->z;
    }

}
