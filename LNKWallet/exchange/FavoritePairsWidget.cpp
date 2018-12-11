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

    if(btnTexts.size() > 0)
    {
        setCurrentBtn(btnTexts.first());
    }
    else
    {
        adjustPosAndVisiable();
    }
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

void FavoritePairsWidget::setCurrentBtn(QString text)
{
    if(!btnTexts.contains(text) || text.isEmpty())    return;
    currentText = text;
    adjustPosAndVisiable();
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

        if(currentText == btnTexts.at(i))
        {
            ui->lineLabel->move( (x + (btnVector.at(i)->width() - ui->lineLabel->width()) / 2), 25 );
            btnVector.at(i)->setChecked(true);
        }

        x += btnVector.at(i)->width() + 40;
    }

    for(; i < size; i++)
    {
        btnVector.at(i)->hide();
    }
}

void FavoritePairsWidget::onBtnClicked()
{
    foreach(QToolButton* btn, btnVector)
    {
        if(btn == sender())
        {
            btn->setChecked(true);
            showPair(btn->text());
            setCurrentBtn(btn->text());
        }
        else
        {
            btn->setChecked(false);
        }
    }

}



