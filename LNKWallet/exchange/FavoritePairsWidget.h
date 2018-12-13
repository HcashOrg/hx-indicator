#ifndef FAVORITEPAIRSWIDGET_H
#define FAVORITEPAIRSWIDGET_H

#include <QWidget>
#include <QToolButton>

namespace Ui {
class FavoritePairsWidget;
}

class FavoritePairsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FavoritePairsWidget( int _size, QWidget *parent = nullptr);
    ~FavoritePairsWidget();

    void init(); // 是否setcheck
    void setBtns(QStringList texts);
    void setCurrentBtn(QString text);
    void removeBtn(QString text);
    void adjustPosAndVisiable();

private slots:
    void onBtnClicked();
signals:
    void showPair(QString);

private:
    Ui::FavoritePairsWidget *ui;
    int size = 0;

    QStringList btnTexts;
    QString currentText;
    QVector<QToolButton*>   btnVector;
};

#endif // FAVORITEPAIRSWIDGET_H
