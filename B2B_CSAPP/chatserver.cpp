#include "chatserver.h"
#include "ui_chatserver.h"

#include <QtGui>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QtNetwork>
#include <QTcpServer>
#include <QTcpSocket>

#define BLOCK_SIZE  1024

ChatServer::ChatServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatServer)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()),SLOT(clientConnect()));
    if (!tcpServer->listen(QHostAddress::Any, 8010))
    {
        QMessageBox::critical(this, tr("Echo Server"),tr("Unable to start the server : %1.").arg(tcpServer->errorString()));
        close();
        return;
    }

//    infoLabel->setText(tr("The server is running on port %1.").arg(tcpServer->serverPort()));
}

ChatServer::~ChatServer()
{
    delete ui;
}

void ChatServer::clientConnect()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()), SLOT(removeClient()));
    connect(clientConnection, SIGNAL(readyRead()),this,SLOT(receiveData()));

    QString ip = clientConnection->peerAddress().toString();
    quint16 port = clientConnection->peerPort();
    clientConnection->peerPort();
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0,ip);
    item->setText(1,QString::number(port));
    item->setText(2,0);
    item->setText(3,0);
    item->setText(4,"new connection is established...");
    item->setText(5,QTime::currentTime().toString());
    ui->logTreeWidget->addTopLevelItem(item);

    clientList.append(clientConnection);
}

void ChatServer::receiveData()
{
    QTcpSocket *clientConnection = (QTcpSocket *)sender();
    if (clientConnection->bytesAvailable() > BLOCK_SIZE) return;
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);

    chatProtocolType data;                                          // 프로토콜타입 유형
    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in >> data.type;
    in.readRawData(data.data, 1020);

    QString ip = clientConnection->peerAddress().toString();
    quint16 port = clientConnection->peerPort();

    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0,ip);
    item->setText(1,QString::number(port));
    item->setText(2,0);
    item->setText(3,0);
    item->setText(4,bytearray);
    item->setToolTip(4,bytearray);
    item->setText(5,QTime::currentTime().toString());
    ui->logTreeWidget->addTopLevelItem(item);

    switch(data.type) {
    case Server_In:
        foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchFixedString, 1))
        {

        }
    }

    foreach(QTcpSocket *sock, clientList) {
        if (sock != clientConnection)
            sock->write(bytearray);
    }


}

void ChatServer::showServerClient(QTreeWidgetItem* item)
{
    ui->totalClientTreeWidget->addTopLevelItem(item);
}

void ChatServer::on_pushButton_clicked()
{
    emit clickedUpdate();
}

void ChatServer::removeClient()
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    clientList.removeOne(clientConnection);
    clientConnection->deleteLater();

    QString name = clientNameHash[clientConnection->peerAddress().toString()];
    foreach(auto item, ui->clientTreeWidget->findItems(name, Qt::MatchContains, 1)) {
        item->setText(0, "X");
    }
}

