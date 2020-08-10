QT       += core gui charts multimedia

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
    alglib/src/alglibinternal.cpp \
    alglib/src/alglibmisc.cpp \
    alglib/src/ap.cpp \
    alglib/src/dataanalysis.cpp \
    alglib/src/diffequations.cpp \
    alglib/src/fasttransforms.cpp \
    alglib/src/integration.cpp \
    alglib/src/interpolation.cpp \
    alglib/src/linalg.cpp \
    alglib/src/optimization.cpp \
    alglib/src/solvers.cpp \
    alglib/src/specialfunctions.cpp \
    alglib/src/statistics.cpp \
    catransient.cpp \
    effecttoolbutton.cpp \
    main.cpp \
    pathbrowsewdgt.cpp \
    rdrCustomPixmapItem.cpp \
    rdrEllipseItem.cpp \
    rdrRegionVisualization.cpp \
    rdr_calc.cpp \
    rdrmaindlg.cpp \
    rdrmaindlg_calcu.cpp \
    rdrmaindlg_export.cpp \
    rdrmaindlg_import.cpp \
    rdrmaindlg_table.cpp \
    xcamthread.cpp \
    xcwdgt.cpp \
    xr_toolfunctions.cpp \
    xrcolorpickdlg.cpp \
    xrcolorsettingsitem.cpp \
    xrsettingswdgt.cpp

HEADERS += \
    alglib/src/alglibinternal.h \
    alglib/src/alglibmisc.h \
    alglib/src/ap.h \
    alglib/src/dataanalysis.h \
    alglib/src/diffequations.h \
    alglib/src/fasttransforms.h \
    alglib/src/integration.h \
    alglib/src/interpolation.h \
    alglib/src/linalg.h \
    alglib/src/optimization.h \
    alglib/src/solvers.h \
    alglib/src/specialfunctions.h \
    alglib/src/statistics.h \
    alglib/src/stdafx.h \
    catransient.h \
    effecttoolbutton.h \
    pathbrowsewdgt.h \
    rdrCustomPixmapItem.h \
    rdrEllipseItem.h \
    rdrPathItem.h \
    rdrRegionVisualization.h \
    rdr_calc.h \
    rdrmaindlg.h \
    xcamthread.h \
    xcwdgt.h \
    xr_toolfunctions.h \
    xrcolorpickdlg.h \
    xrcolorsettingsitem.h \
    xrsettingswdgt.h

FORMS += \
    pathbrowsewdgt.ui \
    rdrimporter.ui \
    rdrmaindlg.ui \
    xcwdgt.ui \
    xrcolorpickdlg.ui \
    xrsettingswdgt.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    foxIcon.qrc \
    xCam_resources.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libtiff-master/libtiff/release/ -ltiff
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libtiff-master/libtiff/debug/ -ltiff
else:unix: LIBS += -L$$PWD/../libtiff-master/libtiff/ -ltiff

INCLUDEPATH += $$PWD/../libtiff-master/libtiff
DEPENDPATH += $$PWD/../libtiff-master/libtiff

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../opt/pylon5/lib64/release/ -lGCBase_gcc_v3_1_Basler_pylon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../opt/pylon5/lib64/debug/ -lGCBase_gcc_v3_1_Basler_pylon
else:unix: LIBS += -L$$PWD/../../../../opt/pylon5/lib64/ -lGCBase_gcc_v3_1_Basler_pylon

INCLUDEPATH += $$PWD/../../../../opt/pylon5/include
DEPENDPATH += $$PWD/../../../../opt/pylon5/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../opt/pylon5/lib64/release/ -lGenApi_gcc_v3_1_Basler_pylon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../opt/pylon5/lib64/debug/ -lGenApi_gcc_v3_1_Basler_pylon
else:unix: LIBS += -L$$PWD/../../../../opt/pylon5/lib64/ -lGenApi_gcc_v3_1_Basler_pylon

INCLUDEPATH += $$PWD/../../../../opt/pylon5/include/GenApi
DEPENDPATH += $$PWD/../../../../opt/pylon5/include/GenApi

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../opt/pylon5/lib64/release/ -lpylonbase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../opt/pylon5/lib64/debug/ -lpylonbase
else:unix: LIBS += -L$$PWD/../../../../opt/pylon5/lib64/ -lpylonbase

INCLUDEPATH += $$PWD/../../../../opt/pylon5/include
DEPENDPATH += $$PWD/../../../../opt/pylon5/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../opt/pylon5/lib64/release/ -lpylonc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../opt/pylon5/lib64/debug/ -lpylonc
else:unix: LIBS += -L$$PWD/../../../../opt/pylon5/lib64/ -lpylonc

INCLUDEPATH += $$PWD/../../../../opt/pylon5/include
DEPENDPATH += $$PWD/../../../../opt/pylon5/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../opt/pylon5/lib64/release/ -lpylonutility
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../opt/pylon5/lib64/debug/ -lpylonutility
else:unix: LIBS += -L$$PWD/../../../../opt/pylon5/lib64/ -lpylonutility

INCLUDEPATH += $$PWD/../../../../opt/pylon5/include
DEPENDPATH += $$PWD/../../../../opt/pylon5/include

