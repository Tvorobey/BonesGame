//Local
#include "MainHandler.h"
//Qt
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainHandler handler;

    return a.exec();
}
