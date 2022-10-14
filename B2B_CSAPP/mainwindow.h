#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ClientManager;
class ItemManager;
class OrderManager;
class ChatServer;
class ChatWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    ClientManager *clientManager;
    ItemManager *itemManager;
    OrderManager *orderManager;
    ChatServer *chatManager;
    ChatWindow *chatWindow;
};
#endif // MAINWINDOW_H
