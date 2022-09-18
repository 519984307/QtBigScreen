# This project demonstrates how to use QtWebAppLib by including the
# sources into this project.

TARGET = QtBigScreen
TEMPLATE = app
QT += core network
# CONFIG += console
LIBS +=User32.LIB
QT += xml
greaterThan(QT_MAJOR_VERSION,4):QT+=widgets
HEADERS += \
           src/Toast.h \
           src/addon.h \
           src/global.h \
           src/icon.h \
           src/requestmapper.h \
           src/controller/fileuploadcontroller.h

SOURCES += src/main.cpp \
           src/Toast.cpp \
           src/addon.cpp \
           src/global.cpp \
           src/icon.cpp \
           src/requestmapper.cpp \
           src/controller/fileuploadcontroller.cpp

OTHER_FILES += etc/* etc/real_docroot/* etc/templates/* etc/ssl/* logs/* etc/addon/* etc/source/*

DISTFILES += ../CHANGELOG.txt ../LICENSE.txt ../README.txt \
    etc/ssl/README.txt
#---------------------------------------------------------------------------------------
# The following lines include the sources of the QtWebAppLib library
#---------------------------------------------------------------------------------------

include(./QtWebApp/logging/logging.pri)
include(./QtWebApp/httpserver/httpserver.pri)
include(./QtWebApp/templateengine/templateengine.pri)

FORMS += \
    src/Toast.ui

RESOURCES += \
    src/img.qrc

