#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QWidget>
#include <QLabel>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class ChatServer;
}

class ChatServer : public QWidget
{
    Q_OBJECT

public:
    explicit ChatServer(QWidget *parent = nullptr);
    ~ChatServer();

    void clientConnect();
    void echoData();

private:
    Ui::ChatServer *ui;

    QLabel *infoLabel;
    QTcpServer *tcpServer;
    QList<QTcpSocket*> clientList;
};

#endif // CHATSERVER_H
