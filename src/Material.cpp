/*
** Cloth Simulation
** Author: Arturo Nereu
*/

#include "Material.h"

Material::Material()
{
    //ctor
}

Material::~Material()
{
    //dtor
}

void Material::setAmbient(float x, float y, float z, float e)
{
    ambient[0] = x;
    ambient[1] = y;
    ambient[2] = z;
    ambient[3] = e;
}

void Material::setDifusse(float x, float y, float z, float e)
{
    difusse[0] = x;
    difusse[1] = y;
    difusse[2] = z;
    difusse[3] = e;
}

void Material::setSpecular(float x, float y, float z, float e)
{
    specular[0] = x;
    specular[1] = y;
    specular[2] = z;
    specular[3] = e;
}

void Material::setShininess(float shine)
{
    shininess = shine;
}

