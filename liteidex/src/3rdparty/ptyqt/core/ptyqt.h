#ifndef PTYQT_H
#define PTYQT_H

#include "iptyprocess.h"

class PtyQt
{
public:
    static IPtyProcess *createPtyProcess(IPtyProcess::PtyType ptyType);
};

#endif // PTYQT_H
