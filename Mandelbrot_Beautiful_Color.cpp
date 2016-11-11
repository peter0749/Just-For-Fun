#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <gmpxx.h>
#include <climits>
#include <omp.h>
#define MAX 10000
#define PALETTE_SIZE 1024
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

double palette[PALETTE_SIZE][3];
const char ite[ITE_SIZE][10] = {"1e-8\0","1e-9\0","1e-29\0","1e-46\0","1e-55\0","1e-60\0","1e-64\0","1e-80\0","1e-82\0","1e-85\0","1e-92\0","1e-95\0","1e-98\0","1e-101\0"};
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
inline void slow_YIQ2RGB( double mtx[3] ){
       double temp[3];
       memset(temp,0x00,sizeof(temp));
       for(int k=0; k<3; ++k){
               for(int i=0; i<3; ++i){
                      temp[i] += rYIQ[i][k] *  mtx[k];
               }
       }
       memcpy(mtx, temp, std::min(sizeof(mtx),sizeof(temp)));
}

void InitPalette(void){
       double oriI=0.0f, oriQ=0.5226f, oriY=0.0f;
       const double stepQ = 0.5226f*2.0f/(double)PALETTE_SIZE;
       const double stepI = 0.5957f*2.0f/(double)PALETTE_SIZE;
       const double stepY = 1.0f/(double)PALETTE_SIZE;

       /*Use YIQ, Q Channel.*/
       for(int i=0; i<PALETTE_SIZE; ++i){
               palette[i][0] = pow(i*stepY,0.3f);//Y
               palette[i][1] = stepI*(double)i;//I
               palette[i][2] = oriQ-(stepQ*(double)i);//Q
       }
       /*Convert YIQ to RGB*/
       for(int i=0; i<PALETTE_SIZE; ++i){
               slow_YIQ2RGB(palette[i]);
       }
}

inline int INSET(const int t, const mpf_t &R, const mpf_t &I)
{
	int step = 0;
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

	while( mpf_cmp(temp,c1) <=0  && step < t )
	{
		mpf_mul(nr, r, r);
		mpf_mul(temp, i, i);
		mpf_sub(nr, nr, temp);
		mpf_add(nr, nr, R);
		mpf_mul(temp, r, i);
		mpf_mul(temp, temp, c2 );
		mpf_add(ni, temp, I);
		if( mpf_cmp(ni,i)==0 && mpf_cmp(nr,r)==0 ){step = t; break;}
		mpf_set(r,nr);
		mpf_set(i,ni);
		step++;
	}
	mpf_clear(r);
	mpf_clear(i);
	mpf_clear(nr);
	mpf_clear(ni);
	mpf_clear(c1);
	mpf_clear(c2);
	mpf_clear(temp);

	return step;
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

int main(void)
{
	omp_set_num_threads(7);
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
	cout<<"Height: ";    fscanf(stdin,"%d",&h);
	cout<<"Width: ";     fscanf(stdin,"%d",&w);
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
	c_y = img.getY()>>1;
	c_x = img.getX()>>1;
	total = (float)term/100.0f;
	est=old=0;
	InitPalette();
	for(int k=0; k<term; k++)
	{
		t = ch_iterate(scalex, scaley);
#pragma omp parallel for
		for(int i=0;i<img.getX();i++)
		{
			for( int j=img.getY();j>=0;--j)
			{
				mpf_t R, I;
				mpf_init_set_si(R, i-c_x);
				mpf_mul(R, R, scalex);
				mpf_add(R, R, enterx);
				mpf_init_set_si(I, j-c_y);
				mpf_mul(I, I, scaley);
				mpf_add(I, I, entery);
				int step=INSET(t,R,I);
				COLOR si;
				if(step==t) { si.R=si.G=si.B=0; }
				else
				{
					int index = floor((double)step/(double)t*PALETTE_SIZE-0.5f);
					si.R = floor(palette[index][0]*255.0f-0.5f);
					si.G = floor(palette[index][1]*255.0f-0.5f);
					si.B = floor(palette[index][2]*255.0f-0.5f);
				}
				img.setCOL(i,j,si);
				mpf_clear(R);
				mpf_clear(I);
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