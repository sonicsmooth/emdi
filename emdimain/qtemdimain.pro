QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/documents.cpp \
    src/main.cpp \
    src/mainwindow.cpp \


HEADERS += \
    include/documents.h \
    include/mainwindow.h \

INCLUDEPATH += \
    include \
    ../emdilib/include

FORMS += \
    ui/mainwindow.ui

TARGET = emdimain

debug:   DESTDIR = debug
release: DESTDIR = release


DESTDIR     = $$DESTDIR
OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR     = $$DESTDIR/moc
RCC_DIR     = $$DESTDIR/rcc
UI_DIR      = $$DESTDIR/ui


CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../emdilib/build-qtemdilib-Desktop_Qt_5_12_6_MSVC2017_64bit-Debug/debug/ -lemdilib
    PRE_TARGETDEPS += $$PWD/../emdilib/build-qtemdilib-Desktop_Qt_5_12_6_MSVC2017_64bit-Debug/debug/emdilib.lib
    emdimain.depends = emdilib
}


CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../emdilib/build-qtemdilib-Desktop_Qt_5_12_6_MSVC2017_64bit-Release/release/ -lemdilib
    PRE_TARGETDEPS += $$PWD/../emdilib/build-qtemdilib-Desktop_Qt_5_12_6_MSVC2017_64bit-Release/release/emdilib.lib
    emdimain.depends = emdilib
}





# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


