#include "FavoritePairsWidget.h"
#include "ui_FavoritePairsWidget.h"

#include <QDebug>

#include "wallet.h"

FavoritePairsWidget::FavoritePairsWidget( int _size, QWidget *parent) :
    QWidget(parent),
    size(_size),
    ui(new Ui::FavoritePairsWidget)
{
    ui->setupUi(this);

    for( int i = 0; i < size; i++)
    {
        QToolButton* btn = new QToolButton(this);
        btnVector.append(btn);
        btn->setProperty("BtnStyle","favoritePair");
        btn->setCheckable(true);
        btn->setMinimumHeight(20);
        connect(btn,SIGNAL(clicked()),this,SLOT(onBtnClicked()));
    }

    init();
}

FavoritePairsWidget::~FavoritePairsWidget()
{
    delete ui;
}

void FavoritePairsWidget::init()
{
    HXChain::getInstance()->configFile->beginGroup("/FavoriteOrderPairs");
    QStringList keys = HXChain::getInstance()->configFile->childKeys();
    HXChain::getInstance()->configFile->endGroup();
    for(QString& key : keys)
    {
        key.replace("+","/");
    }
    setBtns(keys);

    adjustPosAndVisiable();
}

void FavoritePairsWidget::setBtns(QStringList texts)
{
    btnTexts.clear();
    foreach(const QString& str, texts)
    {
        if(btnTexts.size() >= size)  break;
        if(btnTexts.contains(str))  continue;
        btnTexts.append(str);
    }
}

void FavoritePairsWidget::removeBtn(QString text)
{
    if(!btnTexts.contains(text))    return;
    btnTexts.removeAll(text);
    adjustPosAndVisiable();
}

void FavoritePairsWidget::adjustPosAndVisiable()
{
    int i = 0;
    int x = 90;
    for(;i < btnTexts.size(); i++)
    {
        btnVector.at(i)->setText(btnTexts.at(i));
        btnVector.at(i)->show();
        btnVector.at(i)->adjustSize();
        btnVector.at(i)->move(x,5);
        x += btnVector.at(i)->width() + 40;
    }

    for(; i < size; i++)
    {
        btnVector.at(i)->hide();
    }
}

void FavoritePairsWidget::onBtnClicked()
{
    QToolButton* btn = static_cast<QToolButton*>(sender());
    showPair(btn->text());
}



