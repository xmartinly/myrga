QT       += core gui printsupport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chart_plugin/qcp_modify.cpp \
    chart_plugin/qcustomplot.cpp \
    utility/comm_http.cpp \
    dlgs/addrga_dlg.cpp \
    dlgs/ipaddress.cpp \
    dlgs/recipe_dlg.cpp \
    main.cpp \
    myrga.cpp \
    data_helper.cpp \
    observer/data_observer.cpp \
    observer/observer_subject.cpp \
    utility/response_calc.cpp \
    utility/rga_utility.cpp \
    utility/static_container.cpp

HEADERS += \
    utility/comm_http.h \
    thirdparty/httplib.h \
    myrga.h \
    data_helper.h \
    chart_plugin/qcp_modify.h \
    chart_plugin/qcustomplot.h \
    dlgs/addrga_dlg.h \
    dlgs/ipaddress.h \
    dlgs/recipe_dlg.h \
    observer/data_observer.h \
    observer/observer_subject.h \
    utility/http_command.h \
    utility/response_calc.h \
    utility/rga_structs.h \
    utility/rga_utility.h \
    utility/static_container.h

FORMS += \
    dlgs/addrga_dlg.ui \
    dlgs/recipe_dlg.ui \
    myrga.ui


TRANSLATIONS += \
    myrga_zh_CN.ts

#CONFIG += lrelease
#CONFIG += embed_translations


PRECOMPILED_HEADER = stable.h

QMAKE_CXXFLAGS += /MP

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    CONFIG(debug, debug|release) {  # 需要基于 Debug 模式
        DEFINES += VLD_MODULE
        VLD_PATH = "C:\Program Files (x86)\Visual Leak Detector"
        INCLUDEPATH += $${VLD_PATH}/include
        LIBS += -L$${VLD_PATH}/lib/Win32 -lvld
    }
}

RESOURCES += \
    resources/resources.qrc

RC_ICONS = myrga.ico
