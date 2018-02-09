#include "CLOptions.h"

CLOptions::CLOptions(int const argc, char const * const * const argv)
{
    // 'path' is defined by the first argument:
    path = QString(argv[1]);

    // search for other arguments:
    for(int i = 2; i < argc; i++) {

        QString option = argv[i];

        // '-windowed' argument found:
        if(option == optionWindowed()) {
            windowed = true;
        }
    }
}
