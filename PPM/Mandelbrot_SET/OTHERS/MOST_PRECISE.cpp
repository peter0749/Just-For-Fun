#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <quadmath.h>
#include <ctime>
#include <omp.h>
#define MAX 1000
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

inline int INSET(const int t, const __float128 R, const __float128 I)
{
	__float128 r=(__float128)0.0;
	__float128 i=(__float128)0.0;
	__float128 nr,ni;
	int step=0;
	while(r*r+i*i < (__float128)4.0 && step < t )
	{
		nr=r*r-i*i+R;
		ni=(__float128)2.0*r*i+I;
		r=nr;
		i=ni;
		step++;
	}
	return step;
}

int main(void)
{
	omp_set_num_threads(32);
	int h, w;
	int c_x, c_y, t;
	int est, old, term;
	float total;
	__float128 scalex, scaley, enterx, entery, speed;
	__float128 R,I;
	char temp[MAX], fpt[MAX];
	COLOR col;
	cout<<"Zoom times: "; fscanf(stdin,"%d",&term);
	cout<<"Zoom speed: "; fscanf(stdin,"%s",temp);
	speed = strtoflt128(temp,NULL);
	cout<<"Height: ";    fscanf(stdin,"%d",&h);
	cout<<"Width: ";     fscanf(stdin,"%d",&w);
	cout<<"Default BACKGROUND COLOR(R G B): ";
	fscanf(stdin,"%d%d%d",&col.R,&col.G,&col.B);
	cout<<"Scalex Scaley: ";
	fscanf(stdin,"%s%s",temp,fpt);
	scalex = strtoflt128(temp, NULL);
	scaley = strtoflt128(fpt, NULL);
	cout<<"Zooming Point(x y): ";
	fscanf(stdin,"%s%s",temp,fpt);
	enterx = strtoflt128(temp,NULL);
	entery = strtoflt128(fpt, NULL);
	printf("%lf %lf\n",enterx,entery);

	cout<<"Set f(t), t=? ";
	fscanf(stdin,"%d",&t);
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
				__float128 R = (__float128)(i-c_x);
				R = R*scalex+enterx;
				__float128 I = (__float128)(j-c_y);
				I = I*scaley+entery;
				int step=INSET(t,R,I);
				COLOR si;
				if(step==t) { si.R=si.G=si.B=0; }
				else
				{
					float brightness = ((float)step / (float)t)*255.0;
					si.R = 0; si.G = brightness; si.B = 0;
				}
				img.setCOL(i,j,si);
			}
		}
		scalex /= speed;
		scaley /= speed;
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
