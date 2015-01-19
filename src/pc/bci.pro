#------------------------------------------------#
#                                                #
# Brain Controlled Scalextrix Interface Code     #
#                                                #
# Produced by the Warwick Biomedical Engineering #
# Outreach Group at the University of Warwick    #
#                                                #
#                                                #
# Software Released Under LGPL-v2.1              #
#                                                #
#------------------------------------------------#

QT       += core serialport
QT       -= gui

CONFIG += C++11

TARGET    = bci-app
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
        main.cpp \
        mindwavecontroller.cpp \
        arduinointerface.cpp

HEADERS += \
        mindwavecontroller.h \
        arduinointerface.h \
        defines.h
