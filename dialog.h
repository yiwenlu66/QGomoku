#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
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

private:
    Ui::Dialog *ui;

    GomokuBoardWidget *m_board;

    QTcpSocket *m_socket;

    GomokuBoardWidget::Color m_role;
    bool m_isMyTurn;
    void switchTurn();
    void setTurnText();
};

#endif // DIALOG_H
