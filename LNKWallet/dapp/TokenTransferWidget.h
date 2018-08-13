#ifndef TOKENTRANSFERWIDGET_H
#define TOKENTRANSFERWIDGET_H

#include <QWidget>

namespace Ui {
class TokenTransferWidget;
}

class TokenTransferWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TokenTransferWidget(QWidget *parent = 0);
    ~TokenTransferWidget();

private:
    Ui::TokenTransferWidget *ui;
};

#endif // TOKENTRANSFERWIDGET_H
