/**
 * @file    nscore.h
 * @brief   fake nscore.h
 * @author  Yunhui Fu (yhfudev@gmail.com)
 * @version 1.0
 * @date    2015-01-13
 * @license GPL 2.0/LGPL 2.1
 */

#ifndef __NS_FAKE_CORE_H
#define __NS_FAKE_CORE_H

#include <stdint.h>
#include <stddef.h>

typedef bool     PRBool;
typedef int32_t  PRInt32;
typedef uint32_t PRUint32;
typedef int16_t  PRInt16;
typedef uint16_t PRUint16;
typedef int8_t   PRInt8;
typedef uint8_t  PRUint8;

enum nsresult {
    NS_OK,
    NS_ERROR_OUT_OF_MEMORY
};

#define PR_FALSE false
#define PR_TRUE  true
#define nsnull   0

#define NS_ARRAY_LENGTH(a) (sizeof(a)/sizeof((a)[0]))

#endif /* __NS_FAKE_CORE_H */
