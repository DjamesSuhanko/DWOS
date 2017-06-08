#-------------------------------------------------
#
# Project created by QtCreator 2014-10-04T17:30:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DWatson
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    common.cpp \
    wiping.cpp \
    systeminfo.cpp \
    gptandformat.cpp \
    bitwisecopy.cpp \
    graphrealtime.cpp \
    popup.cpp \
    fingerprintandtraces.cpp \
    documentation.cpp \
    screenshot.cpp \
    screencast.cpp \
    progressbarundefinedvalue.cpp \
    autorenameshot.cpp \
    ewfrelat.cpp \
    expertwitnessformat.cpp \
    encrypt.cpp \
    iostats.cpp

HEADERS  += mainwindow.h \
    common.h \
    wiping.h \
    systeminfo.h \
    gptandformat.h \
    bitwisecopy.h \
    graphrealtime.h \
    popup.h \
    fingerprintandtraces.h \
    documentation.h \
    screenshot.h \
    screencast.h \
    progressbarundefinedvalue.h \
    autorenameshot.h \
    ewfrelat.h \
    expertwitnessformat.h \
    encrypt.h \
    iostats.h

FORMS    += mainwindow.ui \
    popup.ui \
    ewfrelat.ui

RESOURCES += \
    logo.qrc

TRANSLATIONS = resources/i18n/dwos_en.ts \
               resources/i18n/dwos_es.ts


OTHER_FILES += \
               resources/bandeiras/espanha.jpg \
               resources/bandeiras/eua.png \
    resources/i18n/dwos_en.ts \
    resources/i18n/dwos_es.ts \
    resources/bandeiras/brasil.png \
    resources/bandeiras/espanha.png \
    resources/lupa.png

