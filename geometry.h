#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>

class Vect2f{
public :
    float x,y;
    Vect2f():x(0),y(0){}
    Vect2f(float _x,float _y);
    float get(unsigned int i);
    void set(unsigned int i, float f);
};

class Vect3f : public Vect2f{
public :
    float z;
    Vect3f():Vect2f(),z(0){}
    Vect3f(float _x,float _y,float _z);
    float get(unsigned int i);
    void set(unsigned int i, float f);
    void normalize();
    float norm();
};

Vect3f operator-(Vect3f const& v,Vect3f const& v2);

Vect3f operator+(Vect3f const& v,Vect3f const& v2);

float operator*(Vect3f const& v,Vect3f const& v2);

Vect3f operator*(Vect3f const& v,float f);

std::ostream& operator<<(std::ostream &flux,Vect2f const& v);

std::ostream& operator<<(std::ostream &flux,Vect3f const& v);

Vect3f cross(Vect3f v1,Vect3f v2);

// Matrix

const int SIZE_M = 4;

class Matrix{
    std::vector<std::vector<float>> tab;
    int nbR, nbC;
public:
    Matrix(int r=SIZE_M,int c=SIZE_M);
    static Matrix identity(int d);
    int getR();
    int getC();
    std::vector<float>& operator[](const int i);
    Matrix operator*(Matrix const& m);
};

std::ostream& operator<<(std::ostream &flux,Matrix& v);

#endif // GEOMETRY_H
