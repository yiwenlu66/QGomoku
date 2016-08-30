#ifndef DIALPAD_H
#define DIALPAD_H

#include <QWidget>
#include <QString>
#include <QSignalMapper>

namespace Ui {
class DialPad;
}

class DialPad : public QWidget
{
    Q_OBJECT

public:
    explicit DialPad(QWidget *parent = 0);
    void setText(const QString &);
    QString text();
    ~DialPad();

signals:
    void textChanged(QString);

public slots:
    void clear();
    void append(const QString &);

private:
    Ui::DialPad *ui;
    QSignalMapper *m_signalMapper;
};

#endif // DIALPAD_H
