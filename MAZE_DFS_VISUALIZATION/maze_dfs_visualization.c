#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define UVIS
#ifdef VIS
#include <windows.h>
#endif
#define MAXN 101
#define STKL 131072
#define STRL 2000
#define _MIN(X,Y) (X) < (Y) ? (X) : (Y)
#define CLS CLSCR()

//const int dirs[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};//y,x
const int dirsA[4][2] = {{0,1},{1,0},{-1,0},{0,-1}};
const int dirsB[4][2] = {{0,-1},{-1,0},{1,0},{0,1}};
int maze[2][MAXN+2][MAXN+2];
char mark[2][MAXN+2][MAXN+2];
int stkA[STKL][4];//y,x,dir
int stkB[STKL][4];//y,x,dir
int fullpathA[STKL][3];//z,y,x
int fullpathB[STKL][3];//z,y,x
int topA, topB, fA, fB;
int startA[3]= {0,1,1}, startB[3]= {1,99,99}, endA[3]= {0,99,99}, endB[3]= {0,1,1};

#ifdef VIS
void gotoxy(int xpos, int ypos)
{
    COORD scrn;
    HANDLE hOuput = GetStdHandle(STD_ERROR_HANDLE);
    scrn.X = xpos;
    scrn.Y = ypos;
    SetConsoleCursorPosition(hOuput,scrn);
}

void CLSCR(void)
{
    int i;
    for(i=0; i<300; ++i) fputs("",stderr);
    gotoxy(0,0);
}

void prtout(int crr_level)
{
    int i, j;
    CLS;
    for(i=0; i<MAXN; i++)
    {
        for(j=0; j<MAXN; j++)
        {
            fprintf(stderr,maze[crr_level][i][j] ? (maze[crr_level][i][j]==2?" o":"█"):"  ");
        }
        fprintf(stderr,"\n");
    }
    //printf("狀態\nO:\t抵達\nW:\t撞牆\nM:\t已拜訪過\nX:\t死路");
}

void prtchar(int crr_row, int crr_col, int crr_dir, int crr_level)
{
    static int pr_col=1, pr_row=1, pr_level=-1;
    if(crr_level!=pr_level)
    {
        _sleep(1000);
        CLS;
        fputs("Changing map...",stderr);
        _sleep(1000);
        prtout(crr_level);
        pr_level = crr_level;
    }
    else
    {
        gotoxy(pr_col*2, pr_row);
        fprintf(stderr,maze[pr_level][pr_row][pr_col] ? (maze[pr_level][pr_row][pr_col]==2?" o":"█"):"  ");
    }
    gotoxy(crr_col*2, crr_row);
    switch(crr_dir)
    {
    case 0:
        fprintf(stderr,"→");
        break;
    case 1:
        fprintf(stderr,"↓");
        break;
    case 2:
        fprintf(stderr,"↑");
        break;
    case 3:
        fprintf(stderr,"←");
        break;
    case -1:
        fprintf(stderr," O");
        break;
    case -2:
        fprintf(stderr," W");
        break;
    case -3:
        fprintf(stderr," M");
        break;
    default:
        fprintf(stderr," D");
        break;
    }
    pr_col = crr_col;
    pr_row = crr_row;
    usleep(150000);
}
#endif
char go_mice(const int startpos[3], const int endpos[3], const int dirs[4][2], int *top, int stk[STKL][4], int *fp, int fstk[STKL][3])
{
    int crr_pos[4];
    int z,y,x,nz,ny,nx,dir,i;
    char found=0, teleported=0;
    const int z_dir = endpos[0] - startpos[0];
    memset(mark,0x00,sizeof(mark));
    mark[startpos[0]][startpos[1]][startpos[2]] = 1;
    (*top)++;
    stk[*top][0] = startpos[0];
    stk[*top][1] = startpos[1];
    stk[*top][2] = startpos[2];
    stk[*top][3] = 0;
    //for(i=0, ++(*fp); i<3; i++) fstk[*fp][i] = stk[*top][i];
    #ifdef VIS
    CLS;
    prtout(startpos[0]);
    #endif
    while(*top>=0 && !found)
    {
        i=4;
        while(i--)
        {
            crr_pos[i] = stk[*top][i];
        };
        (*top)--;//pop
        z = crr_pos[0];
        y = crr_pos[1];
        x = crr_pos[2];
        dir = crr_pos[3];
        #ifdef VIS
        prtchar(y,x,dir,z);
        #endif
        for(i=0, ++(*fp); i<3; i++) fstk[*fp][i] = crr_pos[i];//Mouse back track
        while(dir<4&&!found)
        {
            nz = z;
            ny = y + dirs[dir][0];
            nx = x + dirs[dir][1];
            //printf("%d,%d,%d\n", ny,nx,dir);
            if( nz==endpos[0] && ny==endpos[1] && nx==endpos[2] )
                found = 1;
            else if( !maze[nz][ny][nx] && !mark[nz][ny][nx])
            {
                #ifdef VIS
                prtchar(ny,nx,dir,nz);
                #endif
                ++(*fp);//Mouse explore new area
                fstk[*fp][0] = nz;
                fstk[*fp][1] = ny;
                fstk[*fp][2] = nx;
                mark[nz][ny][nx] = 1;
                (*top)++;
                stk[*top][0] = z;
                stk[*top][1] = y;
                stk[*top][2] = x;
                stk[*top][3] = ++dir;
                x = nx;
                y = ny;
                z = nz;
                dir=0;
            }
            else
            {
                ++dir;
            }
        }
        //found ? prtchar(ny,nx,-found):prtchar(y,x,dir);//Found or 'X'
    }
    if(found)
    {
        (*top)++;
        stk[*top][0] = z;
        stk[*top][1] = y;
        stk[*top][2] = x;
        //for(i=0, ++(*fp); i<3; i++) fstk[*fp][i] = stk[*top][i];
        (*top)++;
        stk[*top][0] = nz;
        stk[*top][1] = ny;
        stk[*top][2] = nx;
        //for(i=0, ++(*fp); i<3; i++) fstk[*fp][i] = stk[*top][i];//Mouse at endpoint
        #ifdef VIS
        CLS;
        fprintf(stderr,"Row.\tCol.\n");
        for(i=0; i<=*top; i++)
            fprintf(stderr,"%d\t%d\t%d\n", stk[i][0], stk[i][1], stk[i][2]);
        #endif
    }
    return found;
}

int main(int argc, char *argv[])
{
    int i,j,z,y,x,dir,nz,ny,nx;
    int crr_pos[4];//y,x
    char found = 0, flagA, flagB;
    char input[STRL];
    int step;
    FILE *FP, *FOUT;
    FP = fopen(argv[1], "r");
    FOUT = fopen("result.txt","w");
    if(!FP || !FOUT) exit(-1);
    fgets(input,STRL, FP);
    for(i=0; i<MAXN; i++)
    {
        fgets(input,STRL, FP);
        for(j=0; j<MAXN; j++)
        {
            maze[0][i][j] = (input[j]=='.'?0:(input[j]=='o'?2:1));
            if(maze[0][i][j]==2)
            {
                endA[0] = 0; endA[1] = i; endA[2] = j;
            }
        }
    }
    fgets(input,STRL, FP);
    for(i=0; i<MAXN; i++)
    {
        fgets(input,STRL, FP);
        for(j=0; j<MAXN; j++)
        {
            maze[1][i][j] = (input[j]=='.'?0:(input[j]=='o'?2:1));
            if(maze[1][i][j]==2)
            {
                endB[0] = 1; endB[1] = i; endB[2] = j;
            }
        }
    }
    fA = topA = -1;
    flagA = go_mice(startA,endA,dirsA,&topA,stkA,&fA,fullpathA);
    flagB = go_mice(endB,startB,dirsA,&topA,stkA,&fA,fullpathA);
    fB = topB = -1;
    go_mice(startB,endB,dirsB,&topB,stkB,&fB,fullpathB);
    go_mice(endA,startA,dirsB,&topB,stkB,&fB,fullpathB);
    printf("Short Path:\n");
    step = _MIN(topA,topB);
    for(i=0; i<=step; i++)
    {
        printf("Step:%d\tratA(%d,%d,%d)\tratB(%d,%d,%d)\n",i,stkA[i][0],stkA[i][1],stkA[i][2],stkB[i][0],stkB[i][1],stkB[i][2]);
    }
    printf("Full Path:\n");
    step = _MIN(fA,fB);
    for(i=0; i<=step && !( fullpathA[i][0]==fullpathB[i][0] && fullpathA[i][1]==fullpathB[i][1] && fullpathA[i][2]==fullpathB[i][2] ); i++)
    {
        printf("Step:%d\tratA(%d,%d,%d)\tratB(%d,%d,%d)\n",i,fullpathA[i][0],fullpathA[i][1],fullpathA[i][2],fullpathB[i][0],fullpathB[i][1],fullpathB[i][2]);
        fprintf(FOUT,"Step:%d\tratA(%d,%d,%d)\tratB(%d,%d,%d)\n",i,fullpathA[i][0],fullpathA[i][1],fullpathA[i][2],fullpathB[i][0],fullpathB[i][1],fullpathB[i][2]);
    }
    if(i<=step)
    {
        printf("Rats met each other in (%d,%d,%d)\n",fullpathA[i][0],fullpathA[i][1],fullpathA[i][2]);
        fprintf(FOUT,"Rats met each other in (%d,%d,%d)\n",fullpathA[i][0],fullpathA[i][1],fullpathA[i][2]);
    }
    else
    {
        printf("Rats didn't meet each other in the maze.\n");
        fprintf(FOUT,"Rats didn't meet each other in the maze.\n");
        if(fA==fB)
        {
            printf("Tie!\n");
            fprintf(FOUT,"Tie!\n");
        }
        else if(fA<fB)
        {
            printf("ratA arrived first!\n");
            fprintf(FOUT,"ratA arrived first!\n");
        }
        else
        {
            printf("ratB arrived first!\n");
            fprintf(FOUT,"ratB arrived first!\n");
        }
    }
    fclose(FOUT); fclose(FP);
    return 0;
}
