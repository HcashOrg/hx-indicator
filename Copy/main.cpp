#include <QApplication>

#include "UpdateWidget.h"
#include "DataUtil.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    qInstallMessageHandler(DataUtil::myMessageOutput);
    UpdateWidget wi;
    wi.show();

    return a.exec();
}
