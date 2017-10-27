/**
 * @file    nsUTF8Prober.cpp
 * @brief   nsUTF8Prober
 * @license GPL 2.0/LGPL 2.1
 */

#include "nsUTF8Prober.h"

void  nsUTF8Prober::Reset(void)
{
  mCodingSM->Reset(); 
  mNumOfMBChar = 0;
  mState = eDetecting;
}

nsProbingState nsUTF8Prober::HandleData(const char* aBuf, PRUint32 aLen)
{
  nsSMState codingState;

  for (PRUint32 i = 0; i < aLen; i++)
  {
    codingState = mCodingSM->NextState(aBuf[i]);
    if (codingState == eItsMe)
    {
      mState = eFoundIt;
      break;
    }
    if (codingState == eStart)
    {
      if (mCodingSM->GetCurrentCharLen() >= 2)
        mNumOfMBChar++;
    }
  }

  if (mState == eDetecting)
    if (GetConfidence() > SHORTCUT_THRESHOLD)
      mState = eFoundIt;
  return mState;
}

//#define ONE_CHAR_PROB   (float)0.50
/* use the value 0.547 so that the confident value may less than that of Thai when the file is encoded in windows-874 */
#define ONE_CHAR_PROB   (float)0.547

float nsUTF8Prober::GetConfidence(void)
{
  float unlike = (float)0.99;

  if (mNumOfMBChar < 6)
  {
    for (PRUint32 i = 0; i < mNumOfMBChar; i++)
      unlike *= ONE_CHAR_PROB;
    return (float)1.0 - unlike;
  }
  else
    return (float)0.99;
}

