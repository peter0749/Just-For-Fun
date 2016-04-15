#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#define HMAX 43
#define WMAX 150

const int dire[8][2]={{0,-1},{0,1},{1,-1},{1,0},{1,1},{-1,-1},{-1,0},{-1,1}};

void prtout(bool map[HMAX][WMAX])
{
	int i,j;
	for(i=0;i<100;i++)printf("\n");
	for(i=0;i<HMAX;i++)
	{
		for(j=0;j<WMAX;j++)
			printf(map[i][j]?"#":".");
		printf("\n");
	}
}

int live(const int x, const int y, bool map[HMAX][WMAX])
{
	int count=0;
	int tx,ty;
	int i;
	for(i=0;i<8;i++)
	{
		tx=x+dire[i][0];
		ty=y+dire[i][1];
		if(tx>=0&&tx<WMAX&&ty>=0&&ty<HMAX&&map[ty][tx])count++;
	}
	return count;
}

void grow(const int x, const int y, bool map1[HMAX][WMAX], bool map2[HMAX][WMAX])
{
	int n=live(x,y,map1);
	if(map1[y][x])
	{
		if(n<=1||n>=4)map2[y][x]=false;
		else map2[y][x]=true;
	}
	else
	{
		if(n==3)map2[y][x]=true;
		else map2[y][x]=false;
	}
}

int main(void)
{
	int n, i, j, k;
	int tx,ty,t;
	FILE *fptr;
	char temp[WMAX+2];
	srand(time(NULL));
	bool map[2][HMAX][WMAX];
	for(i=0; i<HMAX; i++)for(j=0;j<WMAX;j++)for(k=0;k<2;k++)
		map[k][i][j]=false;
	for(i=0;i<WMAX;i++)temp[i]=0;
	scanf("%d%d",&n,&t);
	if(n<=0)
	{
		fptr = fopen("lifegame.txt","r");
		if(fptr==NULL)return -1;
		else
		{
			for(i=0;i<HMAX;i++)
			{
				fscanf(fptr,"%s",temp);
				for(j=0;j<WMAX;j++)
				{
					if(temp[j]=='#')map[0][i][j]=true;
					else map[0][i][j]=false;
				}
			}
		}
		fclose(fptr);
	}
	else
	{
		if(n>HMAX*WMAX)n=HMAX*WMAX;

		for(i=0; i<n; i++)
		{
			while(1)
			{
				tx=rand()%WMAX;
				ty=rand()%HMAX;
				if(!map[0][ty][tx])
				{
					map[0][ty][tx]=true;
					break;
				}
			}
		}
	}
	prtout(map[0]);
	for(k=0; k<t; k++)
	{
		usleep(100000);
		//getchar();
		for(i=0;i<HMAX;i++)for(j=0;j<WMAX;j++)
		{
			grow(j,i,map[k%2],map[(k+1)%2]);
		}
		prtout(map[(k+1)%2]);
	}

	return 0;
}
