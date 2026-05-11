TEMPLATE = app
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Arrera_Hub

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../core/hub.cpp \
    ../update_demon/ctigerdemon.cpp \
    arrera_qt/apushbutton.cpp \
    arrera_qt/arrera_theme.cpp \
    arrera_qt/roundedframe.cpp \
    main.cpp \
    hub_gui.cpp

HEADERS += \
    ../core/hub.h \
    ../update_demon/ctigerdemon.h \
    arrera_qt/apushbutton.h \
    arrera_qt/arrera_theme.h \
    arrera_qt/roundedframe.h \
    hub_gui.h

FORMS += \
    hub_gui.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

macx{
    ICON = asset/arrera_hub.icns
}

RESOURCES += \
    ressource.qrc
