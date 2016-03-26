#include <bits/stdc++.h>
#define MAX 83
#define SIZE 9
#define TSIZE 30
#define MASK 511
#define OUTSIZE 10000000
#define INTERVAL 600000
#define DIFFICULTY 5
using namespace std;

typedef struct
{
	int key;
	int x;
	int y;
} UNDET;

long long int maxstep=0;
UNDET dots[MAX];
int book[SIZE][SIZE];
unsigned int testbit[TSIZE];
const int order[3][9] = {{9,1,8,2,7,3,6,4,5},{5,6,4,7,3,8,2,9,1},{1,9,2,8,3,7,4,6,5}};

void prtout(void)
{
	int i, j;
	int rev = rand()%4;
	for(i=0; i<SIZE; i++)
	{
		for(j=0; j<SIZE; j++)
		{
			if(rand()%10<DIFFICULTY)
			{
				if(rev==1)
					printf(" %d",book[j][i]);
				else if(rev==2)
					printf(" %d",book[SIZE-1-i][j]);
				else if(rev==3)
					printf(" %d",book[i][SIZE-1-j]);
				else
					printf(" %d",book[i][j]);
			}
			else
				printf(" ?");
		}
		printf("\n");
	}
	printf("\n\n======================\n\n");
}

int dfs(const int max_d, int depth)
{
	int i, j;
	int indexa = dots[depth].y;
	int indexb = SIZE+dots[depth].x;
	int indexc = 2*SIZE+(dots[depth].y/3)*3+(dots[depth].x/3);
	int meth;
	unsigned int tb=0, ta=0;

	if(max_d<depth)
	{
		if(maxstep%INTERVAL==0)
		{
			for(i=0; i<=max_d; i++)book[dots[i].y][dots[i].x] = dots[i].key;
			prtout();

		}
		if(maxstep++>OUTSIZE)exit(0);
		return 0;
	}
	else
	{
		//testbit[2*SIZE+(i/3)*3+(j/3)] |= (1<<book[i][j]);
		tb = testbit[indexa] | testbit[indexb] | testbit[indexc];
		meth = rand()%3;
		for(int j=0;j<SIZE;j++)
		{
			i = order[meth][j];
			if(!(tb&(1<<i)))
			{
				/*Mark*/
				testbit[indexa] |= (1<<i);
				testbit[indexb] |= (1<<i);
				testbit[indexc] |= (1<<i);
				dots[depth].key = i;
				if(dfs(max_d,depth+1))  return 1;/*Track*/
				/*UNMark*/
				testbit[indexa] &= ~(1<<i);
				testbit[indexb] &= ~(1<<i);
				testbit[indexc] &= ~(1<<i);
			}
		}
	}
	return 0;
}

int main(void)
{
	srand(time(NULL));
	freopen("testout.txt","w",stdout);
	//cin.sync_with_stdio(false);
	/*
	   if(testbit[indexa] & testbit[indexb]!=0 ||\
	   testbit[indexb] & testbit[indexc]!=0 ||\
	   testbit[indexc] & testbit[indexa]!=0)
	   return 0;
	 */
	char temp[MAX];
	int undet_count;
	int i, j;
	int indexa, indexb, indexc;
	int fail=0;
	int index;
	while((cin.getline(temp,MAX))!=NULL)
	{
		if(strcmp(temp,"end")==0)break;
		undet_count = -1;
		fail = 0;
		for(i=0; i<TSIZE; i++) testbit[i]=0;
		for(i=0; i<SIZE; i++)
		{
			for(j=0; j<SIZE; j++)
			{
				index = i*SIZE+j;
				//book[i][j] = temp[index];
				if(temp[index]=='.')
				{
					book[i][j] = -1;
					++undet_count;
					dots[undet_count].x = j;
					dots[undet_count].y = i;
					dots[undet_count].key = -1;
				}
				else
				{
					book[i][j] = temp[index]-'0';
					indexa = i;
					indexb = SIZE+j;
					indexc = 2*SIZE+(i/3)*3+(j/3);
					if(testbit[indexa]&(1<<book[i][j])||testbit[indexb]&(1<<book[i][j])||testbit[indexc] & (1<<book[i][j]))
					{
						fail = 1;
						break;
					}
					else
					{
						testbit[indexa]|=(1<<book[i][j]);
						testbit[indexb]|=(1<<book[i][j]);
						testbit[indexc] |= (1<<book[i][j]);
					}
				}
			}
		}
		if(fail||!dfs(undet_count,0))puts("No solution.");
	}
	return 0;
}
