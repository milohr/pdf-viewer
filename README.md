# ![Icon](img/icon_lres.png) PDF Viewer

A QML QtQuick 1 component capable of PDF files rendering.

```qml
PdfViewer {
    anchors.fill: parent
    source: "Demo.pdf"
    maxZoom: 4
    backgroundColor: "#eee"
    
    onZoomChanged: console.log("Zoom is: " + zoom);
    onPanChanged: console.log("Pan is: " + pan);
}
```

## Features

- **Flexibility:** The bare component on itself does not impose any UI visuals at all, it is solely responsible for PDF rendering.
- **Plug'n'play:** The repository ships with a [`main.qml`](qml/main.qml) file, displaying a complete PDF viewer interface, serving as a demo and use-case testing.
- **Professionality:** PDF files are rendered by the [Poppler library](https://poppler.freedesktop.org/).
- **Optimization:** Only visible viewport quad is really rendered. Touch or mouse input are handled in C++ implementation.

## Documentation

Run `doxygen` inside the project root directory to generate an HTML documentation.

