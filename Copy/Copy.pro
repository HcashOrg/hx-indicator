QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = Copy
#CONFIG += console
#CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    DataUtil.cpp \
    UpdateWidget.cpp

HEADERS += \
    DataUtil.h \
    UpdateWidget.h

FORMS += \
    UpdateWidget.ui

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

RESOURCES += \
    resource.qrc

INCLUDEPATH += $$PWD/quazip/quazip


win32{
    LIBS += $$PWD/lib/quazip.lib
}
