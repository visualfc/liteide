/**
 * @file    nsLatin1Prober.h
 * @brief   nsLatin1Prober
 * @license GPL 2.0/LGPL 2.1
 */

#ifndef nsLatin1Prober_h__
#define nsLatin1Prober_h__

#include "nsCharSetProber.h"

#define FREQ_CAT_NUM    4

class nsLatin1Prober: public nsCharSetProber {
public:
  nsLatin1Prober(void){Reset();}
  virtual ~nsLatin1Prober(void){}
  nsProbingState HandleData(const char* aBuf, PRUint32 aLen);
  const char* GetCharSetName() {return "windows-1252";}
  nsProbingState GetState(void) {return mState;}
  void      Reset(void);
  float     GetConfidence(void);

#ifdef DEBUG_chardet
  virtual void  DumpStatus();
#endif

protected:
  
  nsProbingState mState;
  char mLastCharClass;
  PRUint32 mFreqCounter[FREQ_CAT_NUM];
};


#endif /* nsLatin1Prober_h__ */

