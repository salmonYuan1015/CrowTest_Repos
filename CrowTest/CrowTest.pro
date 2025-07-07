QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# influxdb需要添加
DEFINES += INFLUXDB_WITH_BOOST
DEFINES += ONLY_HTTP
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Interface/influxclient.cpp \
    blue_data.cpp \
    influxdb/HTTP.cxx \
    influxdb/InfluxDB.cxx \
    influxdb/InfluxDBFactory.cxx \
    influxdb/LineProtocol.cxx \
    influxdb/Point.cxx \
    influxdb/Proxy.cxx \
    main.cpp \
    mainwindow.cpp \
    test.cpp \
    utils.cpp

HEADERS += \
    Interface/influxclient.h \
    blue_data.h \
    crow.h \
    influxdb/HTTP.h \
    influxdb/LineProtocol.h \
    influxdb/UriParser.h \
    influxdb/date/date.h \
    influxdb/include/InfluxDB.h \
    influxdb/include/InfluxDBException.h \
    influxdb/include/InfluxDBFactory.h \
    influxdb/include/Point.h \
    influxdb/include/Proxy.h \
    influxdb/include/Transport.h \
    mainwindow.h \
    utils.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

## curl
#win32:CONFIG(release, debug|release): LIBS += D:/ProgramData/program_install/program_Cplus/curl_install/curl-8.6.0/curl-8.6.0/build/lib/release/libcurl-d_imp.lib
#else:win32:CONFIG(debug, debug|release): LIBS += D:/ProgramData/program_install/program_Cplus/curl_install/curl-8.6.0/curl-8.6.0/build/lib/debug/libcurl-d_imp.lib
#else:unix: LIBS += D:/ProgramData/program_install/program_Cplus/curl_install/curl-8.6.0/curl-8.6.0/build/lib/libcurl-d_imp.lib
#INCLUDEPATH += D:/ProgramData/program_install/program_Cplus/curl_install/curl-8.6.0/curl-8.6.0/include
#DEPENDPATH += D:/ProgramData/program_install/program_Cplus/curl_install/curl-8.6.0/curl-8.6.0/include




# Crow
INCLUDEPATH += D:/ProgramData/program_install/program_Cplus/test_crow/Crow-1.2.0/include
# asio
INCLUDEPATH += D:/ProgramData/program_install/asio_install/asio-1.30.2/include


SUBDIRS += \
    influxdb/influxdb.pro

DISTFILES += \
    influxdb/date/LICENSE \
    influxdb/date/README.md \
    influxdb/influxdb.pro.user \
    influxdb/influxdb.pro.user.2448638


## 孙源Y7000P电脑：curl相关依赖库
#win32:CONFIG(release, debug|release): LIBS += F:/iGreatWorks/iGreatWorks_libs/iGreatWorks_libs/curl-8.6.0/curl-8.6.0/build/lib/release/libcurl-d_imp.lib
#else:win32:CONFIG(debug, debug|release): LIBS += F:/iGreatWorks/iGreatWorks_libs/iGreatWorks_libs/curl-8.6.0/curl-8.6.0/build/lib/debug/libcurl-d_imp.lib
#else:unix: LIBS += F:/iGreatWorks/iGreatWorks_libs/iGreatWorks_libs/curl-8.6.0/curl-8.6.0/build/lib/libcurl-d_imp.lib
#INCLUDEPATH += F:/iGreatWorks/iGreatWorks_libs/iGreatWorks_libs/curl-8.6.0/curl-8.6.0/include
#DEPENDPATH += F:/iGreatWorks/iGreatWorks_libs/iGreatWorks_libs/curl-8.6.0/curl-8.6.0/include

## 孙源Y7000P电脑：boost相关依赖库
#unix|win32: LIBS += F:/iGreatWorks/iGreatWorks_libs/iGreatWorks_libs/boost_1_83_0/boost_1_83_0/stage/lib/libboost_system-vc142-mt-gd-x64-1_83.lib
#INCLUDEPATH += F:/iGreatWorks/iGreatWorks_libs/iGreatWorks_libs/boost_1_83_0/boost_1_83_0
#DEPENDPATH += F:/iGreatWorks/iGreatWorks_libs/iGreatWorks_libs/boost_1_83_0/boost_1_83_0

# 孙源Y7000P电脑：Crow相关依赖库
INCLUDEPATH += F:/iGreatWorks/iGreatWorks_libs/iGreatWorks_libs/Crow-1.2.0/include

# 孙源Y7000P电脑：asio相关依赖库
INCLUDEPATH += F:/iGreatWorks/iGreatWorks_libs/iGreatWorks_libs/asio_1.30.2/asio-1.30.2/include


# 工作站上的相关依赖库目录
# Curl
win32:CONFIG(release, debug|release): LIBS += E:/Laboratory/iGreatWorks_libs/curl-8.6.0/curl-8.6.0/build/lib/release/libcurl-d_imp.lib
else:win32:CONFIG(debug, debug|release): LIBS += E:/Laboratory/iGreatWorks_libs/curl-8.6.0/curl-8.6.0/build/lib/debug/libcurl-d_imp.lib
else:unix: LIBS += E:/Laboratory/iGreatWorks_libs/curl-8.6.0/curl-8.6.0/build/lib/libcurl-d_imp.lib

INCLUDEPATH += E:/Laboratory/iGreatWorks_libs/curl-8.6.0/curl-8.6.0/include
DEPENDPATH += E:/Laboratory/iGreatWorks_libs/curl-8.6.0/curl-8.6.0/include

# Boost
unix|win32: LIBS += E:/Laboratory/iGreatWorks_libs/boost_1_83_0/boost_1_83_0/boost_1_83_0/stage/lib/libboost_system-vc142-mt-gd-x64-1_83.lib

INCLUDEPATH += E:/Laboratory/iGreatWorks_libs/boost_1_83_0/boost_1_83_0/boost_1_83_0
DEPENDPATH += E:/Laboratory/iGreatWorks_libs/boost_1_83_0/boost_1_83_0/boost_1_83_0

# Crow相关依赖库
INCLUDEPATH += E:/Laboratory/iGreatWorks_libs/Crow-1.2.0/Crow-1.2.0/include

# 孙源Y7000P电脑：asio相关依赖库
INCLUDEPATH += E:/Laboratory/iGreatWorks_libs/asio_1.30.2/asio-1.30.2/include
