/**
 * @file    nsUTF8Prober.h
 * @brief   nsUTF8Prober
 * @license GPL 2.0/LGPL 2.1
 */


#ifndef nsUTF8Prober_h__
#define nsUTF8Prober_h__

#include "nsCharSetProber.h"
#include "nsCodingStateMachine.h"

class nsUTF8Prober: public nsCharSetProber {
public:
  nsUTF8Prober(){mNumOfMBChar = 0; 
                mCodingSM = new nsCodingStateMachine(&UTF8SMModel);
                Reset(); }
  virtual ~nsUTF8Prober(){delete mCodingSM;}
  nsProbingState HandleData(const char* aBuf, PRUint32 aLen);
  const char* GetCharSetName() {return "UTF-8";}
  nsProbingState GetState(void) {return mState;}
  void      Reset(void);
  float     GetConfidence(void);

protected:
  nsCodingStateMachine* mCodingSM;
  nsProbingState mState;
  PRUint32 mNumOfMBChar;
};

#endif /* nsUTF8Prober_h__ */

