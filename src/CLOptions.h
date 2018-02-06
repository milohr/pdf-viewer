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
    void parse(int argc, char **argv);

    /**
     * @brief Enables windowed presentation, otherwise the viewer opens in fullscreen mode
     */
    static constexpr const char *OPTION_WINDOWED = "-windowed";

    /**
     * @brief Path of PDF to display
     */
    QString path;

    /**
     * @brief If set, viewer opens in windowed mode instead of fullscreen
     */
    bool windowed = false;
};

#endif // CLOPTIONS_H
