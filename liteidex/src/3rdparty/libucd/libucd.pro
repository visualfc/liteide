TARGET = libucd
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)

INCLUDEPATH += include

SOURCES += src/CharDistribution.cpp \
    src/JpCntx.cpp \
    src/LangBulgarianModel.cpp \
    src/LangCyrillicModel.cpp \
    src/LangCzechModel.cpp \
    src/LangFinnishModel.cpp \
    src/LangFrenchModel.cpp \
    src/LangGermanModel.cpp \
    src/LangGreekModel.cpp \
    src/LangHebrewModel.cpp \
    src/LangHungarianModel.cpp \
    src/LangPolishModel.cpp \
    src/LangSpanishModel.cpp \
    src/LangSwedishModel.cpp \
    src/LangThaiModel.cpp \
    src/LangTurkishModel.cpp \
    src/nsBig5Prober.cpp \
    src/nsCharSetProber.cpp \
    src/nsEscCharsetProber.cpp \
    src/nsEscSM.cpp \
    src/nsEUCJPProber.cpp \
    src/nsEUCKRProber.cpp \
    src/nsEUCTWProber.cpp \
    src/nsGB2312Prober.cpp \
    src/nsHebrewProber.cpp \
    src/nsLatin1Prober.cpp \
    src/nsMBCSGroupProber.cpp \
    src/nsMBCSSM.cpp \
    src/nsSBCharSetProber.cpp \
    src/nsSBCSGroupProber.cpp \
    src/nsSJISProber.cpp \
    src/nsUniversalDetector.cpp \
    src/nsUTF8Prober.cpp \
    src/ucdapi.cpp

DISTFILES += \
    src/Big5Freq.tab \
    src/EUCKRFreq.tab \
    src/EUCTWFreq.tab \
    src/GB2312Freq.tab \
    src/JISFreq.tab \
    src/Makefile.am

HEADERS += \
    src/CharDistribution.h \
    src/JpCntx.h \
    src/nsBig5Prober.h \
    src/nsCharSetProber.h \
    src/nsCodingStateMachine.h \
    src/nscore.h \
    src/nsDebug.h \
    src/nsEscCharsetProber.h \
    src/nsEUCJPProber.h \
    src/nsEUCKRProber.h \
    src/nsEUCTWProber.h \
    src/nsGB2312Prober.h \
    src/nsHebrewProber.h \
    src/nsLatin1Prober.h \
    src/nsMBCSGroupProber.h \
    src/nsPkgInt.h \
    src/nsSBCharSetProber.h \
    src/nsSBCSGroupProber.h \
    src/nsSJISProber.h \
    src/nsUniversalDetector.h \
    src/nsUTF8Prober.h \
    src/prmem.h \
    include/libucd.h
