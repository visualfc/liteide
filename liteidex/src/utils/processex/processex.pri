LIBS *= -l$$qtLibraryName(processex)
win32 {
    LIBS += -luser32 -lshell32
}



