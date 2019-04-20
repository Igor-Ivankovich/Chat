#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QTcpSocket>
#include <QTcpServer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QTcpServer * server;
    QTcpSocket * socket;


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_listenButton_clicked();
    void incConnect();
    void readData();
    void on_sendButton_clicked();
    void connected();
    void on_connectButton_clicked();

    void on_sendFielButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
