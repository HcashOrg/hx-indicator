#ifndef CITIZENPOLICYPAGE_H
#define CITIZENPOLICYPAGE_H

#include <QWidget>
#include <QMap>

namespace Ui {
class CitizenPolicyPage;
}

struct ResolutionInfo
{
    QString id;
    QString expirationTime;
    QString title;
    QMap<int,QString>   optionsMap;
    QMap<int,QStringList>   resultMap;
    QMap<int,QString>   pledgeMap;
};
Q_DECLARE_METATYPE(ResolutionInfo)


class CitizenPolicyPage : public QWidget
{
    Q_OBJECT

public:
    explicit CitizenPolicyPage(QWidget *parent = nullptr);
    ~CitizenPolicyPage();

    void init();

    void refresh();

private slots:
    void jsonDataUpdated(QString id);

    void on_accountComboBox_currentIndexChanged(const QString &arg1);

    void on_resolutionTableWidget_cellClicked(int row, int column);

private:
    Ui::CitizenPolicyPage *ui;
    bool inited = false;

    void paintEvent(QPaintEvent*);
    void showResolutions();
    QString calProposalWeight(const ResolutionInfo &info) const;
    int searchOptionByAddress(const ResolutionInfo &info, QString address);
};

#endif // CITIZENPOLICYPAGE_H
