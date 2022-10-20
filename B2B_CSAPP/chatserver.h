#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QWidget>
#include <QLabel>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTreeWidgetItem>

namespace Ui {
class ChatServer;
}

typedef enum {
    Server_In,
    Chat_In,
    Chat_Talk,
    Chat_Out,
    Server_Out
} Chat_Status;

typedef struct {
    Chat_Status type;
    char data[1020];
} chatProtocolType;

class ChatServer : public QWidget
{
    Q_OBJECT

public:
    explicit ChatServer(QWidget *parent = nullptr);
    ~ChatServer();

public slots:
    void showIdName(int,QString);
    void removeIdName(int,int);
    void clientConnect();
    void receiveData();

    void showServerClient(QTreeWidgetItem*);

private:
    Ui::ChatServer *ui;

    QLabel *infoLabel;
    QTcpServer *tcpServer;
    QList<QTcpSocket*> clientList;

    QString clientId;  // 서버에 아이디가 저장되어 있는지 확인
    QString clientName;

    QHash<int, QString> clientNameHash;
//    QHash<QString, int> clientIdHash;
    QHash<QTcpSocket*, int> clientIdHash;
    QHash<QString, QTcpSocket*> clientSocketHash;
};

#endif // CHATSERVER_H
