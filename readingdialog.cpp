#include "readingdialog.h"
#include "ui_readingdialog.h"
#include <QDebug>
#include <QSerialPort>

ReadingDialog::ReadingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReadingDialog)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    this->setFixedSize(QSize(401, 173));

}

void ReadingDialog::setProgress(int value){
    ui->progressBar->setValue(value);
}

void ReadingDialog::setBarColor(QColor color){
    QPalette p = palette();
    p.setColor(ui->progressBar->foregroundRole(), color);
    ui->progressBar->setPalette(p);
}
void ReadingDialog::setText(QString text, QColor color){

    QPalette palette = ui->info->palette();
    palette.setColor(ui->info->foregroundRole(), color);
    ui->info->clear();
    ui->info->setPalette(palette);
    ui->info->setText(text);
    ui->info->update();
    ui->info->show();

}
void ReadingDialog::setButtonText(QString text){
    ui->pushButton->setText(text);
}

void ReadingDialog::enableButton(){
    ui->pushButton->setEnabled(true);
}

ReadingDialog::~ReadingDialog()
{
    delete ui;
}

void ReadingDialog::on_pushButton_clicked()
{
    close();
}


