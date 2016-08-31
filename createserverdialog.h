#ifndef CREATESERVERDIALOG_H
#define CREATESERVERDIALOG_H

#include <QDialog>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

namespace Ui {
class CreateServerDialog;
}

class CreateServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateServerDialog(QWidget *parent = 0);
    ~CreateServerDialog();

signals:
    void connected(QTcpSocket *);

public slots:
    void listen();
    void cancel();
    void acceptConnection();

private:
    Ui::CreateServerDialog *ui;
    QTcpServer *m_server = nullptr;
};

#endif // CREATESERVERDIALOG_H
