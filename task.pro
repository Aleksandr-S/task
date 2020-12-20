QT += quick quickcontrols2 network sql

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        src/client.cpp \
    src/connections.cpp \
        src/main.cpp \
    src/person.cpp \
		src/persons.cpp \
        src/server.cpp \
        src/serverthread.cpp \
		src/database.cpp

HEADERS += \
    src/client.h \
    src/server.h \
    src/serverthread.h \
    src/message.h \
    src/person.h \
    src/connections.h \
    src/persons.h \
    src/database.h

RESOURCES += src/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += ./src/qml

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


