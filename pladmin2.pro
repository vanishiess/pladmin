QT += core gui
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/bin/libssh-0.9.5/include
LIBS += -L$$PWD/bin -lssh

INCLUDEPATH += $$PWD/bin/inc
LIBS += -L$$PWD/bin -larchive

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    sftpclient.cpp \
    sftpparams.cpp \
    sftpsession.cpp \
    sshsession.cpp \
    xml.cpp \
    prog.cpp \
    version.cpp \
    versionlist.cpp \
    setversion.cpp \
    setprog.cpp \
    userlist.cpp \
    setuser.cpp \
    loading.cpp

HEADERS += \
    mainwindow.h \
    sftpclient.h \
    sftperrors.h \
    sftpparams.h \
    sftpsession.h \
    sshsession.h \
    xml.h \
    prog.h \
    version.h \
    versionlist.h \
    setversion.h \
    setprog.h \
    userlist.h \
    setuser.h \
    moveprog.h \
    loading.h

FORMS += \
    mainwindow.ui \
    versionlist.ui \
    setversion.ui \
    setprog.ui \
    userlist.ui \
    setuser.ui \
    loading.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
