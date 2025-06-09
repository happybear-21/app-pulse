#include <QApplication>
#include "spotlightwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SpotlightWindow window;
    window.show();
    return app.exec();
}