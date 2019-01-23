#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <limits>
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

Vect3f barycentric(Vect3f* v,Vect3f V){
    Vect3f b = cross(Vect3f(v[2].x-v[0].x, v[1].x-v[0].x, v[0].x-V.x), Vect3f(v[2].y-v[0].y, v[1].y-v[0].y, v[0].y-V.y));
    if (abs(b.z)<1)
        return Vect3f(-1,1,1);
    return Vect3f(1.f-(b.x+b.y)/b.z, b.y/b.z, b.x/b.z);
}

void triangle(Vect3f* v,float* zbuffer,TGAImage &image, TGAColor color){
    Vect2f boxmin(image.get_width()-1,  image.get_height()-1);
    Vect2f boxmax(0, 0);
    Vect2f tmp = boxmin;
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            boxmin.set(j, max(0.f, min(boxmin.get(j), v[i].get(j))));
            boxmax.set(j, min(tmp.get(j), max(boxmax.get(j), v[i].get(j))));
        }
    }

    Vect3f V;
    for (V.x = boxmin.x; V.x<=boxmax.x; V.x++) {
        for (V.y = boxmin.y; V.y<=boxmax.y; V.y++) {
            Vect3f bc_screen  = barycentric(v, V);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0)
                continue;
            V.z = 0;
            for(int i=0;i<3;i++)
                V.z+=v[i].z*bc_screen.get(i);
            if (zbuffer[int(V.x+V.y*width)]<V.z) {
                zbuffer[int(V.x+V.y*width)] = V.z;
                image.set(V.x, V.y, color);
            }
        }
    }
}

void triangle_old(Vect2i v1,Vect2i v2,Vect2i v3,TGAImage &image, TGAColor color){
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
        Vect2i A = v1 + (v3-v1)*alpha;
        Vect2i B = second_h ? v2 + (v3-v2)*beta : v1 + (v2-v1)*beta;
        if (A.x>B.x)
            swap(A, B);
        for (int j=A.x; j<=B.x; j++) {
            image.set(j, v1.y+i, color);
        }
    }
}

int main(int argc, char** argv) {
    TGAImage textureTGA;
    TGA_Header headerTGA;
    ifstream file;
    if(argc==2)
        file.open (argv[1], ifstream::in);
    else{
        file.open ("obj/african_head/african_head.obj", ifstream::in);
        textureTGA.read_tga_file("obj/african_head/african_head_diffuse.tga",headerTGA);
    }

    if (file.fail()) {
        cout << "Erreur lors de l'ouverture du fichier obj" << endl;
        return 0 ;
    }

    vector<Vect3f> points, faces, facesT;
    vector<Vect2f> texture;
    string l;
    while(!file.eof()){
        getline(file, l);
        istringstream stream(l.c_str());
        char trash;
        float f;
        Vect3f v, v2;
        Vect2f v3;
        if (l.compare(0, 2, "v ") == 0) {
            stream >> trash;
            for(int i=0;i<3;i++){
                stream >> f;
                v.set(i,f);
            }
            points.push_back(v);
        }else if(l.compare(0, 3, "vt ") == 0){
            stream >> trash;
            stream >> trash;
            for(int i=0;i<2;i++){
                stream >> f;
                v3.set(i,f);
            }
            texture.push_back(v3);
        }else if(l.compare(0, 2, "f ") == 0){
            int index, index2, trash2;
            stream >> trash;
            for(int i=0;i<3;i++){
                stream >> index >> trash >> index2 >> trash >> trash2;
                v.set(i,index-1);
                v2.set(i,index2-1);
            }
            faces.push_back(v);
            facesT.push_back(v2);
        }
    }

    float *zbuffer = new float[width*height];
    for(int i=0;i<width*height;i++)
        zbuffer[i] = -numeric_limits<float>::max();

    TGAImage image(width, height, TGAImage::RGB);
    for(vector<Vect3f>::size_type i = 0; i < faces.size(); i++) {
        Vect3f tab[3], tab2[3];
        for(int j=0;j<3;j++){
            Vect3f v = points[faces[i].get(j)];
            Vect2f pointTexture = texture[faces[i].get(j)];
            cout << pointTexture.x << " " << pointTexture.y << endl;
            tab[j] = Vect3f(int((v.x+1.)*width/2.+.5),int((v.y+1.)*height/2.+.5),v.z);
            tab2[j] = v;
        }
        Vect3f n = cross((tab2[2]-tab2[0]),(tab2[1]-tab2[0]));
        n.normalize();
        n = n * Vect3f(0,0,-1);
        float intensity = n.z;
        if(intensity>0){
            triangle(tab,zbuffer,image,TGAColor(intensity*255, intensity*255, intensity*255, 255));
        }
    }

    image.flip_vertically(); // origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete zbuffer;
    zbuffer = 0;
    return 0;
}
