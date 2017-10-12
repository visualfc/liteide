/**
 * @file    nsSBCharSetProber.h
 * @brief   nsSBCharSetProber
 * @license GPL 2.0/LGPL 2.1
 */

#ifndef nsSingleByteCharSetProber_h__
#define nsSingleByteCharSetProber_h__

#include "nsCharSetProber.h"

#define SAMPLE_SIZE 64
#define SB_ENOUGH_REL_THRESHOLD  1024
#define POSITIVE_SHORTCUT_THRESHOLD  (float)0.95
#define NEGATIVE_SHORTCUT_THRESHOLD  (float)0.05
#define SYMBOL_CAT_ORDER  250
#define NUMBER_OF_SEQ_CAT 4
#define POSITIVE_CAT   (NUMBER_OF_SEQ_CAT-1)
#define NEGATIVE_CAT   0

typedef struct
{
  const unsigned char* const charToOrderMap;    // [256] table use to find a char's order
  const PRUint8* const precedenceMatrix;  // [SAMPLE_SIZE][SAMPLE_SIZE]; table to find a 2-char sequence's frequency
  float  mTypicalPositiveRatio;     // = freqSeqs / totalSeqs 
  bool keepEnglishLetter;         // says if this script contains English characters (not implemented)
  const char* const charsetName;
  const char* const langName;
} SequenceModel;


class nsSingleByteCharSetProber : public nsCharSetProber{
public:
  nsSingleByteCharSetProber(const SequenceModel *model) 
    :mModel(model), mReversed(false), mNameProber(0) { Reset(); }
  nsSingleByteCharSetProber(const SequenceModel *model, bool reversed, nsCharSetProber* nameProber)
    :mModel(model), mReversed(reversed), mNameProber(nameProber) { Reset(); }

  virtual const char* GetCharSetName();
  virtual const char* GetLangName() {return mModel?mModel->langName:"";}
  virtual nsProbingState HandleData(const char* aBuf, PRUint32 aLen);
  virtual nsProbingState GetState(void) {return mState;}
  virtual void      Reset(void);
  virtual float     GetConfidence(void);

  virtual PRBool KeepEnglishLetters() {return mModel->keepEnglishLetter;}

#ifdef DEBUG_chardet
  virtual void  DumpStatus();
#endif

protected:
  nsProbingState mState;
  const SequenceModel* const mModel;
  const bool mReversed; // true if we need to reverse every pair in the model lookup

  //char order of last character
  unsigned char mLastOrder;

  PRUint32 mTotalSeqs;
  PRUint32 mSeqCounters[NUMBER_OF_SEQ_CAT];

  PRUint32 mTotalChar;
  //characters that fall in our sampling range
  PRUint32 mFreqChar;
  
  // Optional auxiliary prober for name decision. created and destroyed by the GroupProber
  nsCharSetProber* mNameProber; 

};


extern const SequenceModel Koi8rModel;
extern const SequenceModel Win1251Model;
extern const SequenceModel Latin5Model;
extern const SequenceModel MacCyrillicModel;
extern const SequenceModel Ibm866Model;
extern const SequenceModel Ibm855Model;
extern const SequenceModel ISO_8859_7greekModel;
extern const SequenceModel WINDOWS_1253greekModel;
extern const SequenceModel Latin5BulgarianModel;
extern const SequenceModel Win1251BulgarianModel;
extern const SequenceModel Latin2HungarianModel;
extern const SequenceModel Win1250HungarianModel;
extern const SequenceModel Win1255Model;
extern const SequenceModel TIS620ThaiModel;
extern const SequenceModel WINDOWS_1252frenchModel;
extern const SequenceModel WINDOWS_1252germanModel;
extern const SequenceModel WINDOWS_1252swedishModel;
extern const SequenceModel ISO_8859_9turkishModel;
extern const SequenceModel WINDOWS_1252finnishModel;
extern const SequenceModel windows_1252spanishModel;
extern const SequenceModel iso_8859_2czechModel;
extern const SequenceModel iso_8859_2polishModel;

#endif /* nsSingleByteCharSetProber_h__ */

