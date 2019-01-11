#ifndef GEOMETRY_H
#define GEOMETRY_H

class Vect2{
public :
    float tab[2];
    Vect2(){}
    Vect2(int x,int y){
        tab[0] = x;
        tab[1] = y;
    }
};

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

#endif // GEOMETRY_H
