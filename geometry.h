#ifndef GEOMETRY_H
#define GEOMETRY_H

class Vect2{
public :
    float x,y;
    Vect2(){}
    Vect2(float _x,float _y);
    float get(unsigned int i);
    void set(unsigned int i, float f);
};

class Vect3 : public Vect2{
public :
    float z;
    Vect3(){}
    Vect3(float _x,float _y,float _z);
    float get(unsigned int i);
    void set(unsigned int i, float f);
};

Vect3 cross(Vect3 v1,Vect3 v2);

Vect2 operator+(Vect2 const& v,Vect2 const& v2);

Vect2 operator-(Vect2 const& v,Vect2 const& v2);

Vect2 operator*(Vect2 const& v,float f);

#endif // GEOMETRY_H
