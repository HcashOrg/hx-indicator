#ifndef QRCODEDIALOG_H
#define QRCODEDIALOG_H

#include <QDialog>

namespace Ui {
class QRCodeDialog;
}

class QRCodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QRCodeDialog( QString address, QWidget *parent = 0);
    ~QRCodeDialog();

private:
    Ui::QRCodeDialog *ui;
};

#endif // QRCODEDIALOG_H
