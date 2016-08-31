#include "dialog.h"
#include "ui_dialog.h"

#include <QSizePolicy>

#include "gomokuboardwidget.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    m_board(new GomokuBoardWidget(this))
{
    ui->setupUi(this);
    ui->horizontalLayout->insertWidget(0, m_board, 2);
}

Dialog::~Dialog()
{
    delete ui;
}
