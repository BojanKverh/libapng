QT += testlib

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_libapng.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libapng/release/ -llibapng
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libapng/debug/ -llibapng
else:unix: LIBS += -L$$OUT_PWD/../libapng/ -llibapng

INCLUDEPATH += $$PWD/../libapng
DEPENDPATH += $$PWD/../libapng
