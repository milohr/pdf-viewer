QT += core gui declarative
QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic

TARGET = pdf-viewer
TEMPLATE = app
LIBS += -lpoppler-qt4

DESTDIR = build/bin
OBJECTS_DIR = build/objects
MOC_DIR = build/moc
RCC_DIR = build/rcc

RESOURCES += rc.qrc
SOURCES += src/main.cpp src/PdfViewer.cpp
HEADERS  += src/PdfViewer.h
