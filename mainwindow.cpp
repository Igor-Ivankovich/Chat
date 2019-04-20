#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(ui->messageLine, SIGNAL(returnPressed()), this,
          SLOT(on_sendButton_clicked()));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_listenButton_clicked() {
  server = new QTcpServer(this);
  server->listen(QHostAddress::Any, 2121);
  connect(server, SIGNAL(newConnection()), this, SLOT(incConnect()));
}

void MainWindow::incConnect() {
  socket = server->nextPendingConnection();
  connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
  connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));  // read Date
}

// get the file
void MainWindow::readData() {
  quint64 fileOrText;
  quint64 fileSize;
  QString fileName;
  QByteArray arrayByte;
  QDataStream addInf(&arrayByte, QIODevice::ReadWrite);
  arrayByte = socket->readAll();
  addInf >> fileOrText;
  qDebug() << fileOrText;
  if (fileOrText != 1) {
    ui->textEdit->append(QString(arrayByte));
    return;
  }
  addInf >> fileSize;
  addInf >> fileName;
  QFile file("C:\\Users\\JP\\Desktop\\test\\" + fileName);
  file.open(QFile::WriteOnly);
  QDataStream write(&file);
  long int bytesDone = 0;
  long int sizeDone = fileSize;
  long int errorSocket;
  while (bytesDone < sizeDone) {
    errorSocket = 0;
    while (errorSocket == 0) {
      errorSocket = socket->waitForReadyRead(-1);
    }
    if (errorSocket == -1) {
      socket->close();
      return;
    }
    QByteArray tmp = socket->readAll();
    errorSocket += write.writeRawData(tmp.data(), tmp.size());
    bytesDone += tmp.size();
  }
  file.close();
}

void MainWindow::on_sendButton_clicked() {
  socket->write(ui->messageLine->text().toLocal8Bit());
  ui->messageLine->clear();
}

void MainWindow::connected() { ui->textEdit->setText("Success connected"); }

void MainWindow::on_connectButton_clicked() {
  socket = new QTcpSocket(this);
  socket->connectToHost(
      (ui->hostLine->text().isEmpty() ? "localhost" : ui->hostLine->text()),
      2121);
  socket->readyRead();
  connect(socket, SIGNAL(connected()), this, SLOT(connected()));
  connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));  // read Date
  connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

// load the file
void MainWindow::on_sendFielButton_clicked() {
  quint64 fileOrText = 1;
  QString path = QFileDialog::getOpenFileName();
  if (path.isEmpty()) return;
  QFile file(path);
  QDataStream readFile(&file);
  if (!file.open(QFile::ReadOnly)) return;
  QByteArray arrayByte;
  QDataStream addInf(&arrayByte, QIODevice::WriteOnly);
  addInf << fileOrText;
  addInf << quint64(file.size());
  addInf << path.section("/", -1, -1);
  socket->write(arrayByte);
  socket->waitForBytesWritten();
  int errorSocket = 0;
  char chareBlock[131072];
  chareBlock[131071] = '\0';
  while (!readFile.atEnd()) {
    int length = readFile.readRawData(chareBlock, 131071);
    QByteArray blockFile(chareBlock, length);
    errorSocket = socket->write(blockFile, length);
    socket->waitForBytesWritten();
    if (errorSocket == -1) {
      socket->close();
      return;
    }
  }
}
