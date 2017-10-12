
# public headers
include_HEADERS = \
    $(top_srcdir)/include/libucd.h \
    $(NULL)

noinst_HEADERS = \
    CharDistribution.h \
    nsCodingStateMachine.h \
    nsGB2312Prober.h \
    nsSBCSGroupProber.h \
    JpCntx.h \
    nsEUCJPProber.h \
    nsHebrewProber.h \
    nsSBCharSetProber.h \
    nsEUCKRProber.h \
    nsLatin1Prober.h \
    nsSJISProber.h \
    nsBig5Prober.h \
    nsEUCTWProber.h \
    nsMBCSGroupProber.h \
    nsUTF8Prober.h \
    nsCharSetProber.h \
    nsEscCharsetProber.h \
    nsPkgInt.h \
    nsUniversalDetector.h \
    nsDebug.h \
    prmem.h \
    nscore.h \
    Big5Freq.tab \
    EUCKRFreq.tab \
    EUCTWFreq.tab \
    GB2312Freq.tab \
    JISFreq.tab \
    $(NULL)

#EXTRA_DIST = libucd.pc.in
#pkgconfigdir = $(libdir)/pkgconfig
#pkgconfig_DATA = libucd.pc

lib_LTLIBRARIES=libucd.la

libucd_la_SOURCES = \
    ucdapi.cpp \
    CharDistribution.cpp \
    JpCntx.cpp \
    LangBulgarianModel.cpp \
    LangCyrillicModel.cpp \
    LangGreekModel.cpp \
    LangHebrewModel.cpp \
    LangHungarianModel.cpp \
    LangThaiModel.cpp \
    LangCzechModel.cpp \
    LangFinnishModel.cpp \
    LangFrenchModel.cpp \
    LangGermanModel.cpp \
    LangPolishModel.cpp \
    LangSpanishModel.cpp \
    LangSwedishModel.cpp \
    LangTurkishModel.cpp \
    nsBig5Prober.cpp \
    nsCharSetProber.cpp \
    nsEUCJPProber.cpp \
    nsEUCKRProber.cpp \
    nsEUCTWProber.cpp \
    nsEscCharsetProber.cpp \
    nsEscSM.cpp \
    nsGB2312Prober.cpp \
    nsHebrewProber.cpp \
    nsLatin1Prober.cpp \
    nsMBCSGroupProber.cpp \
    nsMBCSSM.cpp \
    nsSBCSGroupProber.cpp \
    nsSBCharSetProber.cpp \
    nsSJISProber.cpp \
    nsUTF8Prober.cpp \
    nsUniversalDetector.cpp \
    $(NULL)

libucd_la_CPPFLAGS = -I$(top_srcdir)/src/ -I$(top_srcdir)/include/

#DEFS+= -I$(top_srcdir)/src/ -I$(top_srcdir)/include/

#DEFS+=-DMEMWATCH

if DEBUG
DEFS+= -DDEBUG=1
endif

AM_CPPFLAGS=-g -Wall
