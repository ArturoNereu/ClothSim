/*
** Cloth Simulation
** Author: Arturo Nereu
*/

#include "Vector.h"

Vector::Vector()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}

Vector::Vector(float pX, float pY, float pZ)
{
    //ctor
    x = pX;
    y = pY;
    z = pZ;
}

Vector::~Vector()
{
    //dtor
}

float Vector::getLength()
{
    return (sqrt((x * x) + (y * y) + (z * z)));
}

void Vector::normalize()
{
    float length = getLength();

    this->x = this->x / length;
    this->y = this->y / length;
    this->z = this->z / length;
}

void Vector::substract(Vector *res, Vector v1, Vector v2)
{
    res->x = v2.x - v1.x;
    res->y = v2.y - v1.y;
    res->z = v2.z - v1.z;
}

void Vector::crossProduct(Vector *res, Vector v1, Vector v2)
{
    res->x = v1.y * v2.z - v1.z * v2.y;
    res->y = v1.z * v2.x - v1.x * v2.z;
    res->z = v1.x * v2.y - v1.y * v2.x;
}

void Vector::multiplicationByScalar(Vector *res, Vector v1, float scalarValue)
{
    res->x = v1.x * scalarValue;
    res->y = v1.y * scalarValue;
    res->z = v1.z * scalarValue;
}

float Vector::dotProduct(Vector v1, Vector v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}
