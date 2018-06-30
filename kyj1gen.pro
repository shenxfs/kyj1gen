#-------------------------------------------------
#
# Project created by QtCreator 2018-06-21T22:53:49
#
#-------------------------------------------------

QT       += core gui
RC_ICONS += chip.ico
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
requires(qtConfig(filedialog))
#include($$PWD/qtpropertybrowser/qtpropertybrowser.pri)
TARGET = kyj1gen
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    qtpropertybrowser/qtbuttonpropertybrowser.cpp \
    qtpropertybrowser/qteditorfactory.cpp \
    qtpropertybrowser/qtgroupboxpropertybrowser.cpp \
    qtpropertybrowser/qtpropertybrowser.cpp \
    qtpropertybrowser/qtpropertybrowserutils.cpp \
    qtpropertybrowser/qtpropertymanager.cpp \
    qtpropertybrowser/qttreepropertybrowser.cpp \
    qtpropertybrowser/qtvariantproperty.cpp \
    coff.cpp \
    main.cpp \
    mainwindow.cpp \
    setting.cpp

HEADERS += \
    qtpropertybrowser/qtbuttonpropertybrowser.h \
    qtpropertybrowser/qteditorfactory.h \
    qtpropertybrowser/qtgroupboxpropertybrowser.h \
    qtpropertybrowser/qtpropertybrowser.h \
    qtpropertybrowser/qtpropertybrowserutils_p.h \
    qtpropertybrowser/qtpropertymanager.h \
    qtpropertybrowser/qttreepropertybrowser.h \
    qtpropertybrowser/qtvariantproperty.h \
    coff.h \
    mainwindow.h \
    setting.h

RESOURCES += \
    kyj1gen.qrc \
    qtpropertybrowser/qtpropertybrowser.qrc

FORMS += \
    setting.ui

DISTFILES += \
    qtpropertybrowser/qtpropertybrowser.pri \
    qtpropertybrowser/qtpropertybrowserutils.pri \
    qtpropertybrowser/images/cursor-arrow.png \
    qtpropertybrowser/images/cursor-busy.png \
    qtpropertybrowser/images/cursor-closedhand.png \
    qtpropertybrowser/images/cursor-cross.png \
    qtpropertybrowser/images/cursor-forbidden.png \
    qtpropertybrowser/images/cursor-hand.png \
    qtpropertybrowser/images/cursor-hsplit.png \
    qtpropertybrowser/images/cursor-ibeam.png \
    qtpropertybrowser/images/cursor-openhand.png \
    qtpropertybrowser/images/cursor-sizeall.png \
    qtpropertybrowser/images/cursor-sizeb.png \
    qtpropertybrowser/images/cursor-sizef.png \
    qtpropertybrowser/images/cursor-sizeh.png \
    qtpropertybrowser/images/cursor-sizev.png \
    qtpropertybrowser/images/cursor-uparrow.png \
    qtpropertybrowser/images/cursor-vsplit.png \
    qtpropertybrowser/images/cursor-wait.png \
    qtpropertybrowser/images/cursor-whatsthis.png
