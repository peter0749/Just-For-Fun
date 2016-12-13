#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <gmpxx.h>
#include <climits>
#include <omp.h>
#define MAX 10000
#define MAX_H 2560
#define MAX_W 2560
#define PALETTE_SIZE 256
#define PRECISION 200
#define CLRUNIT 441.67295F
#define ITE_SIZE 14
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
    fwrite(pCLR, 1, 3*resX*resY, FP);
    fclose(FP);FP=NULL;
    return true;
}

unsigned int hist[PALETTE_SIZE+2];
int index_map[MAX_W+20][MAX_H+2];//Col. major
double gam=5.0000f;
double palette[PALETTE_SIZE+2][3];
const char ite[ITE_SIZE+2][10] = {"1e-8\0","1e-9\0","1e-29\0","1e-46\0","1e-55\0","1e-60\0","1e-64\0","1e-80\0","1e-82\0","1e-85\0","1e-92\0","1e-95\0","1e-98\0","1e-101\0"};
const int iteval[ITE_SIZE] = {200,4158,5141,5920,7710,9980,12000,14300,16000,18000,20000,40000,63000,80000};
const double YIQ[3][3]= {
    {0.299,0.587,0.114},
    {0.596,-0.274,-0.322},
    {0.211,-0.523,0.312}
};
const double rYIQ[3][3]= {
    {1.0f, 0.956, 0.621},
    {1.0f, -0.272, -0.647},
    {1.0f, -1.106, 1.703}
};
inline void slow_YIQ2RGB( double target[3], double mtx[3] ){
    for(int i=0; i<3; ++i) target[i]=(double)0.0f;
    for(int k=0; k<3; ++k){
        for(int i=0; i<3; ++i){
            target[i] += rYIQ[i][k] *  mtx[k];
        }
    }
    for(int i=0; i<3; ++i) if(target[i]>0.999999f) target[i]=0.999997f;
}

void InitPalette(void){
    double oriI=-0.5957f, oriQ=0.5226f;
    const double stepQ = 0.5226f*0.6f/(double)PALETTE_SIZE;
    const double stepI = 0.5957f*2.0f/(double)PALETTE_SIZE;

    /*Use YIQ, Q Channel.*/
    for(int i=0; i<PALETTE_SIZE; ++i){
        //palette[i][0] = 1.0f;//Undetermined
        palette[i][1] = oriI+(double)(stepI*(double)i);//I
        palette[i][2] = oriQ-(double)(stepQ*(double)i);//Q
        if(palette[i][1]>0.999999f) palette[i][1]=0.999997f;
        if(palette[i][2]>0.999999f) palette[i][2]=0.999997f;
    }
}

void det_luminance(void){
    for(int i=1; i<PALETTE_SIZE; ++i) hist[i]+=hist[i-1];
#pragma omp parallel for
    for(int i=0; i<PALETTE_SIZE; ++i){
        palette[i][0] = pow((double)hist[i] / (double)hist[PALETTE_SIZE-1], gam);//Determine Luminance
        if(palette[i][0]>0.999999f) palette[i][0]=0.999997f;
    }
}

inline int INSET(const int t, const mpf_t &R, const mpf_t &I)
{
    int res;
    mpf_t r, i, nr, ni;
    mpf_t c1, c2;
    mpf_t temp;

    mpf_init_set_str(r, "0", 10);
    mpf_init_set_str(i, "0", 10);
    mpf_init_set_str(nr, "0", 10);
    mpf_init_set_str(ni, "0", 10);
    mpf_init_set_str(c1, "4", 10);
    mpf_init_set_str(c2, "2", 10);
    mpf_init_set_str(temp, "0", 10);

    res=0;
    for(int step=0 ;step <= t; ++step){
        mpf_mul(nr, r, r);
        mpf_mul(ni, i, i);
        mpf_add(temp, nr, ni);
        if(mpf_cmp(temp,c1) > 0){
            res = step;
            break;
        }
        mpf_sub(nr, nr, ni);
        mpf_add(nr, nr, R);
        mpf_mul(ni, r, i);
        mpf_add(ni, ni, ni);
        mpf_add(ni, ni, I);
        //if( mpf_cmp(ni,i)==0 && mpf_cmp(nr,r)==0 ) break;
        mpf_set(r,nr);
        mpf_set(i,ni);
    }
    mpf_clear(r);
    mpf_clear(i);
    mpf_clear(nr);
    mpf_clear(ni);
    mpf_clear(c1);
    mpf_clear(c2);
    mpf_clear(temp);

    return res;
}

inline int ch_iterate( const mpf_t &scalex, const mpf_t &scaley )
{
    int i=0,j=ITE_SIZE-1;
    int m;
    mpf_t temp;
    mpf_init(temp);

    while( i<j )
    {
        m = (i+j)>>1;
        mpf_set_str(temp, ite[m], 10);
        if( mpf_cmp(temp, scalex) <= 0 || mpf_cmp(temp, scaley) <= 0  ) j = m;
        else i = m+1;
    }
    mpf_clear(temp);
    return iteval[i];
}

void ComputePix(int w, int h, int cx, int cy, const mpf_t &scalex, const mpf_t &scaley, const mpf_t &enterx, const mpf_t &entery, int ite ){
#pragma omp parallel for
        for(int i=0;i<w;i++)
        {
            for( int j=0;j<h;++j)
            {
                mpf_t R, I;
                mpf_init_set_si(R, i-cx);
                mpf_mul(R, R, scalex);
                mpf_add(R, R, enterx);
                mpf_init_set_si(I, j-cy);
                mpf_mul(I, I, scaley);
                mpf_add(I, I, entery);
                index_map[i][h-j-1]=INSET(ite,R,I);
                //index_map[i][j]=0;
                mpf_clear(R);
                mpf_clear(I);
                //printf("%d %d\n",i , j);
            }
        }
}

int main(void)
{
    //omp_set_num_threads(7);
    mpf_set_default_prec(PRECISION);
    int h, w;
    int c_x, c_y, t;
    int est, old, term;
    float total;
    mpf_t scalex, scaley, enterx, entery, speed;
    char temp[MAX], fpt[MAX];
    COLOR col;
    cout<<"Zoom times: "; fscanf(stdin,"%d",&term);
    cout<<"Zoom speed: "; fscanf(stdin,"%s",temp);
    mpf_init_set_str(speed,temp, 10);
    cout<<"Width: ";     fscanf(stdin,"%d",&w);
    cout<<"Height: ";    fscanf(stdin,"%d",&h);
    if(h>MAX_H) h = MAX_H;
    if(w>MAX_W) w = MAX_W;
    cout<<"Default BACKGROUND COLOR(R G B): ";
    fscanf(stdin,"%d%d%d",&col.R,&col.G,&col.B);
    cout<<"Scalex Scaley: ";
    fscanf(stdin,"%s%s",temp,fpt);
    mpf_init_set_str(scalex, temp, 10);
    mpf_init_set_str(scaley, fpt, 10);
    cout<<"Zooming Point(x y): ";
    fscanf(stdin,"%s%s",temp,fpt);
    mpf_init_set_str(enterx, temp, 10);
    mpf_init_set_str(entery, fpt, 10);

    cout<<"File_Path: ";
    fscanf(stdin,"%s",temp);

    Draw img(w,h,255,col);
    printf("# %d %d\n", img.getX(), img.getY());
    c_y = img.getY()>>1;
    c_x = img.getX()>>1;
    total = (float)term/100.0f;
    est=old=0;
    InitPalette();
    for(int k=0; k<term; k++)
    {
        t = ch_iterate(scalex, scaley);
        memset(index_map,0x00,sizeof(index_map));
        ComputePix(w, h, c_x, c_y, scalex, scaley, enterx, entery, t );

        double Q = (double)t / (double)PALETTE_SIZE;
        memset(hist,0x00,sizeof(hist));
        for(int i=0; i<w; ++i)for(int j=0; j<h; ++j) ++hist[ (unsigned)((double)index_map[i][j] / Q) ];
        det_luminance();
        for(int i=0;i<w;i++)
        {
            for( int j=0; j<h; ++j)
            {
                COLOR si;
                double COLMTX[3];
                if(!index_map[i][j]){si.R=si.G=si.B=0;}
                else{
                    slow_YIQ2RGB(COLMTX, palette[ (unsigned)((double)index_map[i][j] / Q) ]);
                    si.R = (unsigned char)floor(COLMTX[0] * 254.99f-0.5f);
                    si.G = (unsigned char)floor(COLMTX[1] * 254.99f-0.5f);
                    si.B = (unsigned char)floor(COLMTX[2] * 254.99f-0.5f);
                }
                img.setCOL(i,j,si);
            }
        }
        mpf_div(scalex, scalex, speed);
        mpf_div(scaley, scaley, speed);
        est = (float)k/(float)total;
        if( est!=old && est<=100 )
        {
            //cout<<est<<"%\n";
            fprintf(stderr,"%d\%\n",est);
            gmp_fprintf(stderr,"scalex: %Ff\nscaley:%Ff\n",scalex,scaley);
            old = est;
        }
        sprintf(fpt,"%s%d.ppm",temp,k);
        img.wTOPPM(fpt);
    }
    mpf_clear(scalex);
    mpf_clear(scaley);
    mpf_clear(enterx);
    mpf_clear(entery);
    mpf_clear(speed);
    return 0;
}
