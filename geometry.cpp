#include "geometry.h"

Vect2::Vect2(float x,float y){
    tab[0] = x;
    tab[1] = y;
}

Vect3::Vect3(float x,float y,float z){
    tab[0] = x;
    tab[1] = y;
    tab[2] = z;
}

Vect3 cross(Vect3 v1,Vect3 v2){
    return Vect3(v1.tab[1]*v2.tab[2] - v1.tab[2]*v2.tab[1],
            v1.tab[2]*v2.tab[0] - v1.tab[0]*v2.tab[2],
            v1.tab[0]*v2.tab[1] - v1.tab[1]*v2.tab[0]);
}

Vect2 operator+(Vect2 const& v,Vect2 const& v2){
    Vect2 res;
    res.tab[0] = v.tab[0] + v2.tab[0];
    res.tab[1] = v.tab[1] + v2.tab[1];
    return res;
}

Vect2 operator-(Vect2 const& v,Vect2 const& v2){
    Vect2 res;
    res.tab[0] = v.tab[0] - v2.tab[0];
    res.tab[1] = v.tab[1] - v2.tab[1];
    return res;
}

Vect2 operator*(Vect2 const& v,float f){
    Vect2 res;
    res.tab[0] = f*v.tab[0];
    res.tab[1] = f*v.tab[1];
    return res;
}
