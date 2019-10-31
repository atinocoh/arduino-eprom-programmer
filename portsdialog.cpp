#include "portsdialog.h"
#include "ui_portsdialog.h"

PortsDialog::PortsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortsDialog)
{
    ui->setupUi(this);
    ui->portsTextBox->setReadOnly(true);
    this->setFixedSize(QSize(512, 318));
}
void PortsDialog::setText(QString text){
    ui->portsTextBox->setText(text);
}
PortsDialog::~PortsDialog()
{
    delete ui;
}

void PortsDialog::on_closeButton_clicked()
{
    close();
}
