#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QThread>
#include <QFile>

#include "portsdialog.h"
#include "readingdialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setFixedSize(QSize(852, 491));
    ui->setupUi(this);

    total=0;
    isBlank=true;
    isEqual=true;
    cancelInProcess=false;
    serial=NULL;
    outputFile=NULL;
    inputFile=NULL;
    r=NULL;
    writeData=NULL;
    writeOrder=false;

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_epromSelector_activated(const QString &arg1)
{
    if(ui->epromSelector->currentIndex()==0){
            QPixmap mypix("graphics/m27c801pos.png");
            ui->chipPosition->setPixmap(mypix);
            QPixmap mypixj("graphics/jumper_pos2_snes.png");
            ui->jumperPosition->setPixmap(mypixj);
            QPixmap mypixl("graphics/m27c801.png");
            ui->chipLayout->setPixmap(mypixl);
    }
    if(ui->epromSelector->currentIndex()==1){
            QPixmap mypix2("graphics/M5L27256Kpos.png");
            ui->chipPosition->setPixmap(mypix2);
            QPixmap mypixj("graphics/jumper_pos1.png");
            ui->jumperPosition->setPixmap(mypixj);
            QPixmap mypixl("graphics/M5L27256K.png");
            ui->chipLayout->setPixmap(mypixl);
    }

}


qint32 MainWindow::getBaudRate(){
    switch(ui->baudsComboBox->currentIndex()){
        case 0:
            return QSerialPort::Baud9600;
        break;
        case 1:
            return QSerialPort::Baud19200;
        break;
        case 2:
            return QSerialPort::Baud38400;
        break;
        case 3:
            return QSerialPort::Baud57600;
        break;
        case 4:
            return QSerialPort::Baud115200;
        break;
        case 5:
            return 250000;
        break;
    }
    return QSerialPort::Baud9600;
}

void MainWindow::on_readButton_clicked()
{
    resetValues();

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save EPROM content"), ".", "All Files (*)");
    outputFile = new QFile(fileName);
    if (outputFile->open(QIODevice::ReadWrite)){
        serial = new QSerialPort(this);
        serial->setPortName(ui->portComboBox->currentText());
        serial->open(QIODevice::ReadWrite);
        if(serial->isOpen()){
            serial->setBaudRate(getBaudRate());
            serial->setDataBits(QSerialPort::Data8);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
            QObject::connect(serial,SIGNAL(readyRead()),this,SLOT(readSerial()));
            QThread::msleep(500);
            serial->clear(QSerialPort::AllDirections);
            serial->write("U",1);
            serial->write(&readCommands[ui->epromSelector->currentIndex()],1);
            //qDebug()<<readCommands[ui->epromSelector->currentIndex()];
            r = new ReadingDialog(this);
            r->show();
            r->setText("Reading EPROM content...",Qt::darkGreen);
            connect(r,SIGNAL(rejected()),this,SLOT(cancelReading()));

            readMode=0;
        }
        else{
            QMessageBox::information(0,"Error !!","Could not open port");
        }
    }
    else{
        outputFile->close();
        QMessageBox::information(0,"Error !!","Could not open file");
    }
}

void MainWindow::readSerial()
{
        //qDebug()<<"entramos en read";
    if(!cancelInProcess){
        QByteArray serialData;
        serialData = serial->readAll();
        QString message;
        unsigned int size = romsize[ui->epromSelector->currentIndex()];
        //qDebug()<<"Recibidos: "<< serialData.size()<<" bytes";
        //qDebug()<<"Total: "<<total;
        //r->setText("Reading EPROM content...",Qt::darkGreen);

        switch(readMode){
            case 0:
                message = "Reading EPROM content...  ";
                message = message + "byte " + QString::number(total) + "/" + QString::number(size);
                r->setText(message,Qt::darkGreen);
                outputFile->write(serialData);
            break;
            case 1:
                for (int var = 0; var < serialData.size(); ++var) {
                    message = "Verifying EPROM content...  ";
                    message = message + "byte " +  QString::number(total+var) + "/" +  QString::number(size);

                    r->setText(message,Qt::darkGreen);
                    if (/*false*/(unsigned char)serialData.at(var)!=255){
                        serial->write("c",1);
                        isBlank=false;
                        QString message = "The chip seems to contain data in byte: ";
                        message = message + QString::number(total+var);
                        r->setText(message,Qt::red);
                        cancelReading();
                        r->enableButton();
                    }
                }
            break;
            case 2:
                for (int var = 0; var < serialData.size()&&isEqual; ++var) {
                    message = "Comparing EPROM content...  ";
                    message = message + "byte " +  QString::number(total+var) + "/" +  QString::number(size);
                    r->setText(message,Qt::darkGreen);
                    char datafile;
                    inputFile->read(&datafile,1);
                    if (serialData.at(var)!=datafile){
                        serial->write("c",1);
                        isEqual=false;
                        QString message = "Found mismatch at byte: ";
                        message = message + QString::number(total+var);
                        message = message + "\nEPROM byte: " + QString::number((unsigned char)serialData.at(var)) + " file byte: " + QString::number((unsigned char)datafile);
                        r->setText(message,Qt::red);
                        cancelReading();
                        inputFile->close();
                        r->enableButton();
                    }
                }
            break;

        }

        total=total+serialData.size();
        r->setProgress((total*100)/size);
        if (total==size){
            serial->close();
            switch(readMode){
            case 0:
                r->setText("        EPROM read succesful",Qt::darkGreen);
                outputFile->close();
                break;
            case 1:
                if (isBlank){
                    r->setText("        The chip is blank",Qt::darkGreen);
                    if (writeOrder){
                        connect(r,SIGNAL(rejected()),this,SLOT(writeEprom()));
                        r->setButtonText("Start burning");
                    }
                    else{
                        QMessageBox::information(0,"Error !!","Impossible to burn EPROM: Chip not empty!");
                    }
                }
                break;
            case 2:
                if (isEqual)
                    r->setText("        EPROM content is equal to file",Qt::darkGreen);
                break;
            }

            r->enableButton();
            total=0;
            readMode=0;
            isBlank=true;
            isEqual=true;
            if (serial->isOpen())
                serial->close();
        }
    }
    else{
        serial->readAll();
    }
    //qDebug()<<"Sale de read";
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog *d;
    d=new AboutDialog(this);
    d->show();
}

void MainWindow::on_writeButton_clicked()
{
    if (writeData!=NULL&&writeData->size()==(int)romsize[ui->epromSelector->currentIndex()]){
        writeOrder=true;
        on_blankButton_clicked();
    }
    else{
        QMessageBox::information(0,"Error !!","Binary data does not correspond with EPROM size!");
    }

}
void MainWindow::writeEprom(){
    //qDebug()<<"writing";

    //serial->clear(QSerialPort::AllDirections);
    r = new ReadingDialog(this);
    r->setWindowTitle("Burning EPROM");
    r->setBarColor(Qt::red);
    r->show();

    //TODO: writting loop SORRY!! Its under development!!
}

void MainWindow::on_selectButton_clicked()
{

    if (writeData!=NULL)
        writeData->clear();
    ui->FilePath->clear();

    QString filename = QFileDialog::getOpenFileName(this,tr("Select BIN File"),".","All files (*.*)");
    this->ui->FilePath->setPlainText(filename);
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(0,"Error !!",file.errorString());
    }
    else{

        ui->FilePath->setPlainText(filename);
        writeData= new QByteArray();
        *writeData = file.readAll();
        QString message = "Binary size is: ";
        message = message + QString::number(writeData->size()) + " bytes";
        ui->binary_textbox->setText(message);
        ui->writeButton->setEnabled(true);
       // qDebug()<<"Leidos "<<writeData->size()<<" bytes";
    }
    //QTextStream in(&file);
    //ui->GngFileBrowser->setText(in.readAll());
}

void MainWindow::on_scanButton_clicked()
{
    ui->portComboBox->clear();
    QString text;

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
            text = text + "Name  :" + info.portName() + "\n";
            text = text + "Description  :" + info.description() + "\n";
            text = text + "Manufactuer :"  + info.manufacturer() + "\n";

            QSerialPort serial;
            serial.setPort(info);

                ui->portComboBox->addItem(info.portName());

            text = text + "-----------------------------------\n";

            serial.close();
            if(ui->portComboBox->depth()>0){
                    ui->portComboBox->setEnabled(true);
                    ui->baudsComboBox->setEnabled(true);
                    ui->blankButton->setEnabled(true);
                    ui->readButton->setEnabled(true);
                    ui->verifyButton->setEnabled(true);
            }

    }
    PortsDialog *d;
    d=new PortsDialog(this);
    d->setText(text);
    d->show();
}

void MainWindow::on_blankButton_clicked()
{
#ifdef DEBUG
    qDebug()<<"Entra en blank";
#endif
    //ser = serial.Serial('/dev/ttyACM3', 250000, timeout=0)
    //time.sleep(10);#my arduino bugs if data is written to the port after opening it
    //same as reading
    resetValues();
    serial = new QSerialPort(this);
    serial->setPortName(ui->portComboBox->currentText());

    if (serial->open(QIODevice::ReadWrite)){
        serial->setBaudRate(getBaudRate());
        serial->setDataBits(QSerialPort::Data8);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);

        QObject::connect(serial,SIGNAL(readyRead()),this,SLOT(readSerial()));
        QThread::msleep(500);
        serial->clear(QSerialPort::AllDirections);
        serial->write("U",1);
        serial->write(&readCommands[ui->epromSelector->currentIndex()],1);

        r = new ReadingDialog(this);
        r->show();
        connect(r,SIGNAL(rejected()),this,SLOT(cancelReading()));
        r->setText("Reading from EPROM...",Qt::darkGreen);
        readMode=1;
    }
    else{
          QMessageBox::information(0,"Error !!","Could not open port");
    }
    #ifdef DEBUG
    qDebug()<<"sale de blank";
    #endif
}

void MainWindow::cancelReading(){
    cancelInProcess=true;
}

void MainWindow::resetValues(){
    total=0;
    cancelInProcess=false;
    //if (outputFile.is_open())
      //      outputFile.close();
    //if (inputFile!=NULL){
      //  if(inputFile->isOpen())
        //   inputFile->close();

    readMode=0;
    isEqual=true;
    isBlank=true;
    if (serial!=NULL && serial->isOpen())
        serial->close();

}



void MainWindow::on_verifyButton_clicked()
{
    resetValues();

    QString filename = QFileDialog::getOpenFileName(this,tr("Select BIN File to compare"),".","All files (*.*)");
    //this->ui->FilePath->setPlainText(filename);
    inputFile= new QFile(filename);
    if (!inputFile->open(QIODevice::ReadOnly)){
        QMessageBox::information(0,"Error !!",inputFile->errorString());
    }
    else{
        serial = new QSerialPort(this);
        serial->setPortName(ui->portComboBox->currentText());
        serial->open(QIODevice::ReadWrite);
        serial->setBaudRate(getBaudRate());
        serial->setDataBits(QSerialPort::Data8);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        QObject::connect(serial,SIGNAL(readyRead()),this,SLOT(readSerial()));
        QThread::msleep(500);
        serial->clear(QSerialPort::AllDirections);
        serial->write("U",1);
        serial->write(&readCommands[ui->epromSelector->currentIndex()],1);
        r = new ReadingDialog(this);
        r->show();
        connect(r,SIGNAL(rejected()),this,SLOT(cancelReading()));
        isEqual=true;
        readMode=2;
    }

}
