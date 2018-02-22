QT += core gui declarative

# Compile to C++98:
CONFIG += c++98
QMAKE_CXXFLAGS += -std=c++98

TARGET = pdf-viewer
TEMPLATE = app
LIBS += -lpoppler-qt4

DESTDIR = build/bin
OBJECTS_DIR = build/objects
MOC_DIR = build/moc
RCC_DIR = build/rcc

RESOURCES += rc.qrc

SOURCES += \
    src/main.cpp \
    src/pdf_viewer/PdfViewer.cpp

HEADERS  += \
    src/pdf_viewer/PdfViewer.h
