/*
*   Sorting Practice from 581 course;
*   Author: Kuang-Yu Jeng;
*   Date: 2016/5/23;
*	Note: <stdio.h>, <string.h> and <time.h> are needed; So does the custom compare function ;)
*/

#ifndef INCLUDE_SORT
#define INCLUDE_SORT

#ifndef S_SLOW_MODE
	#define ENABLE_SHORT
	#define ENABLE_HEAP
#endif

#ifdef ENABLE_HEAP
	#ifndef GROUP
	#define GROUP 4 //Default Heap size-1
	#endif
#endif // ENABLE_HEAP

void real_sort(const int left, const int right, char *ptr, const size_t block, const size_t word_len, int (*cmp)(const void *, const void *))
{
    if(left>=right)return;/*The size of partition <= 0, exit.*/
    int i, j, rd_m;
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

#ifdef ENABLE_HEAP
    if( right - left <= GROUP ) //If there are 8 elements, switch to heap sort
    {
        int k, pa;
        register char *t2 = (char*)malloc(word_len);
        temp = (char*)malloc(word_len*(GROUP+1));
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
        //for(i=0; i<=j; i++)printf("## %d\n", *(int*)(temp+block*i));
        //puts("ok");
        for(i=left; i<=right; i++)
        {
            memcpy(ptr+i*block, temp, word_len);
            //printf("de %d\n", *(int*)(ptr+i*block));
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
        //for(i=0; i<=9; i++)printf("?? %d\n", *(int*)(temp+block*i));
        //puts("ok2");
        free(temp);
        free(t2);
        return;
    }
#endif // 1

    temp = (char*)malloc(word_len);/*Allocate temporary variable.*/
    rd_m = left;
#ifndef S_SLOW_MODE
    rd_m += (((int)rand())%(right-left+1)); //Get random pivot
    memcpy(temp, ptr+rd_m*block, word_len);//random pivot? Move from rd_m to leftmost.
    memcpy(ptr+rd_m*block, ptr+left*block, word_len);
    memcpy(ptr+left*block,temp,word_len);
#endif
    /*begin to partition.*/
    i = left;
    j = right+1;
    do
    {
        do{i++;}while( cmp( ptr+block*i, ptr+block*left ) < 0 );//Find bigger than pivot
        do{j--;}while( cmp( ptr+block*j, ptr+block*left ) > 0 );//Find smaller than pivot
        if(i<j)
        {
            /*Swap it.*/
            memcpy(temp, ptr+block*i, word_len);
            memcpy(ptr+block*i, ptr+block*j, word_len);
            memcpy(ptr+block*j, temp, word_len);
        }
    }while(i<j);
    memcpy(temp, ptr+block*j, word_len);
    memcpy(ptr+block*j, ptr+block*left, word_len);
    memcpy(ptr+block*left, temp, word_len);
    free(temp); temp=NULL;
    /*End of partition.*/
    real_sort(left, j-1, ptr, block, word_len, cmp);
    real_sort(j+1, right, ptr, block, word_len, cmp);
}

void my_qsort(void *ptr, size_t n, size_t word_len, int (*cmp)(const void *, const void *))
{
    //Initialize random seed;
    srand((unsigned int)time(NULL));
    real_sort(0, n-1, (char*)ptr, (word_len/sizeof(char)), word_len, cmp);//All cast to char;
}
#endif
