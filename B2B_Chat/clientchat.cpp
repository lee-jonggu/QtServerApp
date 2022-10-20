#include "clientchat.h"
#include "ui_clientchat.h"

#include <QByteArray>

#define BLOCK_SIZE  1024

ClientChat::ClientChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientChat)
{
    ui->setupUi(this);

    ui->inputLine->setDisabled(true);

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

    ui->chatPushButton->setDisabled(true);
    ui->chatOutPushButton->setDisabled(true);
    ui->disConnectPushButton->setDisabled(true);

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

    chatProtocolType data;                                          // 프로토콜타입 유형
    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in >> data.type;
    in.readRawData(data.data, 1020);

    foreach(QTcpSocket *sock, clientList) {
        if (sock != clientConnection)
            sock->write(bytearray);
    }
    ui->textEdit->append(QString(data.data));
}

void ClientChat::sendData()
{
    QString str = ui->inputLine->text();
    if(str.length())
    {
        QByteArray bytearray;
        bytearray = str.toUtf8();
        ui->textEdit->append("나 : " + str);

        sendProtocol(Chat_Talk, bytearray.data());
    }
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
    qstrcpy(data.data, ui->clientIdLineEdit->text().toStdString().data());
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << data.type;
    out.writeRawData(data.data, 1020);
    clientSocket->write(sendArray);

    ui->connectPushButton->setDisabled(true);
    ui->connectPushButton->setText("connected");
    ui->disConnectPushButton->setDisabled(false);

    ui->chatPushButton->setDisabled(false);

    ui->ipAddressLineEdit->setReadOnly(true);
    ui->portLineEdit->setReadOnly(true);
    ui->clientIdLineEdit->setReadOnly(true);
    ui->clientNameLineEdit->setReadOnly(true);
}

void ClientChat::sendProtocol(Chat_Status type, char* data, int size)
{
    QByteArray sendArray;           // 소켓으로 보낼 데이터를 채우고
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out.device()->seek(0);
    out << type;
    out.writeRawData(data, size);
    clientSocket->write(sendArray);     // 서버로 전송
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());
}

void ClientChat::on_chatPushButton_clicked()
{
    sendProtocol(Chat_In, ui->clientIdLineEdit->text().toStdString().data());
    ui->inputLine->setDisabled(false);
    ui->chatPushButton->setDisabled(true);
    ui->chatOutPushButton->setDisabled(false);
    ui->disConnectPushButton->setDisabled(true);
    ui->connectPushButton->setDisabled(true);
    ui->clientIdLineEdit->setReadOnly(true);
    ui->clientNameLineEdit->setReadOnly(true);
}

void ClientChat::on_chatOutPushButton_clicked()
{
    sendProtocol(Chat_Out, ui->clientIdLineEdit->text().toStdString().data());
    ui->inputLine->setDisabled(true);
    ui->chatOutPushButton->setDisabled(true);
    ui->chatPushButton->setDisabled(false);
    ui->disConnectPushButton->setDisabled(false);
}

void ClientChat::on_disConnectPushButton_clicked()
{
    sendProtocol(Server_Out, ui->clientIdLineEdit->text().toStdString().data());
    ui->disConnectPushButton->setDisabled(true);
    ui->connectPushButton->setDisabled(false);
    ui->chatPushButton->setDisabled(true);
    ui->clientIdLineEdit->setReadOnly(false);
    ui->clientNameLineEdit->setReadOnly(false);
    ui->ipAddressLineEdit->setReadOnly(false);
    ui->portLineEdit->setReadOnly(false);
}



