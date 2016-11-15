/*
*   Sorting Practice from 581 course;
*   Author: Kuang-Yu Jeng;
*   Date: 2016/5/23;
*	Note: <stdio.h>, <string.h>, <pthread.h> and <time.h> are needed; So does the custom comparison function ;)
*         And it's better to include <limits.h> too.
*   PS:   It seems that the boost filed :(
*/
#if RAND_MAX==32767
#define ENLARGE_RAND
#endif // RAND_MAX

#ifndef INCLUDE_SORT
#define INCLUDE_SORT

#define MAX_DEPTH 3
#define PARALLEL_LEN 3072

/*It seems that Heap-sort does not help...*/
#if 1
#define ENABLE_HEAP
#endif

#ifndef S_SLOW_MODE
#define ENABLE_SHORT
#define ENABLE_INSERT
#define NOMID
#endif

#ifdef ENABLE_HEAP
#ifndef GROUP
#define GROUP 15 //Default Heap size-1
#endif
#endif // ENABLE_HEAP

//Parameter, ARGVS
typedef struct
{
    int left;
    int right;
    char *ptr;
    size_t block;
    size_t word_len;
    int (*cmp)(const void *, const void *);
    int depth;
} ARGVS;

void real_sort(int left, const int right, char *ptr, const size_t block, const size_t word_len, int (*cmp)(const void *, const void *), int depth);
void* qsort_thread(void *init);
void my_qsort(void *ptr, size_t n, size_t word_len, int (*cmp)(const void *, const void *));

void* qsort_thread(void *init)// with the format of pthread_create :(void*)( void *parameter)
{
    ARGVS *starter = (ARGVS*)init;
    real_sort(starter->left, starter->right, starter->ptr, starter->block, starter->word_len, starter->cmp, starter->depth);//Passing all parameters
    pthread_exit(NULL);
}

void real_sort(int left, const int right, char *ptr, const size_t block, const size_t word_len, int (*cmp)(const void *, const void *), int depth)
{
    if(left>=right)return;/*The size of partition <= 0, exit.*/
    int i, j, rd_m;
    register char *t2;
    int k, pa;
    int left_p, right_p;
    char *temp;
    while(left<right)
    {
#ifdef ENABLE_SHORT
        if( right - left == 1 ) //If there are only 2 elements, no sorting is needed.
        {
            temp = (char*)malloc(word_len);/*Allocate temporary variable.*/
            if( cmp(ptr+left*block, ptr+right*block) > 0 )
            {
                memcpy(temp, ptr+left*block, word_len);
                memcpy(ptr+left*block, ptr+right*block, word_len);
                memcpy(ptr+right*block, temp, word_len);
            }
            free(temp);
            return;
        }
#endif
#ifdef ENABLE_INSERT
        if( right - left < 10 ) // Less than 10 elements
        {
            temp = (char*)malloc(word_len);
            for(i=left+1; i<=right; i++)
            {
                memcpy(temp,ptr+i*block,word_len);
                j = i-1;
                while(j>=0 && cmp(temp, ptr+j*block) < 0)
                {
                    memcpy(ptr+(j+1)*block, ptr+j*block, word_len);
                    j--;
                }
                memcpy(ptr+(j+1)*block, temp, word_len);
            }
            free(temp);
            return;
        }
#endif

#ifdef ENABLE_HEAP
        if( right - left < GROUP ) //If there are GROUP elements, switch to heap sort
        {
            t2 = (char*)malloc(word_len);
            temp = (char*)malloc(word_len*(GROUP));
            j = -1;
            for(i=left; i<=right; i++)
            {
                memcpy(temp+(++j)*block,ptr+i*block,word_len);
                if(j==0)continue;
                k = j;
                while(k>0)
                {
                    pa = (k-1)>>1;
                    if(cmp(temp+k*block,temp+pa*block)<=0)
                    {
                        memcpy(t2,temp+k*block,word_len);
                        memcpy(temp+k*block,temp+pa*block,word_len);
                        memcpy(temp+pa*block, t2, word_len);
                        k = pa;
                    }
                    else break;;
                }
            }

            for(i=left; i<=right; i++)
            {
                memcpy(ptr+i*block, temp, word_len);

                pa = k = 0;
                if(j==-1)break;
                memcpy(temp,temp+j*block,word_len);
                j--;
                while((pa=(k<<1)+1) <= j)
                {
                    if(pa+1 <= j && cmp(temp+pa*block, temp+(pa+1)*block) > 0 ) pa+=1;
                    if(cmp(temp+k*block, temp+pa*block) > 0)
                    {
                        memcpy(t2, temp+k*block, word_len);
                        memcpy(temp+k*block, temp+pa*block, word_len);
                        memcpy(temp+pa*block, t2, word_len);
                        k = pa;
                    }
                    else break;
                }
            }

            free(temp);
            free(t2);
            return;
        }
#endif // 1

#ifdef S_SLOW_MODE
        rd_m = left;
#endif

#ifndef S_SLOW_MODE
#ifndef ENLARGE_RAND
        rd_m = left + 1 + (((int)rand())%(right-left-1)); //Get random pivot
#endif
#ifdef ENLARGE_RAND //Enlarge to 2147483647
        rd_m = left + 1 + ((int)((rand()<<16)|(rand()<<1)|rand()&1) % (right-left-1)); //Get random pivot
#endif
#ifdef MID
        /*Choose better pivot*/
        if( ( (k=cmp(ptr+rd_m*block,ptr+left*block)) < 0 && (pa=cmp(ptr+left*block, ptr+right*block)) < 0) ||\
                ( k>0 && pa>0 ) ) //Suppose LEFT is mid;
        {
            rd_m = left;
        }
        else if( ( (k=cmp(ptr+rd_m*block,ptr+right*block)) < 0 && (pa=cmp(ptr+right*block, ptr+left*block)) < 0) ||\
                 ( k>0 && pa>0 ) ) //Suppose RIGHT is mid;
        {
            rd_m = right;
        }
#endif //MID
#endif //S_SLOW_MODE
        temp = (char*)malloc(word_len);
        t2 = (char*)malloc(word_len);
        i = left;
        j = right;
        k = left+1;

        memcpy(t2, ptr+rd_m*block, word_len);
        memcpy(ptr+rd_m*block, ptr+left*block, word_len);
        memcpy(ptr+left*block, t2, word_len);
        while( k <= j )
        {
            if( (pa=cmp(ptr+k*block, t2)) < 0 )
            {
                memcpy(temp, ptr+k*block, word_len);
                memcpy(ptr+k*block, ptr+i*block, word_len);
                memcpy(ptr+i*block, temp, word_len);
                k++;
                i++;
            }
            else if( pa > 0 )
            {
                memcpy(temp, ptr+k*block, word_len);
                memcpy(ptr+k*block, ptr+j*block, word_len);
                memcpy(ptr+j*block, temp, word_len);
                j--;
            }
            else k++;
        }

        free(t2);
        free(temp);

        /*Parallel Scope*/
        if(depth < MAX_DEPTH && right - left > PARALLEL_LEN)
        {
            ARGVS arg =
            {
                left, i-1, ptr, block, word_len, cmp, depth+1
            };
            pthread_t thread;//New thread
            int re = pthread_create(&thread, PTHREAD_CREATE_JOINABLE, (qsort_thread), (void*)(&arg) );//Add a new thread
            if(re)
            {
                fprintf(stderr,"Failed!\n");
                exit(-1);
            }
            real_sort(j+1, right, ptr, block, word_len, cmp, depth+1);//Parallel processing
            pthread_join(thread, NULL);//Wait the thread terminate
            return;//Halt
        }
        /*End of Parallel*/
        else
        {
            real_sort(left, i-1, ptr, block, word_len, cmp, depth+1);
            left = j+1;//For reusing the recursive stack
        }
    }
}

void my_qsort(void *ptr, size_t n, size_t word_len, int (*cmp)(const void *, const void *))
{
    //Initialize random seed;
    srand((unsigned)time(NULL));
    real_sort(0, n-1, (char*)ptr, (word_len/sizeof(char)), word_len, cmp, 0);//All cast to char;
}
#endif
