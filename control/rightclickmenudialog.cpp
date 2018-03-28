#include "rightclickmenudialog.h"
#include "ui_rightclickmenudialog.h"
#include "../lnk.h"
#include "commondialog.h"

#ifdef WIN32
#include "Windows.h"
#endif

#include <QDebug>
#include <QClipboard>
#include <QPainter>

RightClickMenuDialog::RightClickMenuDialog( QString name, QWidget *parent) :
    QDialog(parent),
    accountName(name),
    ui(new Ui::RightClickMenuDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);


    if( UBChain::getInstance()->registerMapValue(name) != "NO" )  // 如果是已注册账户 不显示修改账户名选项
    {
        ui->renameBtn->hide();
        ui->exportBtn->move(1,59);
        ui->deleteBtn->move(1,88);
        setGeometry(0,0,87,119);
    }

}

RightClickMenuDialog::~RightClickMenuDialog()
{
    delete ui;
}

void RightClickMenuDialog::on_transferBtn_clicked()
{
    close();
    emit transferFromAccount(accountName);
}

void RightClickMenuDialog::on_renameBtn_clicked()
{
    close();
    emit renameAccount(accountName);
}

void RightClickMenuDialog::on_exportBtn_clicked()
{
    close();
    emit exportAccount(accountName);
}

bool RightClickMenuDialog::event(QEvent *event)
{
#ifdef WIN32
    // class_ameneded 不能是custommenu的成员, 因为winidchange事件触发时, 类成员尚未初始化
    static bool class_amended = false;
    if (event->type() == QEvent::WinIdChange)
    {
        HWND hwnd = reinterpret_cast<HWND>(winId());
        if (class_amended == false)
        {
            class_amended = true;
            DWORD class_style = ::GetClassLong(hwnd, GCL_STYLE);
            class_style &= ~CS_DROPSHADOW;
            ::SetClassLong(hwnd, GCL_STYLE, class_style); // windows系统函数
        }
    }
#endif
	
    return QWidget::event(event);
}

void RightClickMenuDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(70,82,113),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(70,82,113),Qt::SolidPattern));
    painter.drawRect(0,0,87,148);
}

void RightClickMenuDialog::on_copyBtn_clicked()
{
    close();
    QString address = UBChain::getInstance()->addressMap.value(accountName).ownerAddress;
    QClipboard* clipBoard = QApplication::clipboard();
    clipBoard->setText(address);
    CommonDialog commonDialog(CommonDialog::OkOnly);
    commonDialog.setText(tr("Copy to clipboard"));
    commonDialog.pop();
}

void RightClickMenuDialog::on_deleteBtn_clicked()
{
    close();
    emit deleteAccount(accountName);
}
