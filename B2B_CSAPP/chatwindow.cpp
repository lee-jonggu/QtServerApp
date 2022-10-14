#include "chatwindow.h"
#include "ui_chatwindow.h"

#include <QWidget>

#define BLOCK_SIZE  1024

ChatWindow::ChatWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);

    QLineEdit *serverPort = new QLineEdit(this);

    clientSocket = new QTcpSocket(this);
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug() << clientSocket->errorString();});
    connect(clientSocket, SIGNAL(readyRead()),this,SLOT(echoData()));
//    setWindowTitle("Echo Client");
}

ChatWindow::~ChatWindow()
{
    clientSocket->close();
}

void ChatWindow::on_pushButton_clicked()
{
    sendData();
    ui->inputLine->clear();
}


void ChatWindow::on_lineEdit_returnPressed()
{
    sendData();
    ui->inputLine->clear();
}

void ChatWindow::echoData()
{
    QTcpSocket *clientSocket = (QTcpSocket *)sender();
    if (clientSocket->bytesAvailable() > BLOCK_SIZE) return;
    QByteArray bytearray = clientSocket->read(BLOCK_SIZE);
    message->append(QString(bytearray));
}

void ChatWindow::sendData()
{
    QString str = inputLine->text();
    if(str.length())
    {
        QByteArray bytearray;
        bytearray = str.toUtf8();
        clientSocket->write(bytearray);
    }
    message->append("나 : " + str);
}
