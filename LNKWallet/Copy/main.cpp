#include <QApplication>

#include "UpdateWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    UpdateWidget wi;
    wi.show();

    return a.exec();
}
