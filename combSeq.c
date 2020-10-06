#include <stdio.h>
#include <math.h>
#include <inttypes.h>

#define TRUE 1
#define FALSE 0

int64_t retMagnitudeAreaOffset(int D, int L, int64_t A, int64_t I)
{
   int64_t offset, a, O, skip, PV, PF, b;
   
   offset = 0;
   a = pow(2, L) + 1;
   O = (a - 1) / 2 + 1;
   PV = pow(a, D - 1);
   PF = A + PV;
   skip = pow(O, D - 1);
   
   while (I - I / PF * PF < PF - PV)
   {
      PV /= a;
      PF = (PF - PV * a + PV) / O;
      skip /= O;
      b = I / PF * PF;
      offset += (I + PV) / PF * skip + b;
      I -= b;
   }
   
   return offset + I;
}

int64_t retTurbulentAreaOffset(int D, int L, int64_t A, int64_t endNum, int64_t I)
{
   int64_t offset, a, E, skip, PV, PF, b, c, last;
   
   if (I < endNum)
   return I;
   
   offset = 0;
   a = pow(2, L) + 1;
   E = pow(2, L) / 2;
   PV = endNum + (endNum - pow(a, D - 2));
   PF = (A - endNum * 2 + PV) / E;
   skip = pow(E, D - 1);
   last = FALSE;
   I -= endNum;
   
   while (!last)
   {
      b = I / PF * PF;
      
      if (I - b >= PF - PV)
      last = TRUE;
      
      skip /= E;
      offset += (I + PV) / PF * skip + b;
      I -= b;
      
      c = PV;
      PV /= a;
      PF = (PF - c + PV) / E;
   }
   
   I += endNum;
   return offset + I;
}

void retAreaSzsAndOffset(int dimensions, int layer,
                         int64_t *magnitudeAreaSz, int64_t *turbulentAreaSz,
                         int64_t *turbulentEndNum, int64_t *areaSzOffset)
{
   int64_t i;
   int64_t powOfTwoLayer;
   int64_t smEvenNum, smOddNum;
   int64_t powOfSmEvenNum;
   int64_t powOfSmOddNum;
   
   int64_t magnitudeNum;
   int64_t magnitudeNumSum;
   int64_t magnitudeSingleNumSum;
   int64_t magnitudeMultiplyOperand;

   int64_t turbulentNum;
   int64_t turbulentNumSum;
   int64_t turbulentSingleNumSum;
   int64_t turbulentMultiplyOperand;
   
   int64_t turbulentAreaSkipCnt;
   
   //initialize
   powOfTwoLayer = pow(2, layer);
   smEvenNum = powOfTwoLayer / 2;
   smOddNum = smEvenNum + 1;
   powOfSmEvenNum = pow(smEvenNum, dimensions-2);
   powOfSmOddNum = pow(smOddNum, dimensions-2);
   
   magnitudeNum = powOfTwoLayer+1;
   magnitudeNumSum = 0;
   magnitudeSingleNumSum = powOfSmOddNum * smEvenNum;
   magnitudeMultiplyOperand = magnitudeSingleNumSum;
   
   turbulentNum = powOfTwoLayer+3;
   turbulentNumSum = 0;
   turbulentSingleNumSum = powOfSmEvenNum * (smEvenNum-1);
   turbulentMultiplyOperand = turbulentSingleNumSum;
   *turbulentEndNum = 1;
   
   for (i=0; i < dimensions-2; i++)
   {
      *turbulentEndNum += magnitudeNum;
      turbulentNum = *turbulentEndNum + (*turbulentEndNum - magnitudeNum);
      turbulentMultiplyOperand /= smEvenNum;
      turbulentNumSum += turbulentNum * turbulentMultiplyOperand;
      magnitudeMultiplyOperand /= smOddNum;
      magnitudeNumSum += magnitudeNum * magnitudeMultiplyOperand;
      magnitudeNum *= powOfTwoLayer + 1;
   }
   
   *magnitudeAreaSz = magnitudeSingleNumSum + magnitudeNumSum;
   *turbulentAreaSz = turbulentSingleNumSum + turbulentNumSum;

   if (dimensions > 1)
   *turbulentAreaSz += *turbulentEndNum * 2;
   
   turbulentAreaSkipCnt = powOfSmEvenNum * smEvenNum;
   *areaSzOffset = *turbulentAreaSz + turbulentAreaSkipCnt;
}

int64_t retOddEvenGroupSkipInc(int dimensions, int layer, int64_t inc)
{
   int64_t magnitudeAreaSz, turbulentAreaSz;
   int64_t turbulentEndNum;
   int64_t areaSzOffset;
   int64_t incRemainder;
   int64_t incOffset;
   
   retAreaSzsAndOffset(dimensions, layer,
                       &magnitudeAreaSz, &turbulentAreaSz, &turbulentEndNum, &areaSzOffset);
   
   incRemainder = inc % (magnitudeAreaSz + turbulentAreaSz);

   if (incRemainder < magnitudeAreaSz)
   {
      incOffset = retMagnitudeAreaOffset(dimensions, layer, magnitudeAreaSz, incRemainder);
   }
   else
   {
      incOffset = retTurbulentAreaOffset(dimensions, layer, turbulentAreaSz, turbulentEndNum, incRemainder-magnitudeAreaSz);
      incOffset += areaSzOffset;
   }
   
   incOffset += inc / (magnitudeAreaSz + turbulentAreaSz) * (areaSzOffset + areaSzOffset);
   
   return incOffset;
}

int64_t retBaseNthDigit(int64_t base, int64_t num, int64_t digitLoc)
{
   int64_t digit;
   int64_t upperDigits;
   int64_t placeValue1;
   int64_t placeValue2;
   
   placeValue1 = pow(base, digitLoc);
   placeValue2 = placeValue1 * base;
   
   //use integer division to remove smaller digits
   upperDigits = num / (int64_t)placeValue1;
   
   //subtract away the larger digits
   digit = upperDigits - num / (int64_t)placeValue2 * base;
   
   return digit;
}

void retSingleGroupComb(int dimensions, int layer, int64_t inc, int *setSize, int *setNum)
{
   int64_t i;
   int64_t groupSize;
   
   groupSize = pow(2, layer-1);
   
   //find all combinations within the group
   for (i=0; i < dimensions; i++)
   setNum[i] = retBaseNthDigit(groupSize, inc, i) * 2 + 1;
   
   *setSize = groupSize * 2 + 1;
}

void retOddEvenGroupComb(int dimensions, int layer, int64_t inc, int *setSize, int *setNum)
{
   int64_t i;
   int64_t skipInc;
   int64_t oddSideGroupSize;
   
   oddSideGroupSize = pow(2, layer)+1;
   
   //find all combinations for both the odd group and the even group
   
   //skip over numbers that were used before
   skipInc = retOddEvenGroupSkipInc(dimensions, layer, inc);
   
   for (i=0; i < dimensions; i++)
   setNum[i] = retBaseNthDigit(oddSideGroupSize, skipInc, i);
   
   *setSize = oddSideGroupSize;
}

void retCombSeq(int valueArrayCnt, uint32_t inc, int *setSize, int *setNum)
{
   int64_t i;
   int64_t firstCombCnt;
   int layer;
   int64_t singleGroupCombCnt;
   int64_t layerValuesInc;
   int64_t pastLayersSum;
   
   
   //find the top and bottom most value combination
   firstCombCnt = pow(2, valueArrayCnt);
   
   if (inc < firstCombCnt)
   {
      for (i=0; i < valueArrayCnt; i++)
      setNum[i] = retBaseNthDigit(2, inc, i);
      
      *setSize = 2;
      return;
   }
   
   //find the layer
   layer = log(pow(inc, 1.0/valueArrayCnt)-1) / log(2) + 1;
   
   if (pow(pow(2, layer)+1, valueArrayCnt) == inc)
   layer++;
   
   pastLayersSum = pow(pow(2, layer-1)+1, valueArrayCnt);
   
   layerValuesInc = inc - pastLayersSum;
   singleGroupCombCnt = pow(firstCombCnt, layer-1);
   
   
   if (layerValuesInc < singleGroupCombCnt)
   retSingleGroupComb(valueArrayCnt, layer, layerValuesInc, &*setSize, &setNum[0]);
   else
   retOddEvenGroupComb(valueArrayCnt, layer, layerValuesInc-singleGroupCombCnt, &*setSize, &setNum[0]);
}
