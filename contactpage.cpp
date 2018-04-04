#include "contactpage.h"
#include "ui_contactpage.h"
#include "wallet.h"

#include "editremarkdialog.h"
#include "addcontactdialog.h"

#include <QDebug>


ContactPage::ContactPage(QWidget *parent) :
    QWidget(parent),
    oldRemark(""),
    ui(new Ui::ContactPage)
{
    
    ui->setupUi(this);

    connect( UBChain::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    updateContactsList();


}

ContactPage::~ContactPage()
{
    
    delete ui;
    
}

void ContactPage::updateContactsList()
{
    if( !UBChain::getInstance()->contactsFile->open(QIODevice::ReadOnly))
    {
        qDebug() << "contact.dat not exist";
    }
    QByteArray rd = UBChain::getInstance()->contactsFile->readAll();
    UBChain::getInstance()->contactsFile->close();

    QString str =  QByteArray::fromBase64( rd);

    QStringList strList = str.split(";");
    strList.removeLast();
    int size = strList.size();

    if( size > 4)
    {
        ui->scrollAreaWidgetContents->setMinimumHeight(418 + (size - 4) * 71);
//        ui->scrollAreaWidgetContents->setGeometry(0,0,826, 397 + (size - 4) * 71);
    }
    else
    {
        ui->scrollAreaWidgetContents->setMinimumHeight(418);
    }

    if( size == 0)
    {
        ui->initLabel->show();
    }
    else
    {
        ui->initLabel->hide();
    }

    releaseVector();
    contactVector.clear();

    int row = 0;
    int column = 0;
    for( int i = 0; i < size; i++)
    {
        QString str2 = strList.at(i);
        SingleContactWidget* widget = new SingleContactWidget(i,str2.left( str2.indexOf("=")),str2.mid( str2.indexOf("=") + 1),ui->scrollAreaWidgetContents);
        widget->setAttribute(Qt::WA_DeleteOnClose);
        connect(widget,SIGNAL(deleteContact()),this,SLOT(updateContactsList()));
        connect(widget,SIGNAL(showShadowWidget()),this,SLOT(shadowWidgetShow()));
        connect(widget,SIGNAL(hideShadowWidget()),this,SLOT(shadowWidgetHide()));
        connect(widget,SIGNAL(editContact(QString,QString)),this,SLOT(editContact(QString,QString)));
        connect(widget,SIGNAL(gotoTransferPage(QString,QString)),this,SIGNAL(gotoTransferPage(QString,QString)));
        contactVector.insert(i,widget);
        contactVector.at(i)->show();

        row = i / 2;
        column = i % 2;
        widget->move( 30 + column * 320, 140 * row);
        widget->show();
    }

    ui->scrollAreaWidgetContents->setMinimumHeight(150 + 140 * row);    
}

void ContactPage::on_addContactBtn_clicked()
{

    AddContactDialog addContactDialog;
    addContactDialog.pop();

    updateContactsList();

}

void ContactPage::releaseVector()
{
    foreach (SingleContactWidget* widget, contactVector) {
        widget->close();
    }
}

void ContactPage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(40,46,66),Qt::SolidLine));
    painter.setBrush(QBrush(QColor(40,46,66),Qt::SolidPattern));
    painter.drawRect(0,0,680,482);
}


void ContactPage::shadowWidgetShow()
{
    emit showShadowWidget();
}

void ContactPage::shadowWidgetHide()
{
    emit hideShadowWidget();
}



void ContactPage::editContact(QString address, QString remark)
{
    
    oldRemark = remark;

    EditRemarkDialog editRemarkDialog( remark);
//    emit showShadowWidget();
    QString remarkName = editRemarkDialog.pop();
//    emit hideShadowWidget();

    if( oldRemark == remarkName)
    {
        return;
    }

    if( !UBChain::getInstance()->contactsFile->open(QIODevice::ReadWrite))
    {
        qDebug() << "contact.dat not exist";
        return;
    }

    QByteArray ba = QByteArray::fromBase64( UBChain::getInstance()->contactsFile->readAll());
    QString str(ba);
    QStringList strList = str.split(";");
    strList.removeLast();

#ifdef WIN32
    for(QStringList::iterator& ss = strList.begin();  ss != strList.end(); ss++)
#else
	QStringList::iterator ss = strList.begin();
    for(;  ss != strList.end(); ss++)
#endif
    {
        if(  (*ss).mid(0, (*ss).indexOf('=')) == address && (*ss).mid( (*ss).indexOf('=') + 1) ==  remarkName )
        {
            UBChain::getInstance()->contactsFile->close();
            return;
        }
        if( ((*ss).mid(0, (*ss).indexOf('=')) == address) && (*ss).mid( (*ss).indexOf('=') + 1) ==  oldRemark )
        {
            (*ss) = address + '=' + remarkName;
            ba = "";
            foreach (QString ss2, strList)
            {
                ba += ss2 + ';';
            }
            ba = ba.toBase64();
            UBChain::getInstance()->contactsFile->resize(0);
            QTextStream ts(UBChain::getInstance()->contactsFile);
            ts << ba;
            UBChain::getInstance()->contactsFile->close();

            updateContactsList();
            return;
        }
    }

    ba += address.toUtf8() + '=' + remarkName.toUtf8() + QString(";").toUtf8();
    ba = ba.toBase64();
    UBChain::getInstance()->contactsFile->resize(0);
    QTextStream ts(UBChain::getInstance()->contactsFile);
    ts << ba;

    UBChain::getInstance()->contactsFile->close();

    
}


void ContactPage::retranslator(QString language)
{
    ui->retranslateUi(this);
}

void ContactPage::jsonDataUpdated(QString id)
{
    if( id == "id_blockchain_get_account")
    {
        QString  result = UBChain::getInstance()->jsonDataValue(id);
        return;
    }

}
