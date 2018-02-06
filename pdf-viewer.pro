QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pdf-viewer
TEMPLATE = app
LIBS += -lpoppler-qt4

DESTDIR = build/bin
OBJECTS_DIR = build/objects
MOC_DIR = build/moc
RCC_DIR = build/rcc

QMAKE_CXXFLAGS += -Wall -Wextra

RESOURCES += src/rc.qrc

SOURCES += src/main.cpp \
    src/Window.cpp \
    src/PdfWidget.cpp \
    src/StatusBar.cpp \
    src/CLOptions.cpp \
    src/ZoomSlider.cpp \
    src/HeaderBar.cpp \
    src/PdfInfoWidget.cpp

HEADERS  += \
    src/Window.h \
    src/PdfWidget.h \
    src/StatusBar.h \
    src/CLOptions.h \
    src/ZoomSlider.h \
    src/HeaderBar.h \
    src/PdfInfoWidget.h
