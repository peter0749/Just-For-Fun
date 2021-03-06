#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
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
	fwrite(pCLR, 1, 3*resX*resY, FP);
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

inline double dist(const double a, const double b)
{
	return a*a+b*b;
}

inline int INSET(const int t, const double R, const double I)
{
	double r=0;
	double i=0;
	double nr,ni;
	int step=0;
	//cout<<R<<I<<endl;
	//getchar();
	while(r*r+i*i < 4 && step < t )
	{
		nr=r*r-i*i+R;
		ni=2*r*i+I;
		r=nr;
		i=ni;
		step++;
	}
	return step;
}

int main(void)
{
	int h, w, d;
	int c_x, c_y, tx, ty, kx, ky, den, i, j, cr, t;
	int step;
	double scalex, scaley, enterx, entery;
	double R,I,nR,nI,tR,tI;
	double r, a, tsin,tcos, tcot, A, k, brightness;
	double ERR, chcl;
	char temp[MAX];
	COLOR col, si;
	srand(time(NULL));
	cout<<"Height: ";    scanf("%d",&h);
	cout<<"Width: ";     scanf("%d",&w);
	cout<<"Default BACKGROUND COLOR(R G B): ";
	scanf("%d%d%d",&col.R,&col.G,&col.B);
	cout<<"Scalex Scaley: ";
	scanf("%lf%lf",&scalex,&scaley);
	cout<<"Zooming Point(x y): ";
	scanf("%lf%lf",&enterx,&entery);

	cout<<"Set f(t), t=? ";
	scanf("%d",&t);
	cout<<"File_Path: ";
	scanf("%s",temp);

	Draw img(w,h,255,col);
	c_y = img.getY()>>1;
	c_x = img.getX()>>1;
	si.R=si.G=si.B=0;
	step=0;
	for(i=0;i<img.getX();i++)for(j=0;j<img.getY();j++)
	{
		R = (i-c_x)*scalex+enterx;
		I = (j-c_y)*scaley+entery;
		step=INSET(t,R,I);
		if(step==t) { si.R=si.G=si.B=0; }
		else
		{
			brightness = step%256;
			si.R = brightness; si.G = sqrt(brightness*255); si.B = 255;
		}
		img.setCOL(i,j,si);
	}

	return img.wTOPPM(temp);
}
