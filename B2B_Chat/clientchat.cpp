#include "clientchat.h"
#include "ui_clientchat.h"

#include <QByteArray>

#define BLOCK_SIZE  1024

ClientChat::ClientChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientChat)
{
    ui->setupUi(this);

    ui->ipAddressLineEdit->setText("127.0.0.1");
    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    QRegularExpressionValidator validator(re);
    ui->ipAddressLineEdit->setPlaceholderText("Server IP Address");
    ui->ipAddressLineEdit->setValidator(&validator);

    ui->portLineEdit->setText("8010");
    ui->portLineEdit->setInputMask("00000;_");
    ui->portLineEdit->setPlaceholderText("Server Port No");

    ui->clientIdLineEdit->setInputMask("0000;_");
    ui->clientIdLineEdit->setPlaceholderText("ID No");
    ui->clientNameLineEdit->setPlaceholderText("Your Name");

    clientSocket = new QTcpSocket(this);
//    clientSocket->connectToHost(ui->ipAddressLineEdit->text(), ui->portLineEdit->text().toInt());
//    connect(clientSocket, &QAbstractSocket::errorOccurred,
//            [=]{ qDebug() << clientSocket->errorString();});
//    connectToServer();
    connect(clientSocket, SIGNAL(readyRead()),this,SLOT(receiveData()));
}

ClientChat::~ClientChat()
{
    clientSocket->close();
}

void ClientChat::on_sendButton_clicked()
{
    sendData();
    ui->inputLine->clear();
}


void ClientChat::on_lineEdit_returnPressed()
{
    sendData();
    ui->inputLine->clear();
}

void ClientChat::receiveData()
{
    QTcpSocket *clientConnection = (QTcpSocket *)sender();
    if (clientConnection->bytesAvailable() > BLOCK_SIZE) return;
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);
    foreach(QTcpSocket *sock, clientList) {
        if (sock != clientConnection)
            sock->write(bytearray);
    }
    ui->textEdit->append(QString(bytearray));
}

void ClientChat::sendData()
{
    QString str = ui->inputLine->text();
    if(str.length())
    {
        QByteArray bytearray;
        bytearray = str.toUtf8();
        clientSocket->write(bytearray);
    }
    ui->textEdit->append("나 : " + str);
}

//void ClientChat::connectToServer()
//{
//    chatProtocolType data;
//    data.type = Server_In;
//    qstrcpy(data.data, name->text().toStdString().data());

//    QByteArray sendArray;
//    QDataStream out(&sendArray, QIODevice::WriteOnly);
//    out << data.type;
//    out.writeRawData(data.data, 1020);
//    clientSocket->write(sendArray);
//}


void ClientChat::on_connectPushButton_clicked()
{
    clientSocket->connectToHost(ui->ipAddressLineEdit->text(), ui->portLineEdit->text().toInt());
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug() << clientSocket->errorString();});

    chatProtocolType data;
    data.type = Server_In;
    qstrcpy(data.data, ui->clientNameLineEdit->text().toStdString().data());
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << data.type;
    out.writeRawData(data.data, 1020);
    clientSocket->write(sendArray);
}

