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
//    QTreeWidgetItem *item = new QTreeWidgetItem;
//    item->setText(0,ip);
//    item->setText(1,QString::number(port));
//    item->setText(2,0);
//    item->setText(3,0);
//    item->setText(4,"new connection is established...");
//    item->setText(5,QTime::currentTime().toString());
//    ui->logTreeWidget->addTopLevelItem(item);
    // 얘를 클라이언트를 대기실로 옮기면 된다

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
    QString id = QString::fromStdString(data.data);



    foreach(auto item, ui->totalClientTreeWidget->findItems(id,Qt::MatchFixedString,0))
    {
        clientName = item->text(1);
    }

    switch(data.type) {
    case Server_In:
        foreach(auto item, ui->totalClientTreeWidget->findItems(id, Qt::MatchFixedString, 0))
        {
            QTreeWidgetItem *enterItem = new QTreeWidgetItem;
//            QString id = item->text(0);
//            QString name = item->text(1);
            enterItem->setText(0,id);
            enterItem->setText(1,clientName);
            ui->loginClientTreeWidget->addTopLevelItem(enterItem);
            clientList.append(clientConnection);        // QList<QTcpSocket*> clientList;
            clientSocketHash[clientName] = clientConnection;
            clientNameHash[port] = clientName;
            clientIdHash[clientConnection] = id.toInt();
            clientId = id;

            QTreeWidgetItem *items = new QTreeWidgetItem;
            items->setText(0,ip);
            items->setText(1,QString::number(port));
//            items->setText(2,QString::number(clientIdHash[clientNameHash[port]]));
            items->setText(2,QString::number(clientIdHash[clientConnection]));
            items->setText(3,clientNameHash[port]);
//            items->setText(4,"ID : " + QString::number(clientIdHash[clientNameHash[port]]) + " Name : " + clientNameHash[port]\
//                    + " LogIn");
//            items->setToolTip(4,"ID : " + QString::number(clientIdHash[clientNameHash[port]]) + " Name : " + clientNameHash[port]\
//                    + " LogIn");
            items->setText(4,"ID : " + QString::number(clientIdHash[clientConnection]) + " Name : " + clientNameHash[port]\
                    + " Server In");
            items->setToolTip(4,"ID : " + QString::number(clientIdHash[clientConnection]) + " Name : " + clientNameHash[port]\
                    + " Server In");
            items->setText(5,QDateTime::currentDateTime().toString());
            ui->logTreeWidget->addTopLevelItem(items);
        }
        break;
    case Chat_In:
        foreach(auto item, ui->totalClientTreeWidget->findItems(id, Qt::MatchFixedString, 0))
        {
            QTreeWidgetItem *enterItem = new QTreeWidgetItem;
            QString id = item->text(0);
            QString name = item->text(1);
            enterItem->setText(0,id);
            enterItem->setText(1,clientName);
            ui->enteredTreeWidget->addTopLevelItem(enterItem);
            clientNameHash[port] = name;

            QTreeWidgetItem *items = new QTreeWidgetItem;
            items->setText(0,ip);
            items->setText(1,QString::number(port));
            items->setText(2,clientId);
            items->setText(3,clientName);
            items->setText(4,"ID : " + clientId + " Name : " + clientName\
                    + " Chatting In");
            items->setToolTip(4,"ID : " + clientId + " Name : " + clientName\
                    + " Chatting In");
            items->setText(5,QDateTime::currentDateTime().toString());
            ui->logTreeWidget->addTopLevelItem(items);
        }

        break;
    case Chat_Talk: {
        foreach(QTcpSocket *sock, clientList) {
            if(clientNameHash.contains(sock->peerPort()) && sock != clientConnection) {
                QByteArray sendArray;
                sendArray.clear();
                QDataStream out(&sendArray, QIODevice::WriteOnly);
                out << Chat_Talk;
//                sendArray.append("<font color=lightsteelblue>");
                sendArray.append(clientNameHash[port].toStdString().data());
                sendArray.append(" : ");
                sendArray.append(id.toStdString().data());
                sock->write(sendArray);
            }
        }

        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0,ip);
        item->setText(1,QString::number(port));
        item->setText(2,clientId);
        item->setText(3,clientNameHash[port]);
        item->setText(4,QString(data.data));
        item->setToolTip(4,QString(data.data));
        item->setText(5,QDateTime::currentDateTime().toString());
        ui->logTreeWidget->addTopLevelItem(item);

        for(int i = 0; i < ui->logTreeWidget->columnCount(); i++)
            ui->logTreeWidget->resizeColumnToContents(i);

    }
        break;
    case Chat_Out:
        foreach(auto item, ui->enteredTreeWidget->findItems(id, Qt::MatchFixedString, 0))
        {
            ui->enteredTreeWidget->takeTopLevelItem(ui->enteredTreeWidget->indexOfTopLevelItem(item));
            clientNameHash.remove(port);

            QTreeWidgetItem *items = new QTreeWidgetItem;
            items->setText(0,ip);
            items->setText(1,QString::number(port));
            items->setText(2,clientId);
            items->setText(3,clientName);
//            items->setText(4,"ID : " + QString::number(clientIdHash[clientConnection]) + " Name : " + clientNameHash[port]\
//                    + " Chatting Out");
//            items->setToolTip(4,"ID : " + QString::number(clientIdHash[clientConnection]) + " Name : " + clientNameHash[port]\
//                    + " Chatting Out");
            items->setText(4,"ID : " + clientId + " Name : " + clientName\
                    + " Chatting Out");
            items->setToolTip(4,"ID : " + clientId + " Name : " + clientName\
                    + " Chatting Out");
            items->setText(5,QDateTime::currentDateTime().toString());
            ui->logTreeWidget->addTopLevelItem(items);
        }
        break;

    case Server_Out:
        foreach(auto item, ui->loginClientTreeWidget->findItems(id, Qt::MatchFixedString, 0))
        {
            ui->loginClientTreeWidget->takeTopLevelItem(ui->loginClientTreeWidget->indexOfTopLevelItem(item));
            clientSocketHash.remove(clientName);

            QTreeWidgetItem *items = new QTreeWidgetItem;
            items->setText(0,ip);
            items->setText(1,QString::number(port));
            items->setText(2,clientId);
            items->setText(3,clientName);
            items->setText(4,"ID : " + clientId + " Name : " +clientName\
                    + " Server Out");
            items->setToolTip(4,"ID : " + clientId + " Name : " + clientName\
                    + " Server Out");
            items->setText(5,QDateTime::currentDateTime().toString());
            ui->logTreeWidget->addTopLevelItem(items);
        }
        break;
    }

}

void ChatServer::showIdName(int id,QString name)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0,QString::number(id));
    item->setText(1,name);
    ui->totalClientTreeWidget->addTopLevelItem(item);
//    clientIdHash[name] = id;
}

void ChatServer::removeIdName(int id,int index)
{
    Q_UNUSED(id);
    ui->totalClientTreeWidget->takeTopLevelItem(index);
}

void ChatServer::showServerClient(QTreeWidgetItem* item)
{
    ui->totalClientTreeWidget->addTopLevelItem(item);
}
