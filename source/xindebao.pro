#-------------------------------------------------
#
# Project created by QtCreator 2019-11-06T13:41:34
#
#-------------------------------------------------

QT       += core gui network serialport serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = xindebao
TEMPLATE = app
CONFIG += c++11

#PROROOT=/home/td/xindebaoProject
#OPENCV_HOME=/home/td/opencv320
#BOOST_HOME = /home/td/boost1640
#DAHENG_ROOT = /home/td/Galaxy_U2_V2.0.1_Linux_x86_64_CN
#GENICAM_ROOT_V2_3 = /home/td/Galaxy_U2_V2.0.1_Linux_x86_64_CN/sdk/genicam

PROROOT=/work/zxx/work_project/xindebaoProject
OPENCV_HOME=/SDK/opencv320
BOOST_HOME = /SDK/boost1640
DAHENG_ROOT = /SDK/Galaxy_Linux_x86_U2_CN1_0/Galaxy_U2
GENICAM_ROOT_V2_3 = /SDK/Galaxy_Linux_x86_U2_CN1_0/Galaxy_U2/sdk/genicam

#DAHENG_ROOT = /SDK/Galaxy_U2_V2.0.1_Linux_x86_64_CN
#GENICAM_ROOT_V2_3 = /SDK/Galaxy_U2_V2.0.1_Linux_x86_64_CN/sdk/genicam

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


#$${PROROOT}/include \

INCLUDEPATH += $${BOOST_HOME}/include \
               $${OPENCV_HOME}/include \
               $${DAHENG_ROOT}/sdk/include \
               $${GENICAM_ROOT_V2_3}/library/CPP/include \

LIBS += -L$${GENICAM_ROOT_V2_3}/bin/Linux64_x64/GenApi/Generic \
            -lGCBase_gcc40_v2_3 -lGenApi_gcc40_v2_3 -llog4cpp_gcc40_v2_3 -lLog_gcc40_v2_3 -lMathParser_gcc40_v2_3 \
        -L$${OPENCV_HOME}/lib \
            -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_tracking -lopencv_video -lopencv_dnn -lopencv_tracking \
        -L$${PROROOT}/lib \
        -L$${BOOST_HOME}/lib/release \
        -L$${DAHENG_ROOT}/sdk/lib \
        -L$${DAHENG_ROOT}/sdk/lib/pub \
            -ldhgentl -lgxiapi -ldximageproc   \ #-ldximageproc
        -L$${GENICAM_ROOT_V2_3}/bin/Linux64_x64 \
        -lboost_filesystem -lboost_system -lboost_date_time

#LIBS += -L/usr/local/lib -ljpeg
#    -L/usr/lib/x86_64-linux-gnu

SOURCES += \
    Setting/CamSetting.cpp \
        main.cpp \
        mainwindow.cpp \
    cameraControl.cpp \
    motor.cpp \
    src/Method/OpenConfig.cpp \
    utils.cpp \
    xdb.cc \
    Setting/MotorSetting.cpp \
    src/Widget/QCvDisplay.cpp \
    src/Method/QCvImageOperate.cpp \
    Setting/DebugDialog.cpp \
    src/Controls/QStatusLight.cpp \
    ransac.cc

HEADERS += \
    Setting/CamSetting.h \
        mainwindow.h \
    src/Method/OpenConfig.h \
    utils.h \
    xdb.h \
    Setting/MotorSetting.h \
    xdb.h \
    cameraControl.h \
    src/Error/error.h \
    src/Widget/QCvDisplay.h \
    src/Method/QCvImageOperate.h \
    Setting/DebugDialog.h \
    src/Controls/QStatusLight.h \
    ransac.h

FORMS += \
        Setting/CamSetting.ui \
        mainwindow.ui \
    Setting/MotorSetting.ui \
    Setting/DebugDialog.ui

RESOURCES += \
    res/res.qrc




win32 {
    QMAKE_POST_LINK += mt -manifest $$PWD/manifest.xml -outputresource:$$OUT_PWD/debug/$$TARGET".exe" $$escape_expand(\n\t)
    QMAKE_LFLAGS += /MANIFESTUAC:"level='requireAdministrator'uiAccess='false'"
}

DISTFILES += \
    menifest.xml \
    #qt.conf
