#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <windows.h>
#define MAXN 19
#define STKL 131072
#define CLS system("cls")
const int dirs[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};//y,x
int maze[MAXN+2][MAXN+2];
char mark[MAXN+2][MAXN+2];
int stk[STKL][3];//y,x,dir
int top;

void gotoxy(int xpos, int ypos)
{
    COORD scrn;
    HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
    scrn.X = xpos;
    scrn.Y = ypos;
    SetConsoleCursorPosition(hOuput,scrn);
}

void prtchar(int crr_row, int crr_col, int crr_dir)
{
    static int pr_col=1, pr_row=1;
    gotoxy(pr_col*2, pr_row);
    printf(maze[pr_row][pr_col] ? "█":"  ");
    gotoxy(crr_col*2, crr_row);
    switch(crr_dir)
    {
    case 0:
        printf("↓");
        break;
    case 1:
        printf("→");
        break;
    case 2:
        printf("↑");
        break;
    case 3:
        printf("←");
        break;
    case -1:
        printf(" O");
        break;
    case -2:
        printf(" W");
        break;
    case -3:
        printf(" M");
        break;
    default:
        printf(" X");
        break;
    }
    pr_col = crr_col;
    pr_row = crr_row;
    _sleep(250);
}

void prtout(void)
{
    int i, j;
    for(i=0; i<MAXN; i++)
    {
        for(j=0; j<MAXN; j++)
        {
            printf(maze[i][j] ? "█":"  ");
        }
        printf("\n");
    }
    printf("狀態\nO:\t抵達\nW:\t撞牆\nM:\t已拜訪過\nX:\t死路");
}

int main(void)
{
    char *locale;
    int i,j,y,x,dir,ny,nx;
    int crr_pos[3];//y,x
    char found = 0;
    locale = setlocale(LC_ALL, "");
    memset(mark,0x00,sizeof(mark));
    mark[1][1] = 1;
    top = 0;
    i=2; while(i--){stk[top][i]=1;}//push
    stk[top][2] = 0;
    for(i=0; i<MAXN; i++)
        for(j=0; j<MAXN; j++)
        {
            scanf("%d",&maze[i][j]);
            //printf("%d %d\n", i, j);
        }
    CLS;
    puts("Start!");
    _sleep(1800);
    CLS;
    prtout();
    while(top>=0 && !found)
    {
        i=3; while(i--){crr_pos[i] = stk[top][i];}; --top;//pop
        y = crr_pos[0];
        x = crr_pos[1];
        dir = crr_pos[2];
        while(dir<4&&!found)
        {
            //prtchar(y,x,dir);
            ny = y + dirs[dir][0];
            nx = x + dirs[dir][1];
            if( ny==MAXN-2 && nx==MAXN-2 )
                found = 1;
            else if( !maze[ny][nx] && !mark[ny][nx])
            {
                mark[ny][nx] = 1;
                prtchar(ny,nx,dir);
                ++top;
                stk[top][0] = y;
                stk[top][1] = x;
                stk[top][2] = ++dir;
                x = nx; y = ny; dir=0;
            }
            else
            {
                prtchar(ny,nx, mark[ny][nx]?-3:-2);//Visited or Wall
                ++dir;
            }
        }
        found ? prtchar(ny,nx,-found):prtchar(y,x,dir);//Found or 'X'
    }
    if(found)
    {
        //prtchar(ny,nx,-found);
        CLS;
        printf("Row.\tCol.\n");
        for(i=0; i<=top; i++)
            printf("%d\t%d\n", stk[i][0], stk[i][1]);
        printf("%d\t%d\n%d\t%d\n", y, x, ny, nx);
    }

    return 0;
}
