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
}COLOR;

class Draw
{
protected:
    COLOR *pCLR;
    unsigned int depth;
    unsigned int resX,resY;
public:
    Draw(const unsigned int x, const unsigned int y, const unsigned int d, const COLOR BACKGROUND);
    ~Draw(void);
    inline void setCOL(const unsigned int x, const unsigned int y, COLOR c);
    inline COLOR readCOL(const unsigned int x, const unsigned int y);
    bool wTOPPM(const char *Filename);
    inline unsigned int getX(void){return resX;}
    inline unsigned int getY(void){return resY;}
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

inline void Draw::setCOL(const unsigned int x, const unsigned int y, COLOR c)
{
    assert(pCLR);
    pCLR[x+resX*y] = c;
}

inline COLOR Draw::readCOL(const unsigned int x, const unsigned int y)
{
    assert(pCLR);
    return pCLR[x+y*resX];
}

bool Draw::wTOPPM(const char *Filename)
{
    FILE *FP=NULL;
    FP = fopen(Filename,"wb");
    if(FP==NULL)return false;
    fprintf(FP,"P6 %d %d %d\n",(int)resX,(int)resY,(int)depth);
    fwrite(pCLR, sizeof(COLOR), resX*resY, FP);
    fclose(FP);FP=NULL;
    return true;
}

inline bool DRAWPOINT(const int r, const int x, const int y, const int tarx, const int tary, Draw &IMG, const COLOR p)
{
    int tx ,ty;
    double dist,tr;
    tr = r*r;
    tx = tarx-x;
    ty = tary-y;
    dist = tx*tx+ty*ty;
    if(dist<tr && tarx>=0 && tary>=0 && tarx<IMG.getX() && tary<IMG.getY()){IMG.setCOL(tarx,tary,p);return true;}
    return false;
}

inline double guass(const double x, const double sig, const double mu)
{
    double t=x-mu;
    double dsi=sig*sig;
    return (1/(sig*sqrt(2*PI)))*pow(E,( -t*t ) / ( 2*dsi ));
}

int main(void)
{
    int h, w, d;
    int c_x, c_y, tx, ty, kx, ky, den, i, j, cr;
    double scalex, scaley;
    double r, a, tsin,tcos, tcot, A, t, k, sigma, u;
    double ERR=0.00001;
    double m;
    char temp[MAX];
    COLOR col, si;
    col.R=col.G=col.B=0;
    cin>>h>>w;
    Draw img(w,h,255,col);
    //col.G=255;
    si.R=si.G=si.B=0;
    si.G=255;
    sigma=w/8;
    u = w/2;
    m= 1 / (sigma*sqrt(2*PI));
    cout<<m<<endl;

    for(i=0;i<img.getX();i++)
    {
        ty = img.getY()-(guass(i,sigma,u)/m)*img.getY();
        if(ty>=0 && i>=0 && i<img.getX() && ty < img.getY())
            img.setCOL(i,ty,si);
    }

    return img.wTOPPM("GUASS.ppm");
}
