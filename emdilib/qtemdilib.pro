QT += core gui widgets sql

TEMPLATE = lib
DEFINES += EMDILIB_LIBRARY

CONFIG += c++17
CONFIG += staticlib

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/dbclone.cpp \
    src/emdilib.cpp

HEADERS += \
    include/docthreadwrapper.h \
    include/emdilib.h \
    include/emdilib_global.h \
    include/idocument.h \
    include/dbclone.h

INCLUDEPATH += include

TARGET = emdilib

debug:DESTDIR = debug
release:DESTDIR = release

DESTDIR     = $$DESTDIR
OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR     = $$DESTDIR/moc
RCC_DIR     = $$DESTDIR/rcc
UI_DIR      = $$DESTDIR/ui


# Default rules for deployment.
unix:target.path = /usr/lib
!isEmpty(target.path): INSTALLS += target
