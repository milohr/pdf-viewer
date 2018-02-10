#include "CLOptions.h"

CLOptions::CLOptions(int const argc, char const * const * const argv)
    : mArgc(argc)
    , mArgv(argv)
    , mPath(argv[1]) // 'path' is defined by the first argument
    , mWindowed(findSwitchOption(optionWindowed()))
{
}

bool CLOptions::findSwitchOption(QString const name)
{
    for(int i = 2; i < mArgc; i++) {
        if(QString(mArgv[i]) == name) {
            return true;
        }
    }
    return false;
}

QString CLOptions::path() const
{
    return mPath;
}

bool CLOptions::windowed() const
{
    return mWindowed;
}
