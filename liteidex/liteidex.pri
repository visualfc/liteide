!isEmpty(LITEIDE_PRI_INCLUDED):error("liteidex.pri already included")
LITEIDE_PRI_INCLUDED = 1

defineReplace(qtLibraryName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
              else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

isEmpty(IDE_LIBRARY_BASENAME) {
    IDE_LIBRARY_BASENAME = lib
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

DEFINES += LITEAPP_LIBRARY

DEFINES += IDE_LIBRARY_BASENAME=\\\"$$IDE_LIBRARY_BASENAME\\\"

IDE_APP_TARGET   = liteide
IDE_SOURCE_TREE = $$PWD
IDE_BUILD_TREE = $$IDE_SOURCE_TREE/liteide

IDE_APP_PATH = $$IDE_BUILD_TREE/bin

macx {
    IDE_APP_TARGET   = "LiteIDE"
    IDE_LIBRARY_PATH = $$IDE_APP_PATH/$${IDE_APP_TARGET}.app/Contents/PlugIns
    IDE_PLUGIN_PATH = $$IDE_LIBRARY_PATH
    IDE_DATA_PATH   = $$IDE_APP_PATH/$${IDE_APP_TARGET}.app/Contents/Resources
    IDE_BIN_PATH     = $$IDE_APP_PATH/$${IDE_APP_TARGET}.app/Contents/MacOS
    contains(QT_CONFIG, ppc):CONFIG += ppc x86
    copydata = 1
} else {
    win32 {
        contains(TEMPLATE, vc.*)|contains(TEMPLATE_PREFIX, vc):vcproj = 1
        IDE_APP_TARGET   = liteide
    } else {
        IDE_APP_TARGET   = liteide
    }
    IDE_BIN_PATH = $$IDE_BUILD_TREE/bin
    IDE_LIBRARY_PATH = $$IDE_BUILD_TREE/$$IDE_LIBRARY_BASENAME/liteide
    IDE_PLUGIN_PATH = $$IDE_LIBRARY_PATH/plugins
    IDE_DATA_PATH = $$IDE_BUILD_TREE/share/liteide
    !isEqual(IDE_SOURCE_TREE, $$IDE_BUILD_TREE):copydata = 1
}

#LIBS += -L$$IDE_LIBRARY_PATH

#DEFINES += QT_NO_CAST_FROM_ASCII
DEFINES += QT_NO_CAST_TO_ASCII
!macx:DEFINES += QT_USE_FAST_OPERATOR_PLUS QT_USE_FAST_CONCATENATION

unix {
    CONFIG(debug, debug|release):OBJECTS_DIR = $${OUT_PWD}/.obj/debug-shared
    CONFIG(release, debug|release):OBJECTS_DIR = $${OUT_PWD}/.obj/release-shared

    CONFIG(debug, debug|release):MOC_DIR = $${OUT_PWD}/.moc/debug-shared
    CONFIG(release, debug|release):MOC_DIR = $${OUT_PWD}/.moc/release-shared

    RCC_DIR = $${OUT_PWD}/.rcc
    UI_DIR = $${OUT_PWD}/.uic
}

linux-g++-* {
    # Bail out on non-selfcontained libraries. Just a security measure
    # to prevent checking in code that does not compile on other platforms.
    QMAKE_LFLAGS += -Wl,--allow-shlib-undefined -Wl,--no-undefined
}

win32-msvc* {
    #Don't warn about sprintf, fopen etc being 'unsafe'
    DEFINES += _CRT_SECURE_NO_WARNINGS
}

win32|macx {
    greaterThan(QT_MAJOR_VERSION, 4) {
    } else {
        DEFINES += LITEIDE_QTWEBKIT
    }
}

