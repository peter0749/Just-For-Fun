#define MAXDEPTH 2
//MAXDEPTH: If your number of cpus is 'n', then set log2(n)
#ifdef PARALLEL
#include <pthread.h>
#endif

#ifdef PARALLEL
typedef struct{
    char *array;
    size_t block;
    int L, R;
    int (*cmp)(const void *, const void *);
    int depth;
}MSORT_ARGS;
#endif

void merge(char *, size_t, int, int, int, int (*cmp)(const void *, const void *));
void devide(char *, size_t, int, int, int (*cmp)(const void *, const void *) ,int);
void *devide_thread(void*);
void mymergesort(void *, size_t , size_t , int (*cmp)(const void *, const void *));

void merge(char *array, size_t block, int L, int M, int R, int (*cmp)(const void *a, const void *b)){
    int i, j, k, t;
    char *temp = (char*)malloc(block*(R-L+1));
    i=L; j=M+1; k=0;
    while(i<=M && j<=R){
        t = cmp(array+block*i, array+block*j);
        if(t<0){
            memcpy(temp+k*block, array+block*i, block);
            ++i;
        }else{
            memcpy(temp+k*block, array+block*j, block);
            ++j;
        }
        ++k;
    }
    while(i<=M) memcpy(temp+(k++)*block, array+block*(i++), block);
    while(j<=R) memcpy(temp+(k++)*block, array+block*(j++), block);
    memcpy(array+block*L, temp, block*(R-L+1));
    free(temp);
    //pass
}
void devide(char *array , size_t block, int L, int R, int (*cmp)(const void *a, const void *b) ,int depth){
    //pass
    if(L==R)return;
    int M=(L+R)>>1;
#ifdef PARALLEL
    if(depth < MAXDEPTH){
        MSORT_ARGS arg1 = 
        {
            array, block, L, M, cmp, depth+1
        };
        MSORT_ARGS arg2 = 
        {
            array, block, M+1, R, cmp, depth+1
        };
        pthread_t thread1, thread2;
        int re = 
            pthread_create(&thread1, PTHREAD_CREATE_JOINABLE, (devide_thread), (void*)(&arg1));
        if(re) exit(-1);
        re = 
            pthread_create(&thread2, PTHREAD_CREATE_JOINABLE, (devide_thread), (void*)(&arg2));
        if(re) exit(-1);
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
    }else{
        devide(array, block, L,M,cmp, depth+1);
        devide(array, block, M+1, R,cmp, depth+1);
    }
#else
    devide(array, block, L,M,cmp, depth+1);
    devide(array, block, M+1, R,cmp, depth+1);
#endif
    merge(array, block, L, M, R, cmp);
}
#ifdef PARALLEL
void *devide_thread(void *init){
    MSORT_ARGS *s = (MSORT_ARGS*)init;
    devide(s->array,s->block,s->L,s->R,s->cmp,s->depth);
    pthread_exit(NULL);
}
#endif

void mymergesort(void *array, size_t num, size_t block, int (*cmp)(const void *a, const void *b)){
    devide((char*)array, block, 0, num-1, cmp, 0);
}
