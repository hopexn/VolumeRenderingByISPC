#include <iostream>
#include <qapplication.h>
#include "DisplayWidget.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    DisplayWidget display(512, 512);
    display.show();
    return app.exec();
}
