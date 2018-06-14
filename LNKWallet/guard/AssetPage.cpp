#include "AssetPage.h"
#include "ui_AssetPage.h"

#include "wallet.h"

AssetPage::AssetPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssetPage)
{
    ui->setupUi(this);

    qDebug() << "asssssssssssssssssss ";
}

AssetPage::~AssetPage()
{
    delete ui;
}

void AssetPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(248,249,253),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(248,249,253),Qt::SolidPattern));

    painter.drawRect(rect());
}
