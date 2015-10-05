#include <QtWidgets>
#include "dasmviewex.h"
#include "../mips3.h"
#include "../memory.h"

DasmViewEx::DasmViewEx(QWidget *parent) :
    QAbstractScrollArea(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    verticalScrollBar()->setMaximum(0x100000000ULL >> 2);

    m_font = QFont("Monaco", 10);
    setFont(m_font);

    QFontMetrics metrics(m_font);

    m_lineHeight = metrics.height();
    m_descent = metrics.descent();

    m_viewFirstAddr = 0;
    m_currentAddr = 0;
    m_selectedAddr = 0;

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(verticalScroll(int)));

    viewport()->setFixedWidth(metrics.width('X') * 60);
}

DasmViewEx::~DasmViewEx()
{

}

void DasmViewEx::paintEvent(QPaintEvent *event)
{
    QPainter p(viewport());

    p.fillRect(event->rect(), QColor(255, 255, 220));
    int linesPerView = viewport()->rect().height() / m_lineHeight + 2;


    for (int i = 0; i < linesPerView; i++) {
        mips::addr_t adr = m_viewFirstAddr + i * 4;
        mips::addr_t eadr;
        QString instr;
        if (m_cpu->translate(adr, &eadr)) {
            instr = "unmapped";
        } else {
            uint32_t opcode = mips::mem::read_word(eadr);
            instr = m_cpu->dasm(opcode, adr).c_str();
        }


        if (adr >= 0xFFFFFFFF)
            break;
        if (adr == m_currentAddr)
            p.fillRect(0, i * m_lineHeight, viewport()->width(), m_lineHeight, QColor(120, 200, 255));

        if (adr == m_selectedAddr)
            p.fillRect(0, i * m_lineHeight, viewport()->width(), m_lineHeight, QColor(255, 200, 200, 200));

        p.drawText(0, (i + 1) * m_lineHeight - m_descent, instr);
    }
}

void DasmViewEx::mousePressEvent(QMouseEvent *event)
{
    int line = event->y() / m_lineHeight;
    m_selectedAddr = m_viewFirstAddr + line * 4;
    viewport()->update();
}

mips::mips3 *DasmViewEx::cpu() const
{
    return m_cpu;
}

void DasmViewEx::setCpu(mips::mips3 *cpu)
{
    m_cpu = cpu;
}

void DasmViewEx::gotoAddress(mips::addr_t addr)
{
    int linesPerView = viewport()->rect().height() / m_lineHeight + 2;

    mips::addr_t lastAddr = (linesPerView - 4) * 4 + m_viewFirstAddr;

    m_currentAddr = addr;
    // Verifica se está no campo de visão
    if (addr >= m_viewFirstAddr && addr <= lastAddr) {
        viewport()->update();
    } else {
        int displace = linesPerView / 2 - 4;
        int idx = (addr - (displace * 4)) / 4;
        verticalScrollBar()->setValue(idx);
    }
}

mips::addr_t DasmViewEx::selectedAddress()
{
    return m_selectedAddr;
}

void DasmViewEx::verticalScroll(int value)
{
    m_viewFirstAddr = ((mips::addr_t) value) * 4;
    update();
}

void DasmViewEx::gotoCurrentAddress()
{
    m_currentAddr = m_cpu->m_state.pc;
    gotoAddress(m_currentAddr);
}


