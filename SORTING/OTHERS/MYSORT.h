/*
*   Sorting Practice from 581 course;
*   Author: Kuang-Yu Jeng;
*   Date: 2016/5/23;
*	Note: <stdio.h>, <string.h> and <time.h> are needed; So does the custom comparison function ;)
*         And it's better to include <limits.h> too.
*/
#if RAND_MAX==32767
#define ENLARGE_RAND
#endif // RAND_MAX

#ifndef INCLUDE_SORT
#define INCLUDE_SORT

/*It's seems that Heap-sort does not help...*/
#if 0
    #define ENABLE_HEAP
#endif

#ifndef S_SLOW_MODE
	#define ENABLE_SHORT
	#define ENABLE_INSERT
#endif

#ifdef ENABLE_HEAP
	#ifndef GROUP
	#define GROUP 15 //Default Heap size-1
	#endif
#endif // ENABLE_HEAP

void real_sort(const int left, const int right, char *ptr, const size_t block, const size_t word_len, int (*cmp)(const void *, const void *))
{
    if(left>=right)return;/*The size of partition <= 0, exit.*/
    int i, j, rd_m;
	register char *t2;
	int k, pa;
    char *temp;
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
            memcpy(temp,temp+j*block,word_len); j--;
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

#endif
	temp = (char*)malloc(word_len);/*Allocate temporary variable.*/
	t2 = (char*)malloc(word_len);
	memcpy(t2,ptr+rd_m*block,word_len);
    /*begin to partition.*/
    i = left-1;
    j = right+1;
    do
    {
        do{i++;}while( cmp( ptr+block*i, t2 ) < 0 );//Find bigger than pivot
        do{j--;}while( cmp( ptr+block*j, t2 ) > 0 );//Find smaller than pivot
        if(i<j)
        {
            /*Swap it.*/
            memcpy(temp, ptr+block*i, word_len);
            memcpy(ptr+block*i, ptr+block*j, word_len);
            memcpy(ptr+block*j, temp, word_len);
        }
    }while(i<j);
    free(t2);
    free(temp);
    /*End of partition.*/
    real_sort(left, i-1, ptr, block, word_len, cmp);
    real_sort(j+1, right, ptr, block, word_len, cmp);
}

void my_qsort(void *ptr, size_t n, size_t word_len, int (*cmp)(const void *, const void *))
{
    //Initialize random seed;
    srand((unsigned int)time(NULL));
    real_sort(0, n-1, (char*)ptr, (word_len/sizeof(char)), word_len, cmp);//All cast to char;
}
#endif
