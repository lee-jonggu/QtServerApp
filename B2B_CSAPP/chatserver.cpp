#include "chatserver.h"
#include "ui_chatserver.h"

#include <QtGui>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QtNetwork>

#define BLOCK_SIZE  1024

ChatServer::ChatServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatServer)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()),SLOT(clientConnect()));
    if (tcpServer->listen(QHostAddress::Any, 8000))
    {
        QMessageBox::critical(this, tr("Echo Server"),tr("Unable to start the server : %1.").arg(tcpServer->errorString()));
        close();
        return;
    }

    infoLabel->setText(tr("The server is running on port %1.").arg(tcpServer->serverPort()));
    setWindowTitle(tr("Echo Server"));
}

ChatServer::~ChatServer()
{
    delete ui;
}

void ChatServer::clientConnect()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),clientConnection, SLOT(deleteLater()));
    connect(clientConnection, SIGNAL(readyRead()),this,SLOT(echoData()));
    infoLabel->setText("new connection is established...");

    clientList.append(clientConnection);
}

void ChatServer::echoData()
{
    QTcpSocket *clientConnection = (QTcpSocket *)sender();
    if (clientConnection->bytesAvailable() > BLOCK_SIZE) return;
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);
//    clientConnection->write(bytearray);
//    infoLabel->setText(QString(bytearray));
    foreach(QTcpSocket *sock, clientList) {
        if (sock != clientConnection)
            sock->write(bytearray);
    }
    infoLabel->setText(QString(bytearray));
}
