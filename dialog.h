#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

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

private:
    Ui::Dialog *ui;

    GomokuBoardWidget *m_board;
};

#endif // DIALOG_H
