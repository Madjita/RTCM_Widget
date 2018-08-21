#-------------------------------------------------
#
# Project created by QtCreator 2018-08-20T08:14:19
#
#-------------------------------------------------

QT       += core gui widgets serialport

TARGET = RTCM_Widget
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    port.cpp \
    rtcm.cpp \
    doublecaptiontable.cpp \
    header2_tebleview.cpp \
    message31_tebleview.cpp \
    message31header_tableview.cpp \
    message3_tebleview.cpp \
    message3header_tebleview.cpp

HEADERS += \
        mainwindow.h \
    port.h \
    rtcm.h \
    doublecaptiontable.h \
    header2_tebleview.h \
    message31_tebleview.h \
    message31header_tableview.h \
    message3_tebleview.h \
    message3header_tebleview.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
