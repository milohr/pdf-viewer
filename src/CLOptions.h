#ifndef CLOPTIONS_H
#define CLOPTIONS_H

#include <QString>

/**
 * @brief Stores options from the command line
 */
struct CLOptions
{

    /**
     * @brief Initialize this options struct with options from the command line
     * @param argc Count of arguments
     * @param argv Arguments
     */
    CLOptions(int const argc, char const * const * const argv);

    /**
     * @brief Path of PDF to display
     */
    QString path() const;

    /**
     * @return If true, viewer opens in windowed mode instead of fullscreen
     */
    bool windowed() const;

private:

    int const mArgc;
    char const * const * const mArgv;
    QString const mPath;
    bool const mWindowed;

    /**
     * @brief Search a boolean switch option by name.
     * @param name Option to look for.
     * @return True if found, otherwise false.
     */
    bool findSwitchOption(QString const name);

    /**
     * @brief Returns option to enable windowed presentation, otherwise the viewer opens in fullscreen mode
     */
    static constexpr const char *optionWindowed() {
        return "-windowed";
    }

};

#endif // CLOPTIONS_H
