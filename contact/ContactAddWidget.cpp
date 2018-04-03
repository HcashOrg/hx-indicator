#include "ContactAddWidget.h"
#include "ui_ContactAddWidget.h"

#include <QPainter>
#include <QRegExpValidator>
#include "ContactDataUtil.h"

#include "wallet.h"

Q_DECLARE_METATYPE(std::shared_ptr<ContactGroup>)

class ContactAddWidget::ContactAddWidgetPrivate
{
public:
    ContactAddWidgetPrivate(std::shared_ptr<ContactSheet> sheet)
        :person(std::make_shared<ContactPerson>())
        ,contactSheet(sheet)
    {

    }
public:
    std::shared_ptr<ContactSheet> contactSheet;//联系人数据表
    std::shared_ptr<ContactPerson> person;//联系人
};

ContactAddWidget::ContactAddWidget(std::shared_ptr<ContactSheet> contactSheet,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactAddWidget),
    _p(new ContactAddWidgetPrivate(contactSheet))
{
    ui->setupUi(this);
    InitWidget();
}

ContactAddWidget::~ContactAddWidget()
{
    delete _p;
    delete ui;
}

void ContactAddWidget::groupModifySlots()
{
    InitComboBox();
}

void ContactAddWidget::addressChangeSlots(const QString &address)
{
    ui->lineEdit_address->setText( ui->lineEdit_address->text().remove(" "));
    ui->lineEdit_address->setText( ui->lineEdit_address->text().remove("\n"));
    if( ui->lineEdit_address->text().isEmpty())
    {
        ui->label_tip->setVisible(false);
        ui->pushButton->setEnabled(false);
        return;
    }

    if( validateAddress(ui->lineEdit_address->text()))
    {
        ui->label_tip->setVisible(true);
        ui->label_tip->setText(tr("address available"));
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::green);
        ui->label_tip->setPalette(pa);
        ui->pushButton->setEnabled(true);
    }
    else
    {
        ui->label_tip->setVisible(true);
        ui->label_tip->setText(tr("address illegal or conflict "));
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::red);
        ui->label_tip->setPalette(pa);
        ui->pushButton->setEnabled(false);
    }
}

void ContactAddWidget::addNewContactSlots()
{
    std::shared_ptr<ContactGroup> group = ui->comboBox_group->currentData(Qt::UserRole).value<std::shared_ptr<ContactGroup>>();
    if(!group) return;

    _p->person = std::make_shared<ContactPerson>();
    _p->person->address = ui->lineEdit_address->text();
    _p->person->name = ui->lineEdit_remark->text().isEmpty()? _p->person->address : ui->lineEdit_remark->text();

    group->groupPeople.push_back(_p->person);
    emit addContactFinishedSignal(_p->person->address);
    ui->pushButton->setEnabled(false);
}

bool ContactAddWidget::validateAddress(const QString &address)
{
    if(!_p->contactSheet) return false;
    if(!_p->contactSheet->validateAddress(address)) return false;
    //调用全局检测函数
    AddressType type = checkAddress(address,AccountAddress | ContractAddress | MultiSigAddress | ScriptAddress);
    if( type == AccountAddress)
    {
        return true;
    }
    else
    {
        return false;
    }

}

void ContactAddWidget::InitStyle()
{
    //setAutoFillBackground(true);
    //QPalette palette;
    //palette.setColor(QPalette::Background, QColor(40,46,66));
    //setPalette(palette);

    //ui->pushButton->setStyleSheet("QPushButton{background-color:rgb(70,82,113);color:white;border:1px solid rgb(70,82,113);border-top-right-radius: 12px;border-bottom-right-radius: 12px;}");

}

void ContactAddWidget::InitWidget()
{
    InitStyle();

    ui->pushButton->setEnabled(false);

    QRegExp regx("[a-zA-Z0-9\-\.\ \n]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    ui->lineEdit_address->setValidator( validator );
    ui->lineEdit_address->setPlaceholderText( tr("Please enter an account address."));

    ui->lineEdit_remark->setPlaceholderText(tr("input note name.(empty input avaliable)"));

    ui->label_tip->setVisible(false);

    if(!_p->contactSheet) return;

    //初始化comboBox
    InitComboBox();

    connect(ui->lineEdit_address,&QLineEdit::textEdited,this,&ContactAddWidget::addressChangeSlots);
    connect(ui->pushButton,&QPushButton::clicked,this,&ContactAddWidget::addNewContactSlots);
}

void ContactAddWidget::InitComboBox()
{
    ui->comboBox_group->clear();
    for(auto it = _p->contactSheet->groups.begin();it != _p->contactSheet->groups.end();++it)
    {
        ui->comboBox_group->addItem((*it)->groupName,QVariant::fromValue<std::shared_ptr<ContactGroup>>(*it));
    }
}
