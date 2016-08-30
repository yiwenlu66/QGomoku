#include "dialpad.h"
#include "ui_dialpad.h"

DialPad::DialPad(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialPad),
    m_signalMapper(new QSignalMapper(this))
{
    ui->setupUi(this);

    connect(ui->pushButton_0, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
    connect(ui->pushButton_1, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
    connect(ui->pushButton_2, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
    connect(ui->pushButton_3, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
    connect(ui->pushButton_4, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
    connect(ui->pushButton_5, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
    connect(ui->pushButton_6, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
    connect(ui->pushButton_7, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
    connect(ui->pushButton_8, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
    connect(ui->pushButton_9, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
    connect(ui->pushButton_point, SIGNAL(clicked(bool)), m_signalMapper, SLOT(map()));
    m_signalMapper->setMapping(ui->pushButton_0, "0");
    m_signalMapper->setMapping(ui->pushButton_1, "1");
    m_signalMapper->setMapping(ui->pushButton_2, "2");
    m_signalMapper->setMapping(ui->pushButton_3, "3");
    m_signalMapper->setMapping(ui->pushButton_4, "4");
    m_signalMapper->setMapping(ui->pushButton_5, "5");
    m_signalMapper->setMapping(ui->pushButton_6, "6");
    m_signalMapper->setMapping(ui->pushButton_7, "7");
    m_signalMapper->setMapping(ui->pushButton_8, "8");
    m_signalMapper->setMapping(ui->pushButton_9, "9");
    m_signalMapper->setMapping(ui->pushButton_point, ".");
    connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(append(QString)));

    connect(ui->pushButton_clear, SIGNAL(clicked(bool)), this, SLOT(clear()));
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));
}

void DialPad::setText(const QString &text)
{
    ui->lineEdit->setText(text);
}

QString DialPad::text()
{
    return ui->lineEdit->text();
}

void DialPad::clear()
{
    ui->lineEdit->clear();
}

void DialPad::append(const QString &s)
{
    ui->lineEdit->setText(ui->lineEdit->text() + s);
}

DialPad::~DialPad()
{
    delete ui;
}
