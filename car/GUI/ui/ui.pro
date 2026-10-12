QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
# 编译器参数
#TOOLCHAIN_DIR = /home/book/100ask_imx6ull-sdk/Buildroot_2020.02.x/output/host
#QMAKE_CC  = $${TOOLCHAIN_DIR}/bin/arm-linux-gnueabihf-gcc
#QMAKE_CXX = $${TOOLCHAIN_DIR}/bin/arm-linux-gnueabihf-g++
#QMAKE_LINK = $${QMAKE_CXX}

QMAKE_CFLAGS   += -march=armv7ve -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a7 \
                  -Wno-missing-field-initializers -Wno-unused-variable -Wno-sign-compare \
                  -g -O3

QMAKE_CXXFLAGS += $${QMAKE_CFLAGS}

SOURCES += \
    backupview.cpp \
    hare_widget.cpp \
    main.cpp \
    mainwindow.cpp \
    menu_widget.cpp \
    ../hardware/led.cpp \
    ../hardware/ap3216c.cpp \
    ../hardware/V4L2Camera.cpp
HEADERS += \
    backupview.h \
    common.h \
    hare_widget.h \
    mainwindow.h \
    menu_widget.h
INCLUDEPATH += \
    /home/book/Desktop/car/GUI/plog/include \
    /home/book/Desktop/car/GUI/hardware \

FORMS += \
    backupview.ui \
    hare_widget.ui \
    mainwindow.ui \
    menu_widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
