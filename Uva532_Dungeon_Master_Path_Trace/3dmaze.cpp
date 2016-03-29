#include <iostream>
#include <iomanip>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#define MAX 150
#define NDEBUG

using namespace std;

typedef struct node
{
    char x;
    char y;
    char z;
    unsigned short step;
	struct node *pa;
    struct node *next;
}CORD;
CORD *head = NULL, *tail = NULL;
CORD *origin = NULL;
char book[MAX][MAX][MAX] = {{{0}}};
char COPY[MAX][MAX][MAX] = {{{0}}};
int stepc[MAX][MAX][MAX] = {{{0}}};

const char dire[6][3] = {{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};

inline void prtout(int z, int y, int x)
{
    int i, j, k;
    for(i=1;i<=z;i++)
    {
        for(j=1;j<=y;j++)
        {
            for(k=1;k<=x;k++)
            {
				if(stepc[i][j][k]!=0)
					cout<<setw(4)<<stepc[i][j][k];
				else
                	cout<<setw(4)<<COPY[i][j][k];
            }
            cout<<endl;
        }
        cout<<endl;
    }
}

inline int push(CORD *pa, int z, int y, int x, int step)
{
    CORD *ptr=NULL;
    if(book[z][y][x]=='#')  return 0;
    ptr = new CORD;
    ptr->x = x;
    ptr->y = y;
    ptr->z = z;
    ptr->step = step;
    ptr->next = NULL;
	ptr->pa = pa;
    tail->next = ptr;
    tail = ptr;
	if(book[z][y][x]=='E')
	{
    	book[z][y][x] = '#';
		return step;
	}
    book[z][y][x] = '#';
    return 0;
}

inline CORD *pop(void)
{
    CORD *ptr;
    if(head->next==NULL)
    {
        return NULL;//Queue is empty;
    }
    ptr = head->next;
    head = head->next;
    return ptr;
}

inline int bfs(int sz, int sy, int sx)
{
    int i;
    int solution = 0;
    CORD *test=NULL;
    if(solution = push(NULL,sz,sy,sx,0)) return solution;/*push head*/
    while(1)
    {
        test = pop();/*pop current head*/
        if(test==NULL)break;    /*queue empty!*/

        for(i=0;i<6;i++)
        {
            if(solution = (push(test,test->z+dire[i][0],test->y+dire[i][1],test->x+dire[i][2],test->step+1)))
            {
                test = NULL;
				test = tail;
				while(test->pa!=NULL)
				{
					stepc[test->z][test->y][test->x] = test->step;
#ifdef DEBUG
					cout<<"Debug:"<<'\t'<<(int)test->z<<'\t'<<(int)test->y<<'\t'<<(int)test->x<<'\t'<<(unsigned short)test->step<<endl;
#endif
					test = test->pa;
				}
                return solution;
            }
        }
        test=NULL;
    }
    return 0;
}

void dstry(CORD *ptr)
{
    if(ptr->next!=NULL) dstry(ptr->next);
    delete ptr;
    //free(ptr);
}

int main(void)
{
    int z, y, x;
    int i, j, k;
    int inx, iny, inz;
    int step;
    char temp[MAX];
    for(i=0;i<MAX;i++)for(j=0;j<MAX;j++)for(k=0;k<MAX;k++){COPY[i][j][k] = book[i][j][k] = '#';stepc[i][j][k]=0;}
    while(scanf("%d%d%d",&z,&y,&x)==3)
    {
        if(z==0&&x==0&&y==0)    return 0;
        origin = new CORD;
        origin->next = NULL;
		origin->pa = NULL;
        //head->x = head->y = head->z = 0;
        tail = head = origin;
        for(i=0;i<z;i++)
        {
            for(j=0;j<y;j++)
            {
                scanf("%s",temp);
                for(k=0;k<x;k++)
                {
                    COPY[i+1][j+1][k+1] = book[i+1][j+1][k+1] = temp[k];
                    if(temp[k]=='S')
                    {
                        inz = i+1;
                        iny = j+1;
                        inx = k+1;
                    }
                }
            }
        }
        //prtout(z,y,x);
        step = bfs(inz,iny,inx);
        if(step) cout<<"Escaped in "<<step<<" minute(s).\n";
        else     cout<<"Trapped!\n";
        prtout(z,y,x);
        head = tail = NULL;
        dstry(origin);
        //delete head;
        origin = NULL;
    }
    return 0;
}

