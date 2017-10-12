/**
 * @file    libucd.h
 * @brief   charset detection functions
 * @author  Yunhui Fu (yhfudev@gmail.com)
 * @version 1.0
 * @date    2015-01-13
 * @license GPL 2.0/LGPL 2.1
 */

#ifndef __LIBUCD_H
#define __LIBUCD_H

#define UCD_IMEXPORT

//#ifdef _WIN32
//#  ifdef DLL_EXPORTS
//#    define UCD_IMEXPORT extern _declspec(dllexport)
//#  else
//#    define UCD_IMEXPORT extern _declspec(dllimport)
//#  endif
//#else
//#  define UCD_IMEXPORT extern __attribute__ ((visibility("default")))
//#endif

#include <stddef.h>

#define UCD_RESULT_OK               0
#define UCD_RESULT_NOMEMORY         (-1)
#define UCD_RESULT_INVALID_DETECTOR (-2)

#define UCD_MAX_ENCODING_NAME       64

#ifdef __cplusplus
extern "C" {
#endif

typedef void * ucd_t;

/**
 * Create an encoding detector.
 * @param pdet [out] pointer to a ucd_t variable that receives
 *             the encoding detector handle.
 * @return UCD_RESULT_OK if succeeded. UCD_RESULT_NOMEMORY otherwise.
 */
UCD_IMEXPORT int ucd_init (ucd_t * pdet);

/**
 * Destroy an encoding detector.
 * @param det [in] the encoding detector handle to be destroyed.
 */
UCD_IMEXPORT void ucd_clear (ucd_t * det);

/**
 * Feed data to an encoding detector.
 * @param det [in] the encoding detector handle
 * @param data [in] data
 * @param len [in] length of data in bytes.
 * @return UCD_RESULT_OK if succeeded.
 *         UCD_RESULT_NOMEMORY if running out of memory.
 *         UCD_RESULT_INVALID_DETECTOR if det was invalid.
 */
UCD_IMEXPORT int ucd_parse (ucd_t * det, const char* data, size_t len);

/**
 * Notify an end of data to an encoding detctor.
 * @param det [in] the encoding detector handle
 * @return UCD_RESULT_OK if succeeded.
 *         UCD_RESULT_INVALID_DETECTOR if det was invalid.
 */
UCD_IMEXPORT int ucd_end (ucd_t * det);

/**
 * Reset an encoding detector.
 * @param det [in] the encoding detector handle
 * @return UCD_RESULT_OK if succeeded.
 *         UCD_RESULT_INVALID_DETECTOR if det was invalid.
 */
UCD_IMEXPORT int ucd_reset (ucd_t * det);

/**
 * Get the name of encoding that was detected.
 * @param det [in] the encoding detector handle
 * @param namebuf [in/out] pointer to a buffer that receives the name of
 *                detected encoding. A valid encoding name or an empty string
 *                will be written to namebuf. If an empty strng was written,
 *                the detector could not detect any encoding.
 *                Written strings will always be NULL-terminated.
 * @param buflen [in] length of namebuf
 * @return UCD_RESULT_OK if succeeded.
 *         UCD_RESULT_NOMEMORY if namebuf was too small to store
 *         the entire encoding name.
 *         UCD_RESULT_INVALID_DETECTOR if det was invalid.
 */
UCD_IMEXPORT int ucd_results (ucd_t * det, char* namebuf, size_t buflen);

#ifdef __cplusplus
};
#endif

#endif /* __LIBUCD_H */
