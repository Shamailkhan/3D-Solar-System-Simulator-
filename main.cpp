#include <QApplication>
#include "SolarSystemSimulator.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    SolarSystemWidget w;
    w.resize(1024, 768);
    w.show();
    return a.exec();
}
