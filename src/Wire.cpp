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
            tmpParticle->setPosition((i - resolution / 2) * delta, 0, (j - resolution / 2) * delta, i, j);
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
            triangles[trianglesIndex] = tmpTriangle;
            trianglesIndex++;

            tmpTriangle = new Triangle();
            tmpTriangle->setTriangle(&particles[i][j+1], &particles[i+1][j+1], &particles[i+1][j]);
            triangles[trianglesIndex] = tmpTriangle;
            trianglesIndex++;
        }
    }
}

void Wire::renderParticles()
{
    for(int i = 0; i < resolution; i++)
        {
            for(int j = 0; j < resolution; j++)
            {
                    glColor3f(1, 1, 1);
                    this->particles[i][j].render();
            }
        }
}

void Wire::renderTriangles(bool showWireframe)
{
    for(int i = 0; i < numberOfTriangles; i++)
    {
        triangles[i]->render(resolution, showWireframe);
    }
}

void Wire::renderSprings()
{
    for(int i = 0; i < numberOfSprings; i++)
    {
        springs[i]->render();
    }
}

void Wire::update()
{
    for(int i = 0; i < resolution; i++)
    {
        for(int j = 0; j < resolution; j++)
        {
            particles[i][j].clearForces();
            particles[i][j].addForce(0, -10.0f, 0);
        }
    }

    for(int i = 0; i < numberOfSprings; i++)
    {
        springs[i]->update();
    }

    for(int i = 0; i < resolution; i++)
    {
        for(int j = 0; j < resolution; j++)
        {
            particles[i][j].update();
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
