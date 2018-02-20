QT += core gui declarative
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic

# Import Qt Quick Controls 1:
QML_IMPORT_PATH += /usr/lib/x86_64-linux-gnu/qt5/qml/

TARGET = pdf-viewer
TEMPLATE = app
LIBS += -lpoppler-qt4

DESTDIR = build/bin
OBJECTS_DIR = build/objects
MOC_DIR = build/moc
RCC_DIR = build/rcc

QMAKE_CXXFLAGS += -Wall -Wextra

RESOURCES += rc.qrc

SOURCES += src/main.cpp \
    src/Window.cpp \
    src/PdfViewer.cpp

HEADERS  += \
    src/Window.h \
    src/PdfViewer.h

