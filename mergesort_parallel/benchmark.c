#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>
#include <assert.h>
#include "msort.h"
#define AMP 300
#define SIZE 10000
#define MAXN AMP*SIZE
int arr[MAXN];

int cmp(const void *a, const void *b){
        return *(int*)a - *(int*)b;
}

int main(void){
        int i,j,t;
        struct timeval startv, endv;
        struct timezone startz, endz;
        srand(time(NULL));
        for(j=1; j<=AMP; ++j){
                t = j*SIZE;
                for(i=0; i<t; ++i) arr[i]=rand();
                gettimeofday(&startv,&startz);
                mymergesort(arr,t,sizeof(int),cmp);
                gettimeofday(&endv,&endz);
                for(i=1; i<t; ++i) assert(arr[i-1]<=arr[i]);
                printf("%d\t%ld\n", t, (endv.tv_sec-startv.tv_sec)*1000000+(endv.tv_usec-startv.tv_usec));
        }
        return 0;
}
