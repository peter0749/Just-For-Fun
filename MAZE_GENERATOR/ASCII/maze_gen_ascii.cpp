#include <bits/stdc++.h>
#define MAX max_size
#define POINTS max_p
#define ARRSIZE 1000
#define DEBUG
using namespace std;

typedef struct
{
    int x, y;
} CORD;

CORD arr[ARRSIZE*ARRSIZE];
int p_count=0;
char maze[ARRSIZE][ARRSIZE] = {0};
int checkrow[ARRSIZE]= {0};
int checkcol[ARRSIZE]= {0};
const char dirindex[4][2]= {{1,0},{0,1},{-1,0},{0,-1}};

int rdcmp(const void *a, const void *b)
{
    return rand()%3-1;
}

int main(void)
{
#ifdef DEBUG
    freopen("ASCII_MAZEOUT.txt","w",stdout);
#endif
    int dire = 0;
    int max_p;
    int max_size;
    int n_x, n_y;
    srand(time(NULL));
    while(1)
    {
        cout<<"Maze size\n(Enter an odd integer, or enter an integer less than 1 to halt the program):\n";
        cin>>max_size;
        if(max_size<1)break;
        else if(max_size%2==0)//Is even
            max_size++;
        cout<<"Selected MAZESIZE: "<<max_size<<'\n';

        cout<<"Difficulty\n(Note: Set too high may led to generate unsolvable maze";
        p_count=0;
        for(int i=0; i<MAX; i++)checkrow[i] = checkcol[i] = 0;
        for(int i=0; i<MAX; i++)
        {
            //arr[i].x = arr[i].y = 0;
            //checkrow[i] = checkcol[i] = 0;
            for(int j=0; j<MAX; j++)
            {
                if(i==0||i==MAX-1)maze[i][j]=1;
                else if(j==0)maze[i][j]=1;
                else if(j==MAX-1)maze[i][j]=1;
                else if(i%2==0&&j%2==0)
                {
                    maze[i][j]=1;
                    arr[p_count].x = j;
                    arr[p_count].y = i;
                    p_count++;
                }
                else
                {
                    checkrow[i]++;
                    checkcol[j]++;
                    maze[i][j]=0;
                }
            }
        }

        cout<<", suggestion: about "<<p_count<<"):\n";

        cin>>max_p;
        cout<<"Selected Difficulty: "<<max_p<<'\n';
#ifdef DEBUG
        cout<<"Before (random)Sort: \n";
        for(int i=0; i<p_count; i++)
            cout<<"x: "<<arr[i].x<<"\ty: "<<arr[i].y<<'\n';
#endif // DEBUG
        qsort(arr,p_count,sizeof(CORD),rdcmp);
#ifdef DEBUG
        for(int i=0; i<MAX; i++)
        {
            for(int j=0; j<MAX; j++)
                cout<<setw(2)<<(int)maze[i][j];
            cout<<'\n';
        }
        cout<<"After (random)Sort: \n";
        for(int i=0; i<p_count; i++)
            cout<<"x: "<<arr[i].x<<"\ty: "<<arr[i].y<<'\n';
#endif
        for(int i=0; i<POINTS; i++)
        {
            dire=rand()%4;
            n_y = arr[i].y+dirindex[dire][0];
            n_x = arr[i].x+dirindex[dire][1];
            if(n_x!=1 && n_y!=1 && n_y!=MAX-2 && n_x!=MAX-2 && maze[n_y][n_x]==0 && (checkcol[n_y]<2 || checkrow[n_x]<2))
            {
                i--;
                continue;
            }
            maze[n_y][n_x] = 1;
            checkcol[n_x]--;
            checkrow[n_y]--;
        }
        for(int i=0; i<MAX; i++)
        {
            for(int j=0; j<MAX; j++)
            {
                if(maze[i][j])cout<<setw(2)<<"#";
                else cout<<setw(2)<<".";
                //cout<<setw(2)<<maze[i][j];
            }
            cout<<'\n';
        }
    }

    return 0;
}
