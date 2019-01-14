#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "tgaimage.h"
#include "geometry.h"

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int height = 800;
const int width = 800;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (abs(x0-x1)<abs(y0-y1)) { // if the line is steep, transpose the image
        swap(x0, y0);
        swap(x1, y1);
        steep = true;
    }
    if (x0>x1) { // make it left−to−right
        swap(x0, x1);
        swap(y0, y1);
    }
    for (int x=x0; x<=x1; x++) {
        float t = (x-x0)/(float)(x1-x0);
        int y = y0*(1.-t) + y1*t;
        if (steep)
            image.set(y, x, color); // if transposed, de−transpose
        else
            image.set(x, y, color);
    }
}

Vect3 barycentric(Vect2* v,Vect2 V){
    Vect3 b = cross(Vect3(v[2].x-v[0].x, v[1].x-v[0].x, v[0].x-V.x), Vect3(v[2].y-v[0].y, v[1].y-v[0].y, v[0].y-V.y));
    if (abs(b.z)<1)
        return Vect3(-1,1,1);
    return Vect3(1.f-(b.x+b.y)/b.z, b.y/b.z, b.x/b.z);
}

void triangle(Vect2* v,TGAImage &image, TGAColor color){
    Vect2 boxmin(image.get_width()-1,  image.get_height()-1);
    Vect2 boxmax(0, 0);
    Vect2 tmp = boxmin;
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            boxmin.set(j, max(0.f, min(boxmin.get(j), v[i].get(j))));
            boxmax.set(j, min(tmp.get(j), max(boxmax.get(j), v[i].get(j))));
        }
    }

    Vect2 V;
    for (V.x = boxmin.x; V.x<=boxmax.x; V.x++) {
        for (V.y = boxmin.y; V.y<=boxmax.y; V.y++) {
            Vect3 bc_screen  = barycentric(v, V);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0)
                continue;
            image.set(V.x, V.y, color);
        }
    }
}

void triangle_old(Vect2 v1,Vect2 v2,Vect2 v3,TGAImage &image, TGAColor color){
    if(v1.y==v2.y && v1.y==v3.y) return;
    if(v1.y>v2.y)
        swap(v1,v2);
    if(v1.y>v3.y)
        swap(v1,v3);
    if(v2.y>v3.y)
        swap(v2,v3);
    int total_h = v3.y-v1.y;
    for(int i=0;i<total_h;i++){
        bool second_h = i>v2.y-v1.y || v2.y==v1.y;
        int segment_h = second_h ? v3.y-v2.y : v2.y-v1.y;
        float alpha = (float)i/total_h;
        float beta  = (float)(i-(second_h ? v2.y-v1.y : 0))/segment_h;
        Vect2 A = v1 + (v3-v1)*alpha;
        Vect2 B = second_h ? v2 + (v3-v2)*beta : v1 + (v2-v1)*beta;
        if (A.x>B.x)
            swap(A, B);
        for (int j=A.x; j<=B.x; j++) {
            image.set(j, v1.y+i, color);
        }
    }
}

int main(int argc, char** argv) {
    ifstream file;
    if(argc==2)
        file.open (argv[1], ifstream::in);
    else
        file.open ("obj/african_head.obj", ifstream::in);

    if (file.fail()) {
        cout << "Erreur lors de l'ouverture du fichier" << endl;
        return 0 ;
    }

    vector<Vect2> points;
    vector<Vect3> faces;
    string l;
    while(!file.eof()){
        getline(file, l);
        istringstream stream(l.c_str());
        char t;
        if (l.compare(0, 2, "v ") == 0) {
            float f;
            stream >> t;
            Vect2 v;
            for(int i=0;i<2;i++){
                stream >> f;
                v.set(i,f);
            }
            points.push_back(v);
        }else if(l.compare(0, 2, "f ") == 0){
            Vect3 v;
            int index, t2;
            stream >> t;
            for(int i=0;i<3;i++){
                stream >> index >> t >> t2 >> t >> t2;
                v.set(i,index-1);
            }
            faces.push_back(v);
        }
    }

    TGAImage image(width, height, TGAImage::RGB);
    for(vector<Vect3>::iterator it = faces.begin(); it != faces.end(); ++it) {
        Vect2 tab[3];
        for(int i=0;i<3;i++){
            Vect2 v = points[it->get(i)];
            tab[i] = Vect2((v.x+1.)*width/2.,(v.y+1.)*height/2.);
        }
        //triangle_old(tab[0],tab[1],tab[2],image,TGAColor(rand()%255, rand()%255, rand()%255, 255));
        triangle(tab,image,TGAColor(rand()%255, rand()%255, rand()%255, 255));
    }

    image.flip_vertically(); // origin at the left bottom corner of the image
    image.write_tga_file("image.tga");
    return 0;
}
