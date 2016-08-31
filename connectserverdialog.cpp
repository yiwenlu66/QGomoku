#include "connectserverdialog.h"
#include "ui_connectserverdialog.h"

ConnectServerDialog::ConnectServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectServerDialog)
{
    ui->setupUi(this);

    m_dialpad = new DialPad(this);
    m_dialpad->setText("127.0.0.1");
    ui->verticalLayout->insertWidget(0, m_dialpad);

    connect(ui->pushButton_connect, SIGNAL(clicked(bool)), this, SLOT(connectServer()));
    connect(ui->pushButton_cancel, SIGNAL(clicked(bool)), this, SLOT(cancel()));
}

void ConnectServerDialog::connectServer()
{
    ui->pushButton_connect->setEnabled(false);
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(connected()), this, SLOT(connectionEstablished()));
    m_socket->connectToHost(m_dialpad->text(), ui->spinBox->value());
}

void ConnectServerDialog::cancel()
{
    if (m_socket != nullptr && m_socket->state() != QTcpSocket::UnconnectedState) {
        m_socket->abort();
    }
    close();
}

void ConnectServerDialog::connectionEstablished()
{
    emit connected(m_socket);
    close();
}

ConnectServerDialog::~ConnectServerDialog()
{
    delete ui;
}
