QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    flowlayout.cpp \
    imagecard.cpp \
    imagereader.cpp \
    imageviewer.cpp \
    imagewriter.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    flowlayout.h \
    imagecard.h \
    imagereader.h \
    imageviewer.h \
    imagewriter.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    icon.qrc \
    loading.gif \
    folder.png \
    view.png \
    image.png \
    icon.jpg \
    icon.qrc

# For WinBooBs Icon
RC_FILE = myapp.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    myapp.rc
