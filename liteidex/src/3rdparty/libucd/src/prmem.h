/**
 * @file    prmem.h
 * @brief   fake prmem.h
 * @author  Yunhui Fu (yhfudev@gmail.com)
 * @version 1.0
 * @date    2015-01-13
 * @license GPL 2.0/LGPL 2.1
 */
#ifndef __NS_FAKE_PRMEM_H
#define __NS_FAKE_PRMEM_H

#include <stdlib.h>

#define PR_Malloc(length) malloc(length)
#define PR_FREEIF(p) do { if (p) free(p); } while(0)

#endif /* __NS_FAKE_PRMEM_H */
