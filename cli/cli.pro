TEMPLATE = app

QT -= gui
QT += core network 

CONFIG += c++17 console

SOURCES += \
    ../core/hub.cpp \
    main.cpp

HEADERS += \
    ../core/hub.h