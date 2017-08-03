/*
 Write a C program to solve this popular job interview problem: Given an array input[] of n (where n >= 3) integers, construct an array output[] (of same size) such that output[i] is equal to the product of all the elements of input[] except input[i]. Solve it without division operator and in O(n) time and space. (Note the simple and obvious solution is O(N2) or uses division to remove the ith element or multiplication by the inverse which is the same as division
 */




#include <stdio.h>
#include <stdlib.h>

void print_answer(int * below_product, int * above_product, int count)
{
    for(int i=0;i<count;i++)
        printf("%d\n",below_product[i]*above_product[i]);
}

void compute_above_product(int *input_array, int * above_product, int count)
{
    above_product[0]=1;
    
    for(int i=1;i<count;i++)
        above_product[i]=input_array[i-1]*above_product[i-1];
}

void compute_below_product(int *input_array, int * below_product, int count)
{
    below_product[count-1]=1;
    
    for(int i=count-2;i>=0;i--)
        below_product[i]=input_array[i+1]*below_product[i+1];
    
}

int get_input_array(int *input_array)
{
    int i=0, num=0;
    while(scanf("%d", &num)>0)
        input_array[i++]=num;
    input_array[i]=-1;
    return i;
}

void process(int count, int *input,int *below_product, int *above_product)
{
    compute_below_product(input,below_product,count);
    compute_above_product(input,above_product,count);
    print_answer(below_product, above_product, count);
}

int main()
{
    int count=0;
    int input[1024], below_product[1024], above_product[1024];
    
    count=get_input_array(input);
    process(count,input,below_product,above_product);
    
    return 0;
}
