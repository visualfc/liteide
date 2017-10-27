/**
 * @file    nsMBCSGroupProber.h
 * @brief   nsMBCSGroupProber
 * @license GPL 2.0/LGPL 2.1
 */

#ifndef nsMBCSGroupProber_h__
#define nsMBCSGroupProber_h__

#include "nsSJISProber.h"
#include "nsUTF8Prober.h"
#include "nsEUCJPProber.h"
#include "nsGB2312Prober.h"
#include "nsEUCKRProber.h"
#include "nsBig5Prober.h"
#include "nsEUCTWProber.h"

#define NUM_OF_PROBERS    7

class nsMBCSGroupProber: public nsCharSetProber {
public:
  nsMBCSGroupProber(PRUint32 aLanguageFilter);
  virtual ~nsMBCSGroupProber();
  nsProbingState HandleData(const char* aBuf, PRUint32 aLen);
  const char* GetCharSetName();
  nsProbingState GetState(void) {return mState;}
  void      Reset(void);
  float     GetConfidence(void);

#ifdef DEBUG_chardet
  void  DumpStatus();
#endif
#ifdef DEBUG_jgmyers
  void GetDetectorState(nsUniversalDetector::DetectorState (&states)[nsUniversalDetector::NumDetectors], PRUint32 &offset);
#endif

protected:
  nsProbingState mState;
  nsCharSetProber* mProbers[NUM_OF_PROBERS];
  bool            mIsActive[NUM_OF_PROBERS];
  PRInt32 mBestGuess;
  PRUint32 mActiveNum;
  PRUint32 mKeepNext;
};

#endif /* nsMBCSGroupProber_h__ */

