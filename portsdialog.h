#ifndef PORTSDIALOG_H
#define PORTSDIALOG_H

#include <QDialog>

namespace Ui {
class PortsDialog;
}

class PortsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PortsDialog(QWidget *parent = 0);
    void setText(QString text);
    ~PortsDialog();

private slots:
    void on_closeButton_clicked();

private:
    Ui::PortsDialog *ui;
};

#endif // PORTSDIALOG_H
