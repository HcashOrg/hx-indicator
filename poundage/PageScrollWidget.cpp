#include "PageScrollWidget.h"
#include "ui_PageScrollWidget.h"

class PageScrollWidget::PageScrollWidgetPrivate
{
public:
    PageScrollWidgetPrivate()
        :totalPage(0)
    {
        buttonNumber.resize(5,0);
    }
public:
    unsigned int totalPage;
    std::vector<QToolButton*> numberButtons;
    std::vector<int> buttonNumber;
};

PageScrollWidget::PageScrollWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PageScrollWidget),
    _p(new PageScrollWidgetPrivate())
{
    ui->setupUi(this);
    InitWidget();
}

PageScrollWidget::~PageScrollWidget()
{
    delete _p;
    delete ui;
}

void PageScrollWidget::SetTotalPage(unsigned int number)
{
    _p->totalPage = number;
    ResetButton();
}

void PageScrollWidget::RefreshButton()
{

}

void PageScrollWidget::InitWidget()
{
    InitStyle();
    _p->numberButtons = {ui->toolButton_first,ui->toolButton_second,ui->toolButton_third,ui->toolButton_forth,ui->toolButton_fifth};

}

void PageScrollWidget::InitStyle()
{

}

void PageScrollWidget::ResetButton()
{
    if(_p->totalPage <= 5)
    {//少于等于5页，直接显示5页按钮，其他隐藏
        for(int i = 0;i < _p->totalPage; ++i)
        {
            _p->numberButtons[i]->setText(QString::number(i+1));
        }
        for(int i = 0;i < 5 - _p->totalPage;++i)
        {
            _p->numberButtons[5-i]->setVisible(false);
        }
        ui->toolButton_pre->setVisible(false);
        ui->toolButton_next->setVisible(false);
    }
    else
    {//多余5页，显示1234+。。。
        for(int i = 0;i < 4;++i)
        {
            _p->numberButtons[i]->setText(QString::number(i+1));
        }
        _p->numberButtons[4]->setText("...");
        ui->toolButton_pre->setVisible(false);
        ui->toolButton_next->setVisible(true);
    }
}
