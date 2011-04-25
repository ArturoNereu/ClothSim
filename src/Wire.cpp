/*
** Cloth Simulation
** Author: Arturo Nereu
*/

#include "Wire.h"
#include <stdio.h>

Wire::Wire(int resolution, int size)
{
    //ctor

    this->resolution = resolution;
    delta = size / (resolution * 1.0f);
    numberOfTriangles = (resolution - 1) * (resolution - 1) * 2;
    //numberOfSprings = (resolution + ((resolution-1) * 2)) * (resolution - 1) + (resolution - 1);
    numberOfSprings = ((((2 * resolution) - 1) + ((resolution - 1) * 2)) * (resolution - 1)) + (resolution - 1);
    particles = new Particle*[resolution];
    triangles = new Triangle*[numberOfTriangles];

    springs = new Spring*[numberOfSprings];

    for(int i = 0; i < resolution; i++)
    {
        *(particles + i) = new Particle[resolution];
    }

    for(int i = 0; i < resolution; i++)
    {
        for(int j = 0; j < resolution; j++)
        {
            Particle *tmpParticle = new Particle();
            tmpParticle->setPosition((i - resolution / 2) * delta, 100, ((j - resolution / 2) * delta) + 100, i, j);
            if(i == 0)
                tmpParticle->isFixed = true;

            addParticle(tmpParticle, i, j);
        }
    }
    generateTriangles(resolution);
    generateSprings();
}

Wire::~Wire()
{
    //dtor
}

void Wire::addParticle(Particle *particle, int i, int j)
{
    particles[i][j] = *particle;
}

void Wire::generateTriangles(int resolution)
{
    int trianglesIndex = 0;

    for(int i = 0; i < resolution - 1; i++)
    {
        for(int j = 0; j < resolution - 1; j++)
        {
            Triangle *tmpTriangle = new Triangle();
            tmpTriangle->setTriangle(&particles[i][j], &particles[i][j+1], &particles[i+1][j]);

            //add the index of adjacent triangles to each particle
            int currentIndex = particles[i][j].numberOfAdjTriangles;
            particles[i][j].adjTriangles[currentIndex] = trianglesIndex;
            particles[i][j].numberOfAdjTriangles++;

            currentIndex = particles[i][j+1].numberOfAdjTriangles;
            particles[i][j+1].adjTriangles[currentIndex] = trianglesIndex;
            particles[i][j+1].numberOfAdjTriangles++;

            currentIndex = particles[i+1][j].numberOfAdjTriangles;
            particles[i+1][j].adjTriangles[currentIndex] = trianglesIndex;
            particles[i+1][j].numberOfAdjTriangles++;

            triangles[trianglesIndex] = tmpTriangle;
            trianglesIndex++;

            tmpTriangle = new Triangle();
            tmpTriangle->setTriangle(&particles[i][j+1], &particles[i+1][j+1], &particles[i+1][j]);

            //add the index of adjacent triangles to each particle
            currentIndex = particles[i][j+1].numberOfAdjTriangles;
            particles[i][j+1].adjTriangles[currentIndex] = trianglesIndex;
            particles[i][j+1].numberOfAdjTriangles++;

            currentIndex = particles[i+1][j+1].numberOfAdjTriangles;
            particles[i+1][j+1].adjTriangles[currentIndex] = trianglesIndex;
            particles[i+1][j+1].numberOfAdjTriangles++;

            currentIndex = particles[i+1][j].numberOfAdjTriangles;
            particles[i+1][j].adjTriangles[currentIndex] = trianglesIndex;
            particles[i+1][j].numberOfAdjTriangles++;

            triangles[trianglesIndex] = tmpTriangle;
            trianglesIndex++;
        }
    }
}

void Wire::renderParticles()
{
    glPointSize(4);

    glDisable(GL_TEXTURE_2D);

    for(int i = 0; i < resolution; i++)
    {
        for(int j = 0; j < resolution; j++)
        {
            if(this->particles[i][j].isFixed)
                glColor3f(1, 0, 0);
            else
                glColor3f(0, 1, 1);

            this->calculateParticleNormal(i, j);
            this->particles[i][j].render();
        }
    }

    glEnable(GL_TEXTURE_2D);
    glPointSize(1);
}

void Wire::calculateParticleNormal(int c, int r)
{
    float x = 0;
    float y = 0;
    float z = 0;
    float length = 0;

    for(int i = 0; i < particles[c][r].numberOfAdjTriangles; i++)
    {
        x += triangles[particles[c][r].adjTriangles[i]]->normal->x;
        y += triangles[particles[c][r].adjTriangles[i]]->normal->y;
        z += triangles[particles[c][r].adjTriangles[i]]->normal->z;

        length += triangles[particles[c][r].adjTriangles[i]]->normal->getLength();
    }

    particles[c][r].normal->x = x / length;
    particles[c][r].normal->y = y / length;
    particles[c][r].normal->z = z / length;

    //printf("Normal: %f, %f, %f\n", particles[c][r].normal->x, particles[c][r].normal->y, particles[c][r].normal->z);
}

void Wire::renderTriangles(bool showWireframe, bool flatShade)
{
    for(int i = 0; i < numberOfTriangles; i++)
    {
        triangles[i]->render(resolution, showWireframe, flatShade);
    }
}

void Wire::renderSprings()
{
    for(int i = 0; i < numberOfSprings; i++)
    {
        springs[i]->render();
    }
}

void Wire::update(int sphereRadius, float gravityForce, Vector gravityDirection, float ks, float kd, float particleRadius)
{
    for(int i = 0; i < resolution; i++)
    {
        for(int j = 0; j < resolution; j++)
        {
            particles[i][j].radius = particleRadius;
            particles[i][j].clearForces();
            particles[i][j].addForce(gravityDirection.x * gravityForce, gravityDirection.y * gravityForce, gravityDirection.z * gravityForce);
        }
    }

    for(int i = 0; i < numberOfSprings; i++)
    {
        springs[i]->update(ks, kd);
    }

    for(int i = 0; i < resolution; i++)
    {
        for(int j = 0; j < resolution; j++)
        {
            particles[i][j].update(sphereRadius);
        }
    }
}

void Wire::generateSprings()
{
    //We use this index to iterate through the list of springs
    int springIndex = 0;
    float diagonalDelta = sqrt(pow(delta, 2) +  pow(delta, 2));

    //Iterate through each Particle to create our springs
    for(int i = 0; i < resolution; i++)
    {
        for(int j = 0; j < resolution; j++)
        {
            //If we are at the right border of the cloth and not in the bottom
            if(i == resolution-1 && j < resolution-1)
            {
                springs[springIndex] = new Spring(&particles[i][j], &particles[i][j+1], delta);
                springIndex++;
            }
            else if(j == 0)//We are at the top of the cloth, we will just create 2 springs here
            {
                springs[springIndex] = new Spring(&particles[i][j], &particles[i+1][j], delta);
                springIndex++;

                springs[springIndex] = new Spring(&particles[i][j], &particles[i][j+1], delta);
                springIndex++;
            }
            else if(j < resolution - 1) //We are between the top and down borders of the cloth
            {
                springs[springIndex] = new Spring(&particles[i][j], &particles[i+1][j-1], diagonalDelta);
                springIndex++;

                springs[springIndex] = new Spring(&particles[i][j-1], &particles[i+1][j], diagonalDelta);
                springIndex++;

                springs[springIndex] = new Spring(&particles[i][j], &particles[i+1][j], delta);
                springIndex++;

                springs[springIndex] = new Spring(&particles[i][j], &particles[i][j+1], delta);
                springIndex++;
            }
            else if( i == resolution-1 && j == resolution-1)
            {
                continue;
            }
            else //we are at the bottom of the cloth
            {
                springs[springIndex] = new Spring(&particles[i][j], &particles[i+1][j-1], diagonalDelta);
                springIndex++;

                springs[springIndex] = new Spring(&particles[i][j], &particles[i+1][j], delta);
                springIndex++;

                springs[springIndex] = new Spring(&particles[i][j-1], &particles[i+1][j], delta);
                springIndex++;
            }
        }
    }

    printf("Springs: %d, expected springs: %d", springIndex, numberOfSprings);
}
