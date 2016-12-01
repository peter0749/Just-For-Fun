#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "msort.h"
#define MAXN 10000000

int cmp(const void *a, const void *b){
    return *(int*)a - *(int*)b;
}

int arr[MAXN];

int main(void){
    int i;
    srand(time(NULL));
    for(i=0; i<MAXN; ++i) arr[i] = rand();
    mymergesort(arr,sizeof(arr)/sizeof(int),sizeof(int),cmp);
    //for(i=0; i<sizeof(arr)/sizeof(int); ++i) printf("%d\n",arr[i]);
    return 0;
}
