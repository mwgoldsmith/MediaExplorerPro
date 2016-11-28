#-------------------------------------------------
#
# Project created by QtCreator 2016-11-12T18:00:53
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = mxpgui
TEMPLATE = app

SOURCES += \
    main.cpp \
    core.cpp \
    Gui/mainwindow.cpp \
    Gui/dockwidgetmanager.cpp \
    Gui/panels/browserpanel.cpp \
    Gui/panels/downloadspanel.cpp \
    Gui/panels/editorpanel.cpp \
    Gui/panels/librarypanel.cpp \
    Gui/docks/clipviewerdock.cpp \
    Gui/docks/statusconsoledock.cpp \
    Gui/docks/clippropertiesdock.cpp \
    Gui/docks/libraryfoldersdock.cpp \
    Gui/workflows/libraryworkflow.cpp \
    Gui/workflows/editorworkflow.cpp \
    Gui/workflows/downloadsworkflow.cpp \
    Gui/workflows/browserworkflow.cpp \
    Gui/widgets/workflowtoolbar.cpp \
    Gui/mediaitemdelegate.cpp \
    Gui/medialistmodel.cpp \
    Gui/mediaitemproxymodel.cpp \
    Media/media.cpp \
    Media/mediafolder.cpp \
    Media/mediaproperty.cpp \
    Settings/settings.cpp \
    Settings/settingvalue.cpp

HEADERS  += \
    core.h \
    Gui/mainwindow.h \
    Gui/dockwidgetmanager.h \
    Gui/panels/browserpanel.h \
    Gui/panels/downloadspanel.h \
    Gui/panels/editorpanel.h \
    Gui/panels/librarypanel.h \
    Gui/docks/clipviewerdock.h \
    Gui/docks/statusconsoledock.h \
    Gui/docks/clippropertiesdock.h \
    Gui/docks/libraryfoldersdock.h \
    Gui/workflows/libraryworkflow.h \
    Gui/workflows/editorworkflow.h \
    Gui/workflows/downloadsworkflow.h \
    Gui/workflows/browserworkflow.h \
    Gui/widgets/workflowtoolbar.h \
    Gui/mediaitemdelegate.h \
    Gui/medialistmodel.h \
    Gui/mediaitemproxymodel.h \
    Media/media.h \
    Media/mediafolder.h \
    Media/mediaproperty.h \
    Settings/settings.h \
    Settings/settingvalue.h \
    Tools/qsingleton.h \
    Tools/singleton.h \
    mxpgui.h

FORMS    += \
    Gui/ui/mainwindow.ui \
    Gui/panels/ui/browserpanel.ui \
    Gui/panels/ui/downloadspanel.ui \
    Gui/panels/ui/editorpanel.ui \
    Gui/panels/ui/librarypanel.ui \
    Gui/docks/ui/clipviewerdock.ui \
    Gui/docks/ui/statusconsoledock.ui \
    Gui/docks/ui/clippropertiesdock.ui \
    Gui/docks/ui/libraryfoldersdock.ui

DESTDIR = ../../build/msvc/bin/x86

INCLUDEPATH += $$PWD/../../include/lib
DEPENDPATH += $$PWD/../../include/lib

win32 {
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../../build/msvc/lib/x86/ -lmxp
    }
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../build/msvc/lib/x86/ -lmxpd
    }
    LIBS += -lole32
}
