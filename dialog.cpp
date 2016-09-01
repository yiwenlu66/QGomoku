#include "dialog.h"
#include "ui_dialog.h"

#include <cstring>

#include <QSizePolicy>
#include <QEvent>
#include <QMouseEvent>
#include <QMessageBox>

#include "gomokuboardwidget.h"
#include "createserverdialog.h"
#include "connectserverdialog.h"

constexpr char const Dialog::TAG_WIN[];
constexpr char const Dialog::TAG_BOARD[];

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
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readData()));
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
            switchTurn();

            QByteArray data;
            QDataStream out(&data, QIODevice::WriteOnly);
            out << TAG_BOARD << *m_board;
            writeData(data);

            if (m_winner != -1) {
                QByteArray data;
                QDataStream out(&data, QIODevice::WriteOnly);
                GomokuBoardWidget::Color color = static_cast<GomokuBoardWidget::Color>(m_winner);
                out << TAG_WIN << static_cast<int>(color);
                writeData(data);
                showWinMessage(color);
            }

            return true;
        }
    }
    return false;
}

void Dialog::win(GomokuBoardWidget::Color color)
{
    m_winner = static_cast<int>(color);
}

void Dialog::showWinMessage(GomokuBoardWidget::Color color)
{
    m_clearFlag = true;
    if (color == m_role) {
        QMessageBox::information(this, tr("Congratulations!"), tr("You won!"), QMessageBox::Ok);
    } else {
        QMessageBox::information(this, tr("Sorry!"), tr("You lost!"), QMessageBox::Ok);
    }
    m_winner = -1;
    if (m_clearFlag) {
        m_clearFlag = false;
        m_board->clear();
    }
}

void Dialog::writeData(const QByteArray &data)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write(encodeInt(data.length()));
        m_socket->write(data);
    }
}

void Dialog::readData()
{
    while (true) {
        if (m_pendingSize == 0) {
            if (m_socket->bytesAvailable() < 4) {
                // incomplete header
                return;
            }
            QByteArray sizeArray = m_socket->read(4);
            m_pendingSize = decodeInt(sizeArray);
        }

        if (m_socket->bytesAvailable() < m_pendingSize) {
            return;
        }

        QByteArray buffer = m_socket->read(m_pendingSize);
        m_pendingSize = 0;
        QDataStream in(&buffer, QIODevice::ReadOnly);
        char *tag;
        in >> tag;
        if (!std::strcmp(tag, TAG_WIN)) {
            int winner;
            in >> winner;
            showWinMessage(static_cast<GomokuBoardWidget::Color>(winner));
        } else if (!std::strcmp(tag, TAG_BOARD)) {
            in >> *m_board;
            m_board->update();
            m_clearFlag = false;
            switchTurn();
        }
        delete[] tag;
    }
}

QByteArray Dialog::encodeInt(const qint32 &src)
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << src;
    return array;
}

qint32 Dialog::decodeInt(const QByteArray &src)
{
    qint32 result;
    QDataStream in(src);
    in >> result;
    return result;
}

Dialog::~Dialog()
{
    delete ui;
}
