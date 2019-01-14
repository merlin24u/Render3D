#include "geometry.h"

Vect2::Vect2(float _x,float _y){
    x = _x;
    y = _y;
}

Vect3::Vect3(float _x,float _y,float _z):Vect2(_x,_y){
    z = _z;
}

float Vect2::get(unsigned int i){
    switch (i) {
    case 0:
        return x;
    case 1:
        return y;
    default:
        return -1;
    }
}

void Vect2::set(unsigned int i, float f){
    switch (i) {
    case 0:
        x = f;
        break;
    case 1:
        y = f;
        break;
    default:
        break;
    }
}

float Vect3::get(unsigned int i){
    if(i==2)
        return z;
    else
        return Vect2::get(i);
}

void Vect3::set(unsigned int i, float f){
    if(i==2)
        z = f;
    else Vect2::set(i,f);
}

Vect2 operator+(Vect2 const& v,Vect2 const& v2){
    Vect2 res;
    res.x = v.x + v2.x;
    res.y = v.y + v2.y;
    return res;
}

Vect2 operator-(Vect2 const& v,Vect2 const& v2){
    Vect2 res;
    res.x = v.x - v2.x;
    res.y = v.y - v2.y;
    return res;
}

Vect2 operator*(Vect2 const& v,float f){
    Vect2 res;
    res.x = f*v.x;
    res.y = f*v.y;
    return res;
}

Vect3 cross(Vect3 v1,Vect3 v2){
    return Vect3(v1.y*v2.z - v1.z*v2.y,
                 v1.z*v2.x - v1.x*v2.z,
                 v1.x*v2.y - v1.y*v2.x);
}
