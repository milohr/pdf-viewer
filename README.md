# ![Icon](src/assets/icon_mres.png) PDF Viewer

A handy PDF Viewer tool using Qt 4 and the Poppler lib

## Features

- PDF files are rendered by the Poppler library
- Only visible viewport quad is really rendered
- Pan, rotate and zoom into pages
- Display document information

## Command-line syntax

```sh
pdf-viewer <file> [-windowed]
```
- `file`: Relative or absolute path to file to display.
- `-windowed`: If given, the app does not start as fullscreen

## Directory structure

- `src:` Source files
    - `assets`: Resource files compiled into binary, like icons
- `test-pdf`: Some Pdf files for testing

## Links
- Poppler:
   - [Homepage](https://poppler.freedesktop.org/)
   - [Tutorial](https://people.freedesktop.org/~aacid/docs/qt4/)
   - [Repository](https://cgit.freedesktop.org/poppler/poppler/tree/)
   - [License](https://cgit.freedesktop.org/poppler/poppler/tree/COPYING)
