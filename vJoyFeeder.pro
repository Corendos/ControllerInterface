TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
SOURCES += main.cpp \
    controller.cpp

HEADERS += \
    controller.h

include(deployment.pri)
include(3rdparty/qextserialport/src/qextserialport.pri)
qtcAddDeployment()

LIBS += -L"D:/Perso/Programmation/Librairies/vJoy/SDK/lib" -lvjoyinterface
INCLUDEPATH += "D:/Perso/Programmation/Librairies/vJoy/SDK/inc"

QMAKE_CXXFLAGS += -std=c++11
