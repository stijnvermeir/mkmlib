#-------------------------------------------------
#
# Project created by QtCreator 2015-10-23T17:58:46
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = mkm
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

SOURCES += \
    src/mkm.cpp \
    src/parser.cpp \
    src/exception.cpp

HEADERS += \
    mkm/mkm.h \
    mkm/product.h \
    mkm/parser.h \
    mkm/wantslist.h \
    mkm/want.h \
    mkm/exception.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
