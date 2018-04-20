#include "PageScrollWidget.h"
#include "ui_PageScrollWidget.h"

class PageScrollWidget::PageScrollWidgetPrivate
{
public:
    PageScrollWidgetPrivate()
        :buttonNumber(5)
        ,totalPage(10)
        ,isShowPre(false)
        ,isShowNext(false)
        ,currentPage(0)
        ,isMoreClicked(false)
    {
        dataShow.resize(buttonNumber,-1);//-1表示不显示，-2表示。。。
    }
public:
    unsigned int buttonNumber;
    unsigned int totalPage;
    std::vector<QToolButton*> numberButtons;
    std::vector<int> dataShow;
    bool isShowPre;
    bool isShowNext;

    int currentPage;
    bool isMoreClicked;
};

PageScrollWidget::PageScrollWidget(unsigned int buttonNumber,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PageScrollWidget),
    _p(new PageScrollWidgetPrivate())
{
    ui->setupUi(this);
    _p->buttonNumber = std::max<unsigned int>(buttonNumber,3);
    InitWidget();
}

PageScrollWidget::~PageScrollWidget()
{
    delete _p;
    delete ui;
}

void PageScrollWidget::SetTotalPage(unsigned int number)
{
    _p->totalPage = std::max<unsigned int>(number,1);
    _p->currentPage = 0;

    QIntValidator *validator = new QIntValidator(1,_p->totalPage,this);
    ui->lineEdit->setValidator( validator );

    ResetButton();
    ui->lineEdit->setVisible(true);
    ui->lineEdit->setText("1");
}

void PageScrollWidget::buttonClickSlots()
{
    _p->isMoreClicked = false;
    QToolButton* button = dynamic_cast<QToolButton*>(sender());
    if(button == ui->toolButton_pre)
    {
        if(_p->currentPage > 0)
        {
            --_p->currentPage;            
        }
    }
    else if(button == ui->toolButton_next)
    {
        if(_p->currentPage+1 < _p->totalPage)
        {
            ++_p->currentPage;
        }
    }
    else
    {//找到点击了第几个翻页按钮
        size_t i = 0;
        for(;i < _p->numberButtons.size();++i)
        {
            if(button == _p->numberButtons[i])
            {
                break;
            }
        }
        _p->dataShow[i] != -2 ? _p->currentPage = _p->dataShow[i] : _p->isMoreClicked = true;
    }
    RefreshData();
    ui->lineEdit->setText(QString::number(_p->currentPage+1));
}

void PageScrollWidget::LineEditFinishSlots()
{
    _p->currentPage = ui->lineEdit->text().toInt()-1;
    RefreshData();
}

void PageScrollWidget::RefreshData()
{//根据当前页面，以及是否按下more按钮，刷新当前数据分组
    _p->isShowNext = true;
    _p->isShowPre = true;
    if(_p->currentPage == 0)
    {
        _p->isShowPre = false;
    }
    if(_p->currentPage+1 == _p->totalPage)
    {
        _p->isShowNext = false;
    }
    if(_p->dataShow.size() >= _p->totalPage)
    {
        _p->isShowNext = false;
        _p->isShowPre = false;
    }
    if(_p->isMoreClicked)
    {//切换morebutton位置RefreshButton
        SwitchMoreButton();
    }
    //设置页码排列
    RearrangePage();

    RefreshButton();
    emit currentPageChangeSignal(static_cast<unsigned int>(_p->currentPage));
}

void PageScrollWidget::RefreshButton()
{
    ui->toolButton_pre->setVisible(_p->isShowPre);
    ui->toolButton_next->setVisible(_p->isShowNext);
    for(size_t i = 0;i < _p->dataShow.size();++i)
    {
        if(_p->dataShow[i] == -1)
        {
            _p->numberButtons[i]->setVisible(false);
        }
        else if(_p->dataShow[i] == -2)
        {
            _p->numberButtons[i]->setText("...");
            _p->numberButtons[i]->setChecked(false);
            _p->numberButtons[i]->setVisible(true);
        }
        else
        {
            _p->numberButtons[i]->setText(QString::number(_p->dataShow[i]+1));
            _p->numberButtons[i]->setVisible(true);
            _p->numberButtons[i]->setChecked(_p->currentPage == _p->dataShow[i]);
        }
    }
}

void PageScrollWidget::SwitchMoreButton()
{
    //互换morebutton位置
    if(_p->dataShow.back() == -2)
    {
        _p->dataShow.clear();
        _p->dataShow.resize(_p->buttonNumber,-1);
        _p->dataShow[0] = 0;
        _p->dataShow[_p->dataShow.size()-1] = _p->totalPage-1;
        _p->dataShow[1] = -2;
        _p->dataShow[_p->dataShow.size()-2] = -2;
        //_p->dataShow[2] = _p->currentPage;
    }
    else
    {
        _p->dataShow.clear();
        _p->dataShow.resize(_p->buttonNumber,-1);
        _p->dataShow[_p->dataShow.size()-1] = -2;
    }
}

void PageScrollWidget::RearrangePage()
{
    for(size_t i = 0;i < _p->dataShow.size();++i)
    {
        if(_p->dataShow[i] == _p->currentPage)
        {
            return;
        }
    }
    //当前按钮没有该页码
    if(_p->dataShow[1] == -2)
    {
        _p->dataShow[2] = _p->currentPage;
    }
    else
    {
        int nu = _p->totalPage - _p->currentPage  ;
        if(nu >= _p->dataShow.size()-1 )
        {
            for(size_t i = 0;i < _p->dataShow.size()-1;++i)
            {
                _p->dataShow[i] = _p->currentPage + i;
            }
        }
        else
        {
            int x =  nu + 1 - _p->dataShow.size();
            for(size_t i = 0;i < _p->dataShow.size()-1;++i)
            {
                _p->dataShow[i] = _p->currentPage + x;
                ++x;
            }
        }

    }

}

void PageScrollWidget::InitWidget()
{
    InitStyle();
    ui->toolButton_next->setVisible(false);
    ui->toolButton_pre->setVisible(false);
    ui->lineEdit->setVisible(false);
    connect(ui->toolButton_next,&QToolButton::clicked,this,&PageScrollWidget::buttonClickSlots);
    connect(ui->toolButton_pre,&QToolButton::clicked,this,&PageScrollWidget::buttonClickSlots);
    connect(ui->lineEdit,&QLineEdit::editingFinished,this,&PageScrollWidget::LineEditFinishSlots);
}

void PageScrollWidget::InitStyle()
{
    setStyleSheet("QToolButton{\
                  max-height:20 px;\
                  max-width:20 px;\
                  min-height:20 px;\
                  min-width:20 px;\
                  font:Microsoft YaHei UI Light;\
                  font-size:12px;\
                  font-weight:580;\
                  border-top-left-radius:10px;  \
                  border-top-right-radius:10px; \
                  border-bottom-left-radius:10px;  \
                  border-bottom-right-radius:10px; \
                  border:1px solid;\
                  border-color:#CACACA;\
                  color:#CACACA;}"
                  "QToolButton:checked{border-color:#4861DC;\
                   color:#4861DC;}"
                  "QToolButton#toolButton_next{border:none;}QToolButton#toolButton_pre{border:none;}"
                  "QLineEdit{max-height:20 px;\
                   max-width:30 px;\
                   min-height:20 px;\
                   min-width:30 px;\
                   font:Microsoft YaHei UI Light;\
                   font-size:12px;\
                   font-weight:bold;\
                   border-top-left-radius:5px;  \
                   border-top-right-radius:5px; \
                   border-bottom-left-radius:5px;  \
                   border-bottom-right-radius:5px; \
                   border:1px solid;\
                   border-color:#CACACA;\
                   color:#CACACA;background-color:transparent;}"
                  );
}

void PageScrollWidget::ResetButton()
{
    for (int cc = ui->button_layout->count()-1; cc >= 0; --cc)
    {
        QLayoutItem *it = ui->button_layout->itemAt(cc);
        QWidget *orderHistory = qobject_cast<QWidget *>(it->widget());
        if (orderHistory != 0)
        {
            orderHistory->setParent(0);
            orderHistory->hide();
            ui->button_layout->removeWidget(orderHistory);
            delete orderHistory;
        }
    }
    _p->dataShow.clear();
    _p->dataShow.resize(_p->buttonNumber,-1);
    if(_p->totalPage <= _p->buttonNumber)
    {//少于等于按钮数页，直接显示按钮，其他隐藏
        for(int i = 0;i < _p->totalPage; ++i)
        {
            _p->dataShow[i] = i;
        }
        _p->isShowNext = false;
        _p->isShowPre = false;
    }
    else
    {//多余按钮数页，显0-按钮数-1  + 。。。
        for(int i = 0;i < _p->buttonNumber-1;++i)
        {
             _p->dataShow[i] = i;
        }
        _p->dataShow[_p->buttonNumber-1] = -2;
        _p->isShowNext = true;
        _p->isShowPre = false;
    }
    //创建按钮

    _p->numberButtons.clear();
    for(unsigned int i = 0; i < _p->buttonNumber;++i)
    {
        QToolButton *toolButton = new QToolButton(this);
        toolButton->setCheckable(true);
        connect(toolButton,&QToolButton::clicked,this,&PageScrollWidget::buttonClickSlots);
        ui->button_layout->addWidget(toolButton);
        _p->numberButtons.push_back(toolButton);
    }

    RefreshButton();
}
