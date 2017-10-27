/**
 * @file    nsSBCharSetProber.cpp
 * @brief   nsSBCharSetProber
 * @license GPL 2.0/LGPL 2.1
 */

#include <stdio.h>
#include "nsSBCharSetProber.h"

nsProbingState nsSingleByteCharSetProber::HandleData(const char* aBuf, PRUint32 aLen)
{
  unsigned char order;

  for (PRUint32 i = 0; i < aLen; i++)
  {
    // Order is in [1-64] but we want 0-63 here. 
    order = mModel->charToOrderMap[(unsigned char)aBuf[i]] -1;

    if (order < SYMBOL_CAT_ORDER)
      mTotalChar++;
    if (order < SAMPLE_SIZE)
    {
        mFreqChar++;

      if (mLastOrder < SAMPLE_SIZE)
      {
        mTotalSeqs++;
        if (!mReversed)
          ++(mSeqCounters[mModel->precedenceMatrix[mLastOrder*SAMPLE_SIZE+order]]);
        else // reverse the order of the letters in the lookup
          ++(mSeqCounters[mModel->precedenceMatrix[order*SAMPLE_SIZE+mLastOrder]]);
      }
    }
    mLastOrder = order;
  }

  if (mState == eDetecting)
    if (mTotalSeqs > SB_ENOUGH_REL_THRESHOLD)
    {
      float cf = GetConfidence();
      if (cf > POSITIVE_SHORTCUT_THRESHOLD)
        mState = eFoundIt;
      else if (cf < NEGATIVE_SHORTCUT_THRESHOLD)
        mState = eNotMe;
    }

  return mState;
}

void  nsSingleByteCharSetProber::Reset(void)
{
  mState = eDetecting;
  mLastOrder = 255;
  for (PRUint32 i = 0; i < NUMBER_OF_SEQ_CAT; i++)
    mSeqCounters[i] = 0;
  mTotalSeqs = 0;
  mTotalChar = 0;
  mFreqChar = 0;
}

//#define NEGATIVE_APPROACH 1

float nsSingleByteCharSetProber::GetConfidence(void)
{
#ifdef NEGATIVE_APPROACH
  if (mTotalSeqs > 0)
    if (mTotalSeqs > mSeqCounters[NEGATIVE_CAT]*10 )
      return ((float)(mTotalSeqs - mSeqCounters[NEGATIVE_CAT]*10))/mTotalSeqs * mFreqChar / mTotalChar;
  return (float)0.01;
#else  //POSITIVE_APPROACH
  float r;

  if (mTotalSeqs > 0) {
    r = ((float)1.0) * mSeqCounters[POSITIVE_CAT] / mTotalSeqs / mModel->mTypicalPositiveRatio;
    r = r*mFreqChar/mTotalChar;
    if (r >= (float)1.00)
      r = (float)0.99;
    return r;
  }
  return (float)0.01;
#endif
}

const char* nsSingleByteCharSetProber::GetCharSetName() 
{
  if (!mNameProber)
    return mModel->charsetName;
  return mNameProber->GetCharSetName();
}

#ifdef DEBUG_chardet
void nsSingleByteCharSetProber::DumpStatus()
{
  printf("  SBCS: %1.3f [%s] [%s]\r\n", GetConfidence(), mModel->langName, GetCharSetName());
}
#endif
