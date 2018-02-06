#include "CLOptions.h"

void CLOptions::parse(int argc, char **argv)
{
    path = QString(argv[1]);
    for(int i = 2; i < argc; i++) {
        const char *option = argv[i];
        if(0 == strcmp(OPTION_WINDOWED, option)) {
            windowed = true;
        }
    }
}
