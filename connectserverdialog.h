#ifndef CONNECTSERVERDIALOG_H
#define CONNECTSERVERDIALOG_H

#include <QDialog>
#include <QtNetwork/QTcpSocket>

#include "dialpad.h"

namespace Ui {
class ConnectServerDialog;
}

class ConnectServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectServerDialog(QWidget *parent = 0);
    ~ConnectServerDialog();

signals:
    void connected(QTcpSocket *);

public slots:
    void connectServer();
    void cancel();
    void connectionEstablished();

private:
    Ui::ConnectServerDialog *ui;
    DialPad *m_dialpad;

    QTcpSocket *m_socket = nullptr;
};

#endif // CONNECTSERVERDIALOG_H
