/**
 * @file    ucdapi.cpp
 * @brief   charset detection functions
 * @author  Yunhui Fu (yhfudev@gmail.com)
 * @version 1.0
 * @date    2015-01-13
 * @license GPL 2.0/LGPL 2.1
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef _WIN32
#  include <windows.h>
#endif

#include "nscore.h"
#include "nsUniversalDetector.h"

#include "libucd.h"

class DllDetector : public nsUniversalDetector
{
protected:
    char *charset_;

public:
    DllDetector()
        : nsUniversalDetector()
    {
        charset_ = NULL;
    }

    virtual ~DllDetector()
    {
        if (charset_) free(charset_);
    }

    virtual void Report(const char* charset)
    {
        if (NULL != charset_) {
            free (charset_);
        }
        charset_ = strdup(charset);
    }

    virtual void Reset()
    {
        nsUniversalDetector::Reset();
        if (NULL != charset_) {
            free (charset_);
        }
        charset_ = NULL;
    }

    const char* GetCharset() const
    {
        return charset_;
    }
};

#ifdef _WIN32
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                                         )
{
    return TRUE;
}
#endif

UCD_IMEXPORT
int ucd_init (ucd_t * pdet)
{
    if (!pdet) return UCD_RESULT_NOMEMORY;

    *pdet = reinterpret_cast<ucd_t>(new DllDetector);
    if (*pdet) {
        return UCD_RESULT_OK;
    } else {
        return UCD_RESULT_NOMEMORY;
    }
}


UCD_IMEXPORT
void ucd_clear (ucd_t * det)
{
    assert (NULL != det);
    if (*det) {
        delete reinterpret_cast<DllDetector *>(*det);
    }
    *det = NULL;
}


UCD_IMEXPORT
int ucd_parse (ucd_t * det, const char* data, size_t len)
{
    assert (NULL != det);
    if (*det) {
        nsresult ret = reinterpret_cast<DllDetector *>(*det)->HandleData(data, (PRUint32)len);
        if (ret == NS_OK) {
            return UCD_RESULT_OK;
        }
        return UCD_RESULT_NOMEMORY;
    }
    return UCD_RESULT_INVALID_DETECTOR;
}


UCD_IMEXPORT
int ucd_end (ucd_t * det)
{
    assert (NULL != det);
    if (*det) {
        reinterpret_cast<DllDetector *>(*det)->DataEnd();
        return UCD_RESULT_OK;
    }
    return UCD_RESULT_INVALID_DETECTOR;
}


UCD_IMEXPORT
int ucd_reset(ucd_t * det)
{
    assert (NULL != det);
    if (*det) {
        reinterpret_cast<DllDetector *>(*det)->Reset();
        return UCD_RESULT_OK;
    }
    return UCD_RESULT_INVALID_DETECTOR;
}

UCD_IMEXPORT
int ucd_results (ucd_t * det, char* namebuf, size_t buflen)
{
    assert (NULL != det);
    if (*det) {
        if (!namebuf) return UCD_RESULT_NOMEMORY;

        const char* name = reinterpret_cast<DllDetector *>(*det)->GetCharset();
        if (name == NULL || *name == 0) {
            // could not detect encoding
#define CSTR_DEFAULT "ISO-8859-1"
            if (buflen > sizeof(CSTR_DEFAULT)-1) {
                //*namebuf = 0;
                strcpy (namebuf, CSTR_DEFAULT);
                return UCD_RESULT_OK;
            }
            return UCD_RESULT_NOMEMORY;
        }
        // encoding detected
        if (buflen >= strlen(name)+1) {
            strcpy(namebuf, name);
            return UCD_RESULT_OK;
        }
        return UCD_RESULT_NOMEMORY;
    }
    return UCD_RESULT_INVALID_DETECTOR;
}

