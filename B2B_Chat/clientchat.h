#ifndef CLIENTCHAT_H
#define CLIENTCHAT_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class ClientChat; }
QT_END_NAMESPACE

typedef enum {
    Server_In,
    Chat_In,
    Caht_Talk,
} Chat_Status;

typedef struct {
    Chat_Status type;
    char data[1020];
} chatProtocolType;

class ClientChat : public QWidget
{
    Q_OBJECT

public:
    ClientChat(QWidget *parent = nullptr);
    ~ClientChat();


private slots:
    void receiveData();
    void sendData();
//    void connectToServer();

    void on_sendButton_clicked();

    void on_lineEdit_returnPressed();

    void on_connectPushButton_clicked();

private:
    Ui::ClientChat *ui;

    QLineEdit *id;
    QLineEdit *name;
    QTextEdit *message;
    QLineEdit *inputLine;
    QTcpSocket *clientSocket;
    QList<QTcpSocket*> clientList;
};
#endif // CLIENTCHAT_H
