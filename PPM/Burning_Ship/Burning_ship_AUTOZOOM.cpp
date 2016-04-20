#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#define MAX 1000
#define PI 3.14159265358979L
#define E  2.71828182845904L
#define PHI 1.61803398874989L
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

inline int INSET(const int t, const long double R, const long double I)
{
	long double r=0;
	long double i=0;
	long double nr,ni;
	int step=0;
	//cout<<R<<I<<endl;
	//getchar();
	while(r*r+i*i < 4L && step < t )
	{
		nr=r*r-i*i+R;
		ni=abs(2L*r*i)+I;
		r=nr;
		i=ni;
		step++;
	}
	return step;
}

int main(void)
{
	int h, w;
	int c_x, c_y, i, j, k, t, brightness, term;
	int step, est, old;
	float total;
	long double scalex, scaley, enterx, entery, speed;
	long double R,I;
	char temp[MAX], fpat[MAX];
	COLOR col, si;
	srand(time(NULL));
	cout<<"Zoom times: "; scanf("%d",&term);
	cout<<"Zoom speed: "; scanf("%llf",&speed);
	cout<<"Height: ";    scanf("%d",&h);
	cout<<"Width: ";     scanf("%d",&w);
	cout<<"Default BACKGROUND COLOR(R G B): ";
	scanf("%d%d%d",&col.R,&col.G,&col.B);
	cout<<"Scalex Scaley: ";
	scanf("%llf%llf",&scalex,&scaley);
	cout<<"Zooming Point(x y): ";
	scanf("%llf%llf",&enterx,&entery);

	cout<<"Set f(t), t=? ";
	scanf("%d",&t);
	cout<<"File_Path: ";
	scanf("%s",temp);
	Draw img(w,h,255,col);
	c_y = img.getY()>>1;
	c_x = img.getX()>>1;
	si.R=si.G=si.B=0;
	total = (float)term/100.0;
	est = old = step = 0;
	for(k=0; k<term; k++)
	{
		for(i=0;i<img.getX();i++)
		{
			for(j=0;j<img.getY();j++)
			{
				R = (long double)(i-c_x);
				R = R*scalex+enterx;
				I = (long double)(j-c_y);
				I = I*scaley+entery;
				step=INSET(t,R,I);
				if(step==t) { si.R=si.G=si.B=0; }
				else
				{
					brightness = ((float)step/(float)t)*255.0;
					brightness = sqrt(brightness*255);
					//float rad = (step - log2( log(sqrt(R*R+I*I)/ log(t)) ) ) * 255.0;
					si.R = 0; si.G = brightness; si.B = 0;
					//si.R =0; si.G = rad; si.B = 0;
				}
				img.setCOL(i,j,si);
			}
		}
		sprintf(fpat,"%s%d.ppm",temp,k);
		img.wTOPPM(fpat);
		est = (float)k/(float)total;
		if(est<=100 && est!=old)
		{
			cout<<est<<"%\n";
			old = est;
		}
		scalex /= speed;
		scaley /= speed;
	}
	return 0;
}

