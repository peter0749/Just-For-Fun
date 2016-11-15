#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <gmpxx.h>
#include <climits>
#include <omp.h>
#define MAX 10000
#define MAXCLR 1000
#define PRECISION 256
#define CLRUNIT 441.67295F
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

double palette[MAXCLR][3];

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
		if( mpf_cmp(ni,i)==0 && mpf_cmp(nr,r)==0 )
		{
			step = t; break;
		}
		mpf_set(r,nr);
		mpf_set(i,ni);
		step++;
	}
	return step;
}

inline void init_pattle(int max_t)
{
	int step=0;
	int ivl = 600/max_t;
	double unit;
	if(ivl<=0)ivl = 1;
	/*Based on rainbow*/
	for(int i=0; i<256; i+=ivl, step++)
	{
		palette[step][0] = 255; palette[step][1] = i; palette[step][2] = 0;
	}
	for(int i=0; i<256; i+=ivl, step++)
	{
		palette[step][0] = 0; palette[step][1] = 255; palette[step][2] = i;
	}
	for(int i=0; i<88; i+=ivl, step++)
	{
		palette[step][0] = i; palette[step][1] = 0; palette[step][2] = 255;
	}

	/*Normalize*/
	for(int i=0; i<step; i++)
	{
		unit = sqrt(palette[i][0]*palette[i][0] +\
				palette[i][1]*palette[i][1] +\
				palette[i][2]*palette[i][2]);
		palette[i][0] /= unit;
		palette[i][1] /= unit;
		palette[i][2] /= unit;
	}
	
}

int main(void)
{
	omp_set_num_threads(32);
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

	cout<<"Set f(t), t=? ";
	fscanf(stdin,"%d",&t);
	init_pattle(t);
	cout<<"File_Path: ";
	fscanf(stdin,"%s",temp);

	Draw img(w,h,255,col);
	c_y = img.getY()>>1;
	c_x = img.getX()>>1;
	total = (float)term/100.0;
	est=old=0;
	for(int k=0; k<term; k++)
	{
#pragma omp parallel for
		for(int i=0;i<img.getX();i++)
		{
			for( int j=0;j<img.getY();j++)
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
					int color;
					float brightness;
					if( t >= 600 ) color = step % 600;
					else color = step % t;
					brightness = (float)step/(float)t * CLRUNIT;/*May generate bad points, but whatever...*/
					si.R = palette[color][0]*brightness;
					si.G = palette[color][1]*brightness;
					si.B = palette[color][2]*brightness;
				}
				img.setCOL(i,j,si);
			}
		}
		mpf_div(scalex, scalex, speed);
		mpf_div(scaley, scaley, speed);
		est = (float)k/(float)total;
		if( est!=old && est<=100 )
		{
			cout<<est<<"%\n";
			old = est;
		}
		sprintf(fpt,"%s%d.ppm",temp,k);
		img.wTOPPM(fpt);
	}
	return 0;
}
