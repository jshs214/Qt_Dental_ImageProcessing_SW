QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    blursharpenmask.cpp \
    cephaloform.cpp \
    cephimageview.cpp \
    cephvalueadjustment.cpp \
    dentalimageview.cpp \
    main.cpp \
    mainwindow.cpp \
    panoramaform.cpp \
    panovalueadjustment.cpp

HEADERS += \
    blursharpenmask.h \
    cephaloform.h \
    cephimageview.h \
    cephvalueadjustment.h \
    dentalimageview.h \
    mainwindow.h \
    panoramaform.h \
    panovalueadjustment.h

FORMS += \
    cephaloform.ui \
    mainwindow.ui \
    panoramaform.ui

TRANSLATIONS += \
    TeamUnsigned_ko_KR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
