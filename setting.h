#ifndef SETTING_H
#define SETTING_H

#include <QDialog>

namespace Ui {
class Setting;
}

class Setting : public QDialog
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = 0);
    ~Setting();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();


private:
    Ui::Setting *ui;
public:
    bool isSetting_sub;
    QString outPath_sub;
    QString coffPath_sub;
    QString symQk;
    QString symQb;
};

#endif // SETTING_H
