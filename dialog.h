#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtGlobal>
#include <QtNetwork/QTcpSocket>

#include "gomokuboardwidget.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

public slots:
    void showCreateServerDialog();
    void showConnectServerDialog();
    void connected(QTcpSocket *);
    void win(GomokuBoardWidget::Color);
    void readData();

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    static constexpr char const TAG_WIN[] = "win";
    static constexpr char const TAG_BOARD[] = "board";

    static QByteArray encodeInt(const qint32& src);
    static qint32 decodeInt(const QByteArray& src);
    qint32 m_pendingSize = 0;
    int m_winner = -1;

    Ui::Dialog *ui;

    GomokuBoardWidget *m_board;

    QTcpSocket *m_socket;
    void writeData(const QByteArray &data);

    GomokuBoardWidget::Color m_role;
    bool m_isMyTurn = false;
    void switchTurn();
    void setTurnText();
    bool m_clearFlag = false;

    void showWinMessage(GomokuBoardWidget::Color);
};

#endif // DIALOG_H
