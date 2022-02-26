QT       += core gui
QT       += network
QT       += webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    compass.cpp \
    joystick.c \
    listop.c \
    main.cpp \
    mainwindow.cpp \
    mqtt_c.cpp \
    mykeyevent.cpp \
    senddata.cpp

HEADERS += \
    compass.h \
    joystick.h \
    listop.h \
    mainwindow.h \
    mqtt_c.h \
    mykeyevent.h \
    senddata.h

FORMS += \
    compass.ui \
    mainwindow.ui


CONFIG += lrelease
CONFIG += embed_translations


LIBS       += -L /home/haha/Desktop/ah8301/code/camera/remote/lib -lVLCQtCore -lVLCQtWidgets -lqmqtt
INCLUDEPATH +=/home/haha/Desktop/ah8301/code/camera/remote/include
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Image/res.qrc \
    Image/res.qrc
