#include "dialog.h"
#include "ui_dialog.h"

#include <QSizePolicy>

#include "gomokuboardwidget.h"
#include "createserverdialog.h"
#include "connectserverdialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    m_board(new GomokuBoardWidget(this))
{
    ui->setupUi(this);
    ui->horizontalLayout->insertWidget(0, m_board, 2);

    connect(ui->pushButton_create, SIGNAL(clicked(bool)), this, SLOT(showCreateServerDialog()));
    connect(ui->pushButton_connect, SIGNAL(clicked(bool)), this, SLOT(showConnectServerDialog()));
}

void Dialog::showCreateServerDialog()
{
    m_role = GomokuBoardWidget::BLACK;
    CreateServerDialog dlg(this);
    connect(&dlg, SIGNAL(connected(QTcpSocket*)), this, SLOT(connected(QTcpSocket*)));
    dlg.exec();
}

void Dialog::showConnectServerDialog()
{
    m_role = GomokuBoardWidget::WHITE;
    ConnectServerDialog dlg(this);
    connect(&dlg, SIGNAL(connected(QTcpSocket*)), this, SLOT(connected(QTcpSocket*)));
    dlg.exec();
}

void Dialog::connected(QTcpSocket *socket)
{
    socket->setParent(this);
    m_socket = socket;
    ui->pushButton_connect->setEnabled(false);
    ui->pushButton_create->setEnabled(false);
    ui->label_role->setText(tr("You take <b>%1</b>.")
                            .arg((m_role == GomokuBoardWidget::BLACK) ? tr("BLACK") : tr("WHITE")));
    m_isMyTurn = (m_role == GomokuBoardWidget::BLACK);
    setTurnText();
}

void Dialog::switchTurn()
{
    m_isMyTurn = !m_isMyTurn;
    setTurnText();
}

void Dialog::setTurnText()
{
    ui->label_turn->setText(tr("It's <b>%1</b> turn.")
                            .arg(m_isMyTurn ? tr("YOUR") : tr("OPPONENT's")));
}

Dialog::~Dialog()
{
    delete ui;
}
