/**
 * @file    nsCharSetProber.h
 * @brief   nsCharSetProber
 * @license GPL 2.0/LGPL 2.1
 */

#ifndef nsCharSetProber_h__
#define nsCharSetProber_h__

#include "nscore.h"

//#define DEBUG_chardet // Uncomment this for debug dump.

typedef enum {
  eDetecting = 0,   //We are still detecting, no sure answer yet, but caller can ask for confidence.
  eFoundIt = 1,     //That's a positive answer
  eNotMe = 2        //Negative answer
} nsProbingState;

#define SHORTCUT_THRESHOLD      (float)0.95

class nsCharSetProber {
public:
  virtual ~nsCharSetProber() {}
  virtual const char* GetCharSetName() = 0;
  virtual const char* GetLangName() {return "";};
  virtual PRBool KeepEnglishLetters() {return PR_TRUE;}
  virtual nsProbingState HandleData(const char* aBuf, PRUint32 aLen) = 0;
  virtual nsProbingState GetState(void) = 0;
  virtual void      Reset(void)  = 0;
  virtual float     GetConfidence(void) = 0;

#ifdef DEBUG_chardet
  virtual void  DumpStatus() {};
#endif

  // Helper functions used in the Latin1 and Group probers.
  // both functions Allocate a new buffer for newBuf. This buffer should be 
  // freed by the caller using PR_FREEIF.
  // Both functions return false in case of memory allocation failure.
  static bool FilterWithoutEnglishLetters(const char* aBuf, PRUint32 aLen, char** newBuf, PRUint32& newLen);
  static bool FilterWithEnglishLetters(const char* aBuf, PRUint32 aLen, char** newBuf, PRUint32& newLen);

};

#endif /* nsCharSetProber_h__ */
