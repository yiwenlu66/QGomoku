#include "createserverdialog.h"
#include "ui_createserverdialog.h"

#include <QtDebug>

#include <QNetworkInterface>
#include <QtNetwork/QHostAddress>
#include <QMessageBox>

CreateServerDialog::CreateServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateServerDialog)
{
    ui->setupUi(this);

    ui->comboBox->addItem("Any");
    for (auto addr : QNetworkInterface::allAddresses()) {
        ui->comboBox->addItem(addr.toString());
    }

    connect(ui->pushButton_create, SIGNAL(clicked(bool)), this, SLOT(listen()));
    connect(ui->pushButton_cancel, SIGNAL(clicked(bool)), this, SLOT(cancel()));
}

void CreateServerDialog::listen()
{
    ui->pushButton_create->setEnabled(false);
    m_server = new QTcpServer(this);
    if (!m_server->listen(
                ui->comboBox->currentIndex() ?
                    QHostAddress(ui->comboBox->itemData(ui->comboBox->currentIndex()).toString()) : QHostAddress::Any,
                ui->spinBox->value())
            ) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to create server!"), QMessageBox::Ok);
        ui->pushButton_create->setEnabled(true);
    } else {
        connect(m_server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    }
}

void CreateServerDialog::acceptConnection()
{
    emit connected(m_server->nextPendingConnection());
    close();
}

void CreateServerDialog::cancel()
{
    if (m_server != nullptr && m_server->isListening()) {
        m_server->close();
    }
    close();
}

CreateServerDialog::~CreateServerDialog()
{
    delete ui;
}
