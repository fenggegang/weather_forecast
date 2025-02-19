#include "weather_forecast.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    weather_forecast w;
    w.show();
    return a.exec();
}
