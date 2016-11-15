#include <sys/time.h>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <climits>
#include <omp.h>
#include <complex>
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
const char ite[11][8] = {"1e-58","1e-127","1e-192","1e-290","1e-384","1e-512","1e-590","1e-666","1e-720","1e-790","1e-850"};
const int iteval[11] = {310,610,900,1350,1800,2300,3000,3500,4000,4600,5500};

inline int INSET(const int t, const double &R, const double &I, const double &powerz)
{
	int step = 0;
	complex<double>z(0.0L,0.0L), nz(0.0L,0.0L);
	const complex<double> cz(R,I);
	const double c = 2.0;

	while( abs(z) < c  && step < t )
	{
		nz = pow(z,powerz) + cz;
		
		if( nz==z )
		{
			step = t; break;
		}
		z = nz;
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

inline int ch_iterate( const double &scalex, const double &scaley )
{
	int i=0,j=10;
	int m;
	double temp=0.0;

	while( i<j )
	{
		m = (i+j)>>1;
		temp = atof(ite[m]);
		if( temp<=scalex  || temp<=scaley ) j = m;
		else i = m+1;
	}
	return iteval[i];
}

int main(void)
{
	struct timeval start_t, end_t;
	unsigned long diff;
	omp_set_num_threads(32);
	int h, w;
	int c_x, c_y, t;
	int est, old, term;
	float total;
	double scalex, scaley, enterx, entery, speed;
	double ch_speed, init_power;
	char temp[MAX], fpt[MAX];
	COLOR col;
	cout<<"Zoom times: "; fscanf(stdin,"%d",&term);
	cout<<"Zoom speed: "; fscanf(stdin,"%lf",&speed);
	cout<<"Initial power: ";
	fscanf(stdin,"%lf",&init_power);
	cout<<"Transform Speed :";
	fscanf(stdin,"%lf",&ch_speed);
	cout<<"Height: ";    fscanf(stdin,"%d",&h);
	cout<<"Width: ";     fscanf(stdin,"%d",&w);
	cout<<"Default BACKGROUND COLOR(R G B): ";
	fscanf(stdin,"%d%d%d",&col.R,&col.G,&col.B);
	cout<<"Scalex Scaley: ";
	fscanf(stdin,"%lf%lf",&scalex,&scaley);
	cout<<"Zooming Point(x y): ";
	fscanf(stdin,"%lf%lf",&enterx,&entery);

	cout<<"File_Path: ";
	fscanf(stdin,"%s",temp);

	Draw img(w,h,255,col);
	c_y = img.getY()>>1;
	c_x = img.getX()>>1;
	total = (float)term/100.0;
	est=old=0;
	t = 101;
	init_pattle(t);
	gettimeofday(&start_t,NULL);
	for(int k=0; k<term; k++)
	{
		//t = ch_iterate(scalex, scaley);
		//init_pattle(t);
#pragma omp parallel for
		for(int i=0;i<img.getX();i++)
		{
			for( int j=0;j<img.getY();j++)
			{
				double R, I;
				R = (double)i-c_x;
				I = (double)j-c_y;
				R = R * scalex + enterx ;
				I = I * scaley + entery ;

				int step=INSET(t,R,I,init_power);
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
		scaley /= speed;
		scalex /= speed;
		init_power += ch_speed;
		est = (float)k/(float)total;
		if( est!=old && est<=100 )
		{
			cout<<est<<"%\n";
			old = est;
		}
		sprintf(fpt,"%s%d.ppm",temp,k);
		img.wTOPPM(fpt);
	}
	gettimeofday(&end_t, NULL);
	diff = 1000000 * (end_t.tv_sec - start_t.tv_sec) + (end_t.tv_usec - start_t.tv_usec);
	printf("The diff: %ld\n",diff);
	return 0;
}
