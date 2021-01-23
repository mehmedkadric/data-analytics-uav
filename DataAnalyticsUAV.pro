QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    changedetector.cpp \
    controller.cpp \
    exif.cpp \
    featurematcher.cpp \
    geocoder.cpp \
    imageanalyzer.cpp \
    inputreader.cpp \
    jsonparser.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    changedetector.h \
    controller.h \
    exif.h \
    featurematcher.h \
    geocoder.h \
    imageanalyzer.h \
    inputreader.h \
    jsonparser.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


INCLUDEPATH += /usr/local/include/opencv4
LIBS += -lopencv_core
LIBS += -lopencv_imgproc
LIBS += -lopencv_highgui
LIBS += -lopencv_ml
LIBS += -lopencv_video
LIBS += -lopencv_features2d
LIBS += -lopencv_calib3d
LIBS += -lopencv_objdetect
LIBS += -lopencv_flann
LIBS += -lopencv_stitching
LIBS += -lopencv_xfeatures2d
LIBS += -lopencv_imgcodecs
LIBS += -L/usr/local/lib/
LIBS += -L/usr/local/Cellar/pcl/1.11.1_6/lib
LIBS += -L/usr/local/Cellar/nlohmann-json/3.9.1_1/lib

INCLUDEPATH += /usr/local/Cellar/pcl/1.11.1_1/include/pcl-1.11

INCLUDEPATH += /usr/local/opt/curl/include
INCLUDEPATH += /usr/local/opt/curlpp/include
INCLUDEPATH += /Users/mehmed/DataAnalyticsUAV/cpp-opencage-geocoder/cpr/include
INCLUDEPATH += /usr/local/Cellar/nlohmann-json/3.9.1_1/include


QT += network
QT += core
