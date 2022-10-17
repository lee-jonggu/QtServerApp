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
    Caht_Talk,
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
    void clientConnect();
    void removeClient();
    void receiveData();

    void showServerClient(QTreeWidgetItem*);

signals:
    void clickedUpdate();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ChatServer *ui;

    QLabel *infoLabel;
    QTcpServer *tcpServer;
    QList<QTcpSocket*> clientList;
};

#endif // CHATSERVER_H
