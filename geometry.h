#ifndef GEOMETRY_H
#define GEOMETRY_H

class Vect2{
public :
    float tab[2];
    Vect2(){}
    Vect2(float x,float y);
};

class Vect3{
public :
    float tab[3];
    Vect3(){}
    Vect3(float x,float y,float z);
};

Vect3 cross(Vect3 v1,Vect3 v2);

Vect2 operator+(Vect2 const& v,Vect2 const& v2);

Vect2 operator-(Vect2 const& v,Vect2 const& v2);

Vect2 operator*(Vect2 const& v,float f);

#endif // GEOMETRY_H
