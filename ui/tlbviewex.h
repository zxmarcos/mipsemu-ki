#ifndef TLBVIEWEX_H
#define TLBVIEWEX_H

#include <QObject>
#include <QWidget>
#include <QAbstractScrollArea>
#include <QFont>
#include "../mips3.h"

class TlbViewEx : public QAbstractScrollArea
{
    Q_OBJECT

public:
    TlbViewEx(QWidget *parent=0);
    ~TlbViewEx();

    mips::mips3 *cpu() const;
    void setCpu(mips::mips3 *cpu);

public slots:
    void verticalScroll(int units);
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
private:
    int m_viewFirstEntry;
    int m_currentEntry;
    mips::mips3 *m_cpu;
    QFont m_font;
    int m_lineHeight;
    int m_descent;
    int m_charWidth;
};

#endif // TLBVIEWEX_H
