/**
 * @file    nsEUCKRProber.h
 * @brief   nsEUCKRProber
 * @license GPL 2.0/LGPL 2.1
 */

#ifndef nsEUCKRProber_h__
#define nsEUCKRProber_h__

#include "nsCharSetProber.h"
#include "nsCodingStateMachine.h"
#include "CharDistribution.h"

class nsEUCKRProber: public nsCharSetProber {
public:
  nsEUCKRProber(bool aIsPreferredLanguage)
    :mIsPreferredLanguage(aIsPreferredLanguage)
  {mCodingSM = new nsCodingStateMachine(&EUCKRSMModel);
    Reset();
  }
  virtual ~nsEUCKRProber(void){delete mCodingSM;}
  nsProbingState HandleData(const char* aBuf, PRUint32 aLen);
  const char* GetCharSetName() {return "EUC-KR";}
  nsProbingState GetState(void) {return mState;}
  void      Reset(void);
  float     GetConfidence(void);

protected:
  void      GetDistribution(PRUint32 aCharLen, const char* aStr);
  
  nsCodingStateMachine* mCodingSM;
  nsProbingState mState;

  //EUCKRContextAnalysis mContextAnalyser;
  EUCKRDistributionAnalysis mDistributionAnalyser;
  char mLastChar[2];
  bool mIsPreferredLanguage;

};


#endif /* nsEUCKRProber_h__ */

