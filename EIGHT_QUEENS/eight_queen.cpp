#include <bits/stdc++.h>
#define MAX 101
#define DEBUG
using namespace std;

int chb[MAX][MAX] = {{0}};
int dfsstk[MAX];
unsigned int testbit[MAX];


void prtout(const int n)
{
#ifdef DEBUG
    //char temp[MAX];
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            chb[i][j]=0;
        }
        chb[i][dfsstk[i]] = 1;
    }
#endif // DEBUG

    cout<<dfsstk[0];
    for(int i=1; i<n; i++)
    {
        cout<<' '<<dfsstk[i];
    }
    cout<<'\n';

#ifdef DEBUG
//cin.getline(temp,MAX);
    cout<<'\n';
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
            cout<<setw(2)<<chb[i][j];
        cout<<'\n';
    }
    cout<<"=====================\n";

#endif // DEBUG

}

int find_bit(const int n)
{
    int index = n/sizeof(int);
    int diff = n%sizeof(int);
    return testbit[index]&(1u<<diff);/*If found, return 1, else return 0.*/
}

void set_bit(const int n)
{
    int index = n/sizeof(int);
    int diff = n%sizeof(int);
    testbit[index] |= (1u<<diff);/*Set the nth bit.*/
}

void unset_bit(const int n)
{
    int index = n/sizeof(int);
    int diff = n%sizeof(int);
    testbit[index] &= ~(1u<<diff);/*unset the nth bit.*/
}

void dfs(int curr_d, const int max_d)
{
    int conflict(0);
    if(curr_d==max_d)
    {
        prtout(max_d);
    }
    else
    {
        for(int i=0; i<max_d; i++)
        {
            if(!find_bit(i))
            {
                for(int j=0; j<curr_d; j++)//o
                {
                    if(abs(dfsstk[j]-i)==(curr_d-j))
                    {
                        conflict=1;
                        break;
                    }
                }
                if(conflict)
                {
                    conflict=0;
                    continue;
                }
                //else
                dfsstk[curr_d] = i;
                set_bit(i);
                dfs(curr_d+1,max_d);
                unset_bit(i);
            }
        }
    }
}

int main(void)
{
    freopen("testout.txt","w",stdout);
    int n;
    for(int i=0; i<MAX; i++)
    {
        testbit[i] = 0;
    }
    cin>>n;
    dfs(0,n);
    return 0;
}
