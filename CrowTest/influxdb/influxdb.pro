QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = app # app
#CONFIG += staticlib # staticlib or sharedlib
CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += INFLUXDB_WITH_BOOST
DEFINES += ONLY_HTTP
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    HTTP.cxx \
    InfluxDB.cxx \
    InfluxDBFactory.cxx \
    LineProtocol.cxx \
    Point.cxx \
    Proxy.cxx \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    HTTP.h \
    LineProtocol.h \
    UriParser.h \
    date/date.h \
    include/InfluxDB.h \
    include/InfluxDBException.h \
    include/InfluxDBFactory.h \
    include/Point.h \
    include/Proxy.h \
    include/Transport.h \
#    include/influxdb_export.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


#win32:CONFIG(release, debug|release): LIBS += -LF:/Package/curl-7.88.1/build/lib/release/ -llibcurl-d_imp
#else:win32:CONFIG(debug, debug|release): LIBS += -LF:/Package/curl-7.88.1/build/lib/debug/ -llibcurl-d_imp
#else:unix: LIBS += -LF:/Package/curl-7.88.1/build/lib/ -llibcurl-d_imp

#INCLUDEPATH += F:/Package/curl-7.88.1/curl-7.88.1/include
#DEPENDPATH += F:/Package/curl-7.88.1/curl-7.88.1/include



unix|win32: LIBS += -L$$PWD/../../../../../SoftWare/boost_1_83_0/stage/lib/ -llibboost_date_time-vc142-mt-gd-x64-1_83

INCLUDEPATH += $$PWD/../../../../../SoftWare/boost_1_83_0
DEPENDPATH += $$PWD/../../../../../SoftWare/boost_1_83_0

unix|win32: LIBS += -L$$PWD/../../../../../SoftWare/boost_1_83_0/stage/lib/ -llibboost_system-vc142-mt-gd-x64-1_83

INCLUDEPATH += $$PWD/../../../../../SoftWare/boost_1_83_0/stage
DEPENDPATH += $$PWD/../../../../../SoftWare/boost_1_83_0/stage

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../Package/curl-7.88.1/build/lib/release/ -llibcurl-d_imp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../Package/curl-7.88.1/build/lib/debug/ -llibcurl-d_imp
else:unix: LIBS += -L$$PWD/../../../../../Package/curl-7.88.1/build/lib/ -llibcurl-d_imp

INCLUDEPATH += $$PWD/../../../../../Package/curl-7.88.1/curl-7.88.1/include
DEPENDPATH += $$PWD/../../../../../Package/curl-7.88.1/curl-7.88.1/include
