/**
  * @brief setup widget class
  * @fn setting.cpp
  * @author shenxfsn@@163.com
  * @version V1.0.1
  */
#include "setting.h"
#include "ui_setting.h"
#include "mainwindow.h"
#include <QDir>

#include <QFileDialog>

/**
 * @brief Setting::Setting
 * @param parent
 */
Setting::Setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);

    coffPath_sub = QDir::homePath() + tr("/kyj1.out");
    outPath_sub = QDir::homePath() + tr("/kyj1");
    symQk = QString(tr("temperature_qk"));
    symQb = QString(tr("temperature_qb"));
    MainWindow *main =(MainWindow*)parent;
    if(main != NULL)
    {
        outPath_sub = main->outPath;
        coffPath_sub = main->coffPath;
        symQk = main->symbol_qk.mid(1);
        symQb = main->symbol_qb.mid(1);
    }
    ui->lineEdit->setText(coffPath_sub);
    ui->lineEdit_2->setText(outPath_sub);
    ui->lineEdit_3->setText(symQk);
    ui->lineEdit_4->setText(symQb);
}

/**
 * @brief Setting::~Setting
 */
Setting::~Setting()
{
    delete ui;
}

/**
 * @brief Setting::on_pushButton_clicked
 */
void Setting::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("打开.out文件"),QDir(coffPath_sub).absolutePath(),"*.out");
    if(fileName.size() !=0 )
    {
        coffPath_sub = fileName;
        ui->lineEdit->setText(fileName);
    }
}

/**
 * @brief Setting::on_pushButton_2_clicked
 */
void Setting::on_pushButton_2_clicked()
{
    QString pathName = QFileDialog::getExistingDirectory(this,tr("设置输出目录"),outPath_sub);
    if(pathName.size() !=0 )
    {
        outPath_sub = pathName + tr("/kyj1");
        ui->lineEdit_2->setText(pathName);
    }

}

