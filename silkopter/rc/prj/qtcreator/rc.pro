#-------------------------------------------------
#
# Project created by QtCreator 2014-06-04T18:33:20
#
#-------------------------------------------------

QT       += core quick qml quickcontrols2

TARGET = rc
TEMPLATE = app

TARGET = rc
target.path = rc
INSTALLS = target

#qmlFiles.path = rc/data
#qmlFiles.files = ../../src/qml/*
#qmlFiles.files = ../../src/NodeEditor/qml/*
#INSTALLS += qmlFiles


CONFIG += c++11

INCLUDEPATH += ../../src
INCLUDEPATH += ../../../../qbase/include
INCLUDEPATH += ../../../../qdata/include
INCLUDEPATH += ../../../../qmath/include
INCLUDEPATH += ../../../../qinput/include
INCLUDEPATH += ../../../../def_lang/include
INCLUDEPATH += ../../../libs
INCLUDEPATH += ../../../../eigen
INCLUDEPATH += ../../../libs/common/comms/def


QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-parameter
QMAKE_CFLAGS += -Wno-unused-variable -Wno-unused-parameter

PRECOMPILED_HEADER = ../../src/stdafx.h
CONFIG *= precompile_header

ROOT_LIBS_PATH = ../../../..

rpi {
    DEFINES+=RASPBERRY_PI
    QMAKE_MAKEFILE = "Makefile.rpi"
    MAKEFILE = "Makefile.rpi"
    CONFIG(debug, debug|release) {
        DEST_FOLDER = rpi/debug
    }
    CONFIG(release, debug|release) {
        DEST_FOLDER = rpi/release
        DEFINES += NDEBUG
    }
    LIBS += -lmmal_core
    LIBS += -lmmal_util
    LIBS += -lmmal_vc_client
    LIBS += -lvcos
    LIBS += -lbcm_host
    LIBS += -lGLESv2
    LIBS += -lEGL
} else {
    QMAKE_MAKEFILE = "Makefile"
    CONFIG(debug, debug|release) {
        DEST_FOLDER = pc/debug
    }
    CONFIG(release, debug|release) {
        DEST_FOLDER = pc/release
        DEFINES += NDEBUG
    }
    LIBS += -lavutil -lavcodec -lavformat -lswscale
}

LIBS += -lfreetype
LIBS += -lboost_system -lboost_thread
LIBS += -lfftw3 -lpcap -lz
LIBS += -L$${ROOT_LIBS_PATH}/qinput/lib/$${DEST_FOLDER} -lqinput
LIBS += -L$${ROOT_LIBS_PATH}/qdata/lib/$${DEST_FOLDER} -lqdata
LIBS += -L$${ROOT_LIBS_PATH}/qmath/lib/$${DEST_FOLDER} -lqmath
LIBS += -L$${ROOT_LIBS_PATH}/qbase/lib/$${DEST_FOLDER} -lqbase
LIBS += -L$${ROOT_LIBS_PATH}/def_lang/lib/$${DEST_FOLDER} -ldef_lang

OBJECTS_DIR = ./.obj/$${DEST_FOLDER}
MOC_DIR = ./.moc/$${DEST_FOLDER}
RCC_DIR = ./.rcc/$${DEST_FOLDER}
UI_DIR = ./.ui/$${DEST_FOLDER}
DESTDIR = ../../bin

RESOURCES += \
    ../../src/res.qrc

HEADERS += \
    ../../src/Comms.h \
    ../../src/HAL.h \
    ../../src/stdafx.h \
    ../../../libs/common/Comm_Data.h \
    ../../../libs/utils/Butterworth.h \
    ../../../libs/utils/Channel.h \
    ../../../libs/utils/PID.h \
    ../../../libs/utils/Serial_Channel.h \
    ../../../libs/utils/Timed_Scope.h \
    ../../../libs/common/bus/IBus.h \
    ../../../libs/common/bus/II2C.h \
    ../../../libs/common/bus/ISPI.h \
    ../../../libs/common/bus/IUART.h \
    ../../../libs/common/stream/IAcceleration.h \
    ../../../libs/common/stream/IADC.h \
    ../../../libs/common/stream/IAngular_Velocity.h \
    ../../../libs/common/stream/IBattery_State.h \
    ../../../libs/common/stream/ICurrent.h \
    ../../../libs/common/stream/IDistance.h \
    ../../../libs/common/stream/IForce.h \
    ../../../libs/common/stream/IFrame.h \
    ../../../libs/common/stream/ILinear_Acceleration.h \
    ../../../libs/common/stream/IMagnetic_Field.h \
    ../../../libs/common/stream/IPressure.h \
    ../../../libs/common/stream/IPWM.h \
    ../../../libs/common/stream/IStream.h \
    ../../../libs/common/stream/ITemperature.h \
    ../../../libs/common/stream/IThrottle.h \
    ../../../libs/common/stream/ITorque.h \
    ../../../libs/common/stream/IVelocity.h \
    ../../../libs/common/stream/IVideo.h \
    ../../../libs/common/stream/IVoltage.h \
    ../../../libs/common/node/IController.h \
    ../../../libs/common/node/IGenerator.h \
    ../../../libs/common/node/ILPF.h \
    ../../../libs/common/node/IMultirotor_Simulator.h \
    ../../../libs/common/node/INode.h \
    ../../../libs/common/node/IPilot.h \
    ../../../libs/common/node/IProcessor.h \
    ../../../libs/common/node/IResampler.h \
    ../../../libs/common/node/ISink.h \
    ../../../libs/common/node/ISource.h \
    ../../../libs/common/node/ITransformer.h \
    ../../../libs/common/stream/IBool.h \
    ../../../libs/common/stream/IPosition.h \
    ../../../libs/common/stream/IGPS_Info.h \
    ../../../libs/lz4/lz4.h \
    ../../../libs/common/stream/IFloat.h \
    ../../../libs/common/stream/IMultirotor_Commands.h \
    ../../../libs/common/stream/IMultirotor_State.h \
    ../../../libs/common/stream/IProximity.h \
    ../../../libs/common/stream/Stream_Base.h \
    ../../../libs/utils/Coordinates.h \
    ../../../libs/common/node/ICombiner.h \
    ../../src/Nodes.h \
    ../../../libs/common/stream/IMultirotor_Commands.h \
    ../../../libs/common/stream/IMultirotor_State.h \
    ../../src/Comms_Slots.h \
    ../../src/OS_QML_Proxy.h \
    ../../src/Menus_QML_Proxy.h \
    ../../src/Comms_QML_Proxy.h \
    ../../src/HAL_QML_Proxy.h \
    ../../src/Video_Decoder.h \
    ../../src/Video_Renderer.h \
    ../../../libs/utils/hw/bcm2835.h \
    ../../../libs/utils/hw/command.h \
    ../../../libs/utils/hw/pigpio.h \
    ../../../libs/utils/comms/Channel.h \
    ../../../libs/utils/comms/RC.h \
    ../../../libs/utils/comms/RCP.h \
    ../../../libs/utils/comms/ISocket.h \
    ../../../libs/utils/comms/RF4463F30_Socket.h \
    ../../../libs/utils/comms/RFMON_Socket.h \
    ../../../libs/utils/comms/UDP_Socket.h \
    ../../../libs/utils/hw/RF4463F30.h \
    ../../../libs/utils/hw/Si4463.h \
    ../../../libs/utils/hw/SPI_Dev.h \
    ../../../libs/utils/hw/RF4463F30_Config_old.h \
    ../../../libs/utils/hw/RF4463F30_Config.h \
    ../../../libs/utils/hw/si446x_patch.h \
    ../../../libs/utils/hw/radiotap/ieee80211_radiotap.h \
    ../../../libs/utils/hw/radiotap/radiotap.h

SOURCES += \
    ../../src/main.cpp \
    ../../src/Comms.cpp \
    ../../src/HAL.cpp \
    ../../../libs/lz4/lz4.c \
    ../../../libs/utils/Coordinates.cpp \
    ../../src/Comms_Slots.cpp \
    ../../src/Comms_QML_Proxy.cpp \
    ../../src/Menus_QML_Proxy.cpp \
    ../../src/OS_QML_Proxy.cpp \
    ../../src/HAL_QML_Proxy.cpp \
    ../../src/Video_Decoder.cpp \
    ../../src/Video_Renderer.cpp \
    ../../../libs/utils/hw/bcm2835.c \
    ../../../libs/utils/hw/command.c \
    ../../../libs/utils/hw/pigpio.c \
    ../../../libs/utils/comms/RC.cpp \
    ../../../libs/utils/comms/RCP.cpp \
    ../../../libs/utils/comms/RF4463F30_Socket.cpp \
    ../../../libs/utils/comms/RFMON_Socket.cpp \
    ../../../libs/utils/comms/UDP_Socket.cpp \
    ../../../libs/utils/hw/RF4463F30.cpp \
    ../../../libs/utils/hw/Si4463.cpp \
    ../../../libs/utils/hw/SPI_Dev.cpp \
    ../../../libs/utils/hw/radiotap/radiotap.cpp

DISTFILES += \
    ../../src/node.png \
    ../../src/pilot.png \
    ../../src/processor.png \
    ../../src/resampler.png \
    ../../src/sink.png \
    ../../src/source.png \
    ../../src/lpf.png
