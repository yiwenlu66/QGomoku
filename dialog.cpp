#include "dialog.h"
#include "ui_dialog.h"

#include <QSizePolicy>
#include <QEvent>
#include <QMouseEvent>
#include <QMessageBox>

#include "gomokuboardwidget.h"
#include "createserverdialog.h"
#include "connectserverdialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    m_board(new GomokuBoardWidget(this))
{
    ui->setupUi(this);

    connect(m_board, SIGNAL(win(GomokuBoardWidget::Color)), this, SLOT(win(GomokuBoardWidget::Color)));
    m_board->installEventFilter(this);
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

bool Dialog::eventFilter(QObject *, QEvent *event)
{
    if (!m_isMyTurn) {
        return false;
    }
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() != Qt::LeftButton) {
            return false;
        }
        if (m_board->putPieceAtPos(m_role, mouseEvent->pos())) {
            // TODO: switch turn and send data
            return true;
        }
    }
    return false;
}

void Dialog::win(GomokuBoardWidget::Color color)
{
    if (color == m_role) {
        QMessageBox::information(this, tr("Congratulations!"), tr("You won!"), QMessageBox::Ok);
    } else {
        QMessageBox::information(this, tr("Sorry!"), tr("You lost!"), QMessageBox::Ok);
    }
    m_board->clear();
}

Dialog::~Dialog()
{
    delete ui;
}
