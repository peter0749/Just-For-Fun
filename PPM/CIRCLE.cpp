#include <iostream>
#include <cassert>
#include <cstdio>
#include <cmath>
#define MAX 1000
#define PI 3.14159265358979
#define E  2.71828182845904
#define PHI 1.61803398874989
using namespace std;

typedef struct
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
} COLOR;

class Draw
{
protected:
    COLOR *pCLR;
    unsigned int depth;
    unsigned int resX,resY;
public:
    Draw(const unsigned int x, const unsigned int y, const unsigned int d, const COLOR BACKGROUND);
    ~Draw(void);
    void setCOL(const unsigned int x, const unsigned int y, COLOR c);
    COLOR readCOL(const unsigned int x, const unsigned int y);
    bool wTOPPM(const char *Filename);
    inline unsigned int getX(void)
    {
        return resX;
    }
    inline unsigned int getY(void)
    {
        return resY;
    }
};

Draw::Draw(const unsigned int x, const unsigned int y, const unsigned int d, const COLOR BACKGROUND)
{
    pCLR=NULL;
    depth = d;
    resX = x;
    resY = y;
    pCLR = new COLOR[resX*resY];
    assert(pCLR);
    for(int i=resY*resX-1; i>=0; i--)
    {
        pCLR[i] = BACKGROUND;
    }
}

Draw::~Draw(void)
{
    if(pCLR==NULL)return;
    delete [] pCLR;
    pCLR = NULL;
}

void Draw::setCOL(const unsigned int x, const unsigned int y, COLOR c)
{
    assert(pCLR);
    pCLR[x+resY*y] = c;
}

COLOR Draw::readCOL(const unsigned int x, const unsigned int y)
{
    assert(pCLR);
    return pCLR[x+y*resY];
}

bool Draw::wTOPPM(const char *Filename)
{
    FILE *FP=NULL;
    FP = fopen(Filename,"wb");
    if(FP==NULL)return false;
    fprintf(FP,"P6 %d %d %d\n",(int)resX,(int)resY,(int)depth);
    fwrite(pCLR, sizeof(COLOR), resX*resY, FP);
    fclose(FP);
    FP=NULL;
    return true;
}

int main(void)
{
    int h, w, d;
    int c_x, c_y, tx, ty, kx, ky, t;
    double scalex, scaley;
    double r, a, tsin,tcos, tcot, A;
    double ERR;
    char temp[MAX];
    COLOR col, si;
    cout<<"Height: ";
    scanf("%d",&h);
    cout<<"Width: ";
    scanf("%d",&w);
    cout<<"Depth: ";
    scanf("%d",&d);
    cout<<"Default BACKGROUND COLOR(R G B): ";
    scanf("%d%d%d",&col.R,&col.G,&col.B);
    cout<<"Circle Color: ";
    scanf("%d%d%d",&si.R,&si.G,&si.B);
    cout<<"Radius: ";
    scanf("%lf",&r);
    cout<<"X_scale Y_scale: ";
    scanf("%lf%lf",&scalex,&scaley);
    cout<<"Set f(t), t=? ";
    scanf("%d",&t);
    cout<<"ERR=? ";
    scanf("%lf",&ERR);
    cout<<"File_Path: ";
    scanf("%s",temp);

    Draw img(w,h,d,col);
    c_y = img.getY()/2;
    c_x = img.getX()/2;

    for(double k=t; k>0; k-=ERR)
    {
        tsin = sin(k);
        tcos = cos(k);
        kx = scalex*r*tcos+c_x;
        ky = scaley*r*tsin+c_y;
        if(kx>=0&&ky>=0&&kx<img.getX()&&ky<img.getY())
            img.setCOL(kx,ky,si);
    }



    return img.wTOPPM(temp);
}
