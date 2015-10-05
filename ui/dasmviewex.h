#ifndef DASMVIEWEX_H
#define DASMVIEWEX_H

#include <QFont>
#include <QAbstractScrollArea>
#include "../mips3.h"

class DasmViewEx : public QAbstractScrollArea
{
    Q_OBJECT

public:
    DasmViewEx(QWidget *parent=0);
    ~DasmViewEx();
    mips::mips3 *cpu() const;
    void setCpu(mips::mips3 *cpu);
    void gotoAddress(mips::addr_t addr);
    mips::addr_t selectedAddress();
public slots:
    void verticalScroll(int units);
    void gotoCurrentAddress();
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
private:
    mips::addr_t m_viewFirstAddr;
    mips::addr_t m_currentAddr;
    mips::addr_t m_selectedAddr;
    mips::mips3 *m_cpu;
    QFont m_font;
    int m_lineHeight;
    int m_descent;
};

#endif // DASMVIEWEX_H
