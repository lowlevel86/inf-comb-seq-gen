#include <stdio.h>
#include <stdlib.h>
#include "combSeq.h"

//compile:
//gcc main.c combSeq.c -o prog -lm

int main(int argc, char **argv)
{
   int64_t i, j; // int64_t to insure "i" does not overflow
   unsigned int iStart, iEnd;
   int paramCnt;
   int setSize;
   int setNum[32];
   
   printf("usage: prog [parameter count] [start] [end]\n\n");
   printf("example: prog 2 0 100\n\n");
   
   if (argc < 4)
   return 0;
   
   paramCnt = atoi(argv[1]);
   iStart = atoi(argv[2]);
   iEnd = atoi(argv[3]);
   
   if (paramCnt <= 32)
   for (i=iStart; i <= iEnd; i++)
   {
      retCombSeq(paramCnt, i, &setSize, &setNum[0]);
      
      for (j=0; j < paramCnt; j++)
      printf("%i,%i\t", setSize, setNum[j]);
      
      for (j=0; j < paramCnt; j++)
      printf("%7.3f ", (float)setNum[j] / (setSize - 1.0));
      
      printf("\n");
   }
   
   return 0;
}
