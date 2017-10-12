/**
 * @file    nsEUCJPProber.cpp
 * @brief   nsEUCJPProber
 * @license GPL 2.0/LGPL 2.1
 */

// for japanese encoding, obeserve characteristic:
// 1, kana character (or hankaku?) often have hight frequency of appereance
// 2, kana character often exist in group
// 3, certain combination of kana is never used in japanese language

#include "nsEUCJPProber.h"
#include "nsDebug.h"

void  nsEUCJPProber::Reset(void)
{
  mCodingSM->Reset(); 
  mState = eDetecting;
  mContextAnalyser.Reset(mIsPreferredLanguage);
  mDistributionAnalyser.Reset(mIsPreferredLanguage);
}

nsProbingState nsEUCJPProber::HandleData(const char* aBuf, PRUint32 aLen)
{
  NS_ASSERTION(aLen, "HandleData called with empty buffer");
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
      PRUint32 charLen = mCodingSM->GetCurrentCharLen();

      if (i == 0)
      {
        mLastChar[1] = aBuf[0];
        mContextAnalyser.HandleOneChar(mLastChar, charLen);
        mDistributionAnalyser.HandleOneChar(mLastChar, charLen);
      }
      else
      {
        mContextAnalyser.HandleOneChar(aBuf+i-1, charLen);
        mDistributionAnalyser.HandleOneChar(aBuf+i-1, charLen);
      }
    }
  }

  mLastChar[0] = aBuf[aLen-1];

  if (mState == eDetecting)
    if (mContextAnalyser.GotEnoughData() && GetConfidence() > SHORTCUT_THRESHOLD)
      mState = eFoundIt;

  return mState;
}

float nsEUCJPProber::GetConfidence(void)
{
  float contxtCf = mContextAnalyser.GetConfidence();
  float distribCf = mDistributionAnalyser.GetConfidence();

  return (contxtCf > distribCf ? contxtCf : distribCf);
}

