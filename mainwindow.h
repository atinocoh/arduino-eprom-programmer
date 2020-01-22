#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSaveFile>
#include <QFile>
#include <fstream>
#include <iostream>
#include "readingdialog.h"
using namespace std;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_epromSelector_activated(const QString &arg1);

    void on_readButton_clicked();

    void on_actionAbout_triggered();

    void on_writeButton_clicked();

    void on_selectButton_clicked();

    void on_scanButton_clicked();

    void on_blankButton_clicked();

    void on_verifyButton_clicked();

    void readSerial();

    void cancelReading();

    void writeEprom();

    void on_baudsComboBox_activated(int index);

private:

    void resetValues();

    qint32 getBaudRate();


    Ui::MainWindow *ui;
    bool isBlank;
    bool isEqual;
    bool cancelInProcess;
    int readMode; //0: read      1: blank check       2: verify
    const unsigned int romsize[2]={1048576, 32768};
    const char readCommands[2]={'r','z'};
    QSerialPort *serial;
    QFile *outputFile;
    QFile *inputFile;
    unsigned int total;
    ReadingDialog *r;
    QByteArray *writeData;
    bool writeOrder;
    unsigned long bauds;
};

#endif // MAINWINDOW_H
