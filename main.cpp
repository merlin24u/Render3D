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
    if (abs(x0-x1)<abs(y0-y1)) { // if the line is steep, we transpose the image
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
    Vect3 b = cross(Vect3(v[2].tab[0]-v[0].tab[0], v[1].tab[0]-v[0].tab[0], v[0].tab[0]-V.tab[0]), Vect3(v[2].tab[1]-v[0].tab[1], v[1].tab[1]-v[0].tab[1], v[0].tab[1]-V.tab[1]));
    if (b.tab[2]==0)
        return Vect3(-1,1,1);
    return Vect3(1.f-(b.tab[0]+b.tab[1])/b.tab[2], b.tab[1]/b.tab[2], b.tab[0]/b.tab[2]);
}

void triangle(Vect2* v,TGAImage &image, TGAColor color){
    Vect2 boxmin(image.get_width()-1,  image.get_height()-1);
    Vect2 boxmax(0, 0);
    Vect2 tmp = boxmin;
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            boxmin.tab[j] = max(0.f, min(boxmin.tab[j], v[i].tab[j]));
            boxmax.tab[j] = min(tmp.tab[j], max(boxmax.tab[j], v[i].tab[j]));
        }
    }

    //cout << boxmin.tab[0] << " " << boxmin.tab[1] << " / " << boxmax.tab[0] << " " <<  boxmax.tab[1] << endl;

    Vect2 V;
    for (V.tab[0]=boxmin.tab[0]; V.tab[0]<=boxmax.tab[0]; V.tab[0]++) {
        for (V.tab[1]=boxmin.tab[1]; V.tab[1]<=boxmax.tab[1]; V.tab[1]++) {
            Vect3 bc_screen  = barycentric(v, V);
            if (bc_screen.tab[0]<0 || bc_screen.tab[1]<0 || bc_screen.tab[2]<0)
                continue;
            image.set(V.tab[0], V.tab[1], color);
        }
    }
}

void triangle_old(Vect2 v1,Vect2 v2,Vect2 v3,TGAImage &image, TGAColor color){
    if(v1.tab[1]==v2.tab[1] && v1.tab[1]==v3.tab[1]) return;
    if(v1.tab[1]>v2.tab[1])
        swap(v1,v2);
    if(v1.tab[1]>v3.tab[1])
        swap(v1,v3);
    if(v2.tab[1]>v3.tab[1])
        swap(v2,v3);
    int total_h = v3.tab[1]-v1.tab[1];
    for(int i=0;i<total_h;i++){
        bool second_h = i>v2.tab[1]-v1.tab[1] || v2.tab[1]==v1.tab[1];
        int segment_h = second_h ? v3.tab[1]-v2.tab[1] : v2.tab[1]-v1.tab[1];
        float alpha = (float)i/total_h;
        float beta  = (float)(i-(second_h ? v2.tab[1]-v1.tab[1] : 0))/segment_h;
        Vect2 A = v1 + (v3-v1)*alpha;
        Vect2 B = second_h ? v2 + (v3-v2)*beta : v1 + (v2-v1)*beta;
        if (A.tab[0]>B.tab[0])
            swap(A, B);
        for (int j=A.tab[0]; j<=B.tab[0]; j++) {
            image.set(j, v1.tab[1]+i, color); // attention, due to int casts t0.y+i != A.y
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
            stream >> t;
            Vect2 v;
            for(int i=0;i<2;i++)
                stream >> v.tab[i];
            points.push_back(v);
        }else if(l.compare(0, 2, "f ") == 0){
            int index,t2;
            Vect3 v;
            stream >> t;
            for(int i=0;i<3;i++){
                stream >> index >> t >> t2 >> t >> t2;
                v.tab[i] = index-1;
            }
            faces.push_back(v);
        }
    }

    TGAImage image(width, height, TGAImage::RGB);
    for(vector<Vect3>::iterator it = faces.begin(); it != faces.end(); ++it) {
        Vect2 tab[3];
        for(int i=0;i<3;i++){
            Vect2 v = points[it->tab[i]];
            tab[i] = Vect2((v.tab[0]+1.0)*width/2,(v.tab[1]+1.0)*height/2);
        }
        //triangle_old(tab[0],tab[1],tab[2],image,TGAColor(rand()%255, rand()%255, rand()%255, 255));
        triangle(tab,image,TGAColor(rand()%255, rand()%255, rand()%255, 255));
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("image.tga");
    return 0;
}
