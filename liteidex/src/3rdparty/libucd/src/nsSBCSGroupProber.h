/**
 * @file    nsSBCSGroupProber.h
 * @brief   nsSBCSGroupProber
 * @license GPL 2.0/LGPL 2.1
 */

#ifndef nsSBCSGroupProber_h__
#define nsSBCSGroupProber_h__


#define NUM_OF_SBCS_PROBERS    24

class nsCharSetProber;
class nsSBCSGroupProber: public nsCharSetProber {
public:
  nsSBCSGroupProber();
  virtual ~nsSBCSGroupProber();
  nsProbingState HandleData(const char* aBuf, PRUint32 aLen);
  const char* GetCharSetName();
  nsProbingState GetState(void) {return mState;}
  void      Reset(void);
  float     GetConfidence(void);

#ifdef DEBUG_chardet
  void  DumpStatus();
#endif

protected:
  nsProbingState mState;
  nsCharSetProber* mProbers[NUM_OF_SBCS_PROBERS];
  bool            mIsActive[NUM_OF_SBCS_PROBERS];
  PRInt32 mBestGuess;
  PRUint32 mActiveNum;
};

#endif /* nsSBCSGroupProber_h__ */

