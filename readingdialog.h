#ifndef READINGDIALOG_H
#define READINGDIALOG_H

#include <QDialog>
#include <QSerialPort>

namespace Ui {
class ReadingDialog;
}

class ReadingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReadingDialog(QWidget *parent = 0);
    void setProgress(int value);
    void setText(QString text, QColor color);
    void setButtonText(QString text);
    void setBarColor(QColor color);
    void enableButton();
    ~ReadingDialog();

private slots:
    void on_pushButton_clicked();



private:
    Ui::ReadingDialog *ui;

};

#endif // READINGDIALOG_H
