#include <QtWidgets>
#include "tlbviewex.h"
#include "../mips3.h"

TlbViewEx::TlbViewEx(QWidget *parent) :
    QAbstractScrollArea(parent)
{
    m_font = QFont("Monaco", 10);
    setFont(m_font);

    QFontMetrics metrics(m_font);

    m_lineHeight = metrics.height();
    m_descent = metrics.descent();

    m_viewFirstEntry = 0;
    m_currentEntry = 0;

    m_cpu = nullptr;
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(verticalScroll(int)));
}

TlbViewEx::~TlbViewEx()
{

}

void TlbViewEx::paintEvent(QPaintEvent *event)
{
    QPainter p(viewport());

    p.fillRect(event->rect(), QColor(255, 255, 220));
    if (!m_cpu)
        return;

    int linesPerView = viewport()->rect().height() / m_lineHeight + 2;


    for (int i = 0; i < linesPerView; i++) {
        int entry = m_viewFirstEntry + i;
        if (entry >= m_cpu->m_tlb_entries)
            break;
        const mips::mips3::tlb_entry *t = &m_cpu->m_tlb[entry];

        QString instr = QString("%1 - 0x%2 : 0x%3")
                .arg(QString::number(entry, 16), 2, '0')
                .arg(QString::number(t->v[1], 16), 16, '0')
                .arg(QString::number(t->v[0], 16), 16, '0');

        if (entry == m_currentEntry)
            p.fillRect(0, i * m_lineHeight, viewport()->width(), m_lineHeight, QColor(180, 255, 180));
        p.drawText(0, (i + 1) * m_lineHeight - m_descent, instr);
    }
}

void TlbViewEx::mousePressEvent(QMouseEvent *event)
{
    int line = event->y() / m_lineHeight;
    m_currentEntry = m_viewFirstEntry + line;
    viewport()->update();
}

mips::mips3 *TlbViewEx::cpu() const
{
    return m_cpu;
}

void TlbViewEx::setCpu(mips::mips3 *cpu)
{
    m_cpu = cpu;
    verticalScrollBar()->setRange(0, m_cpu->m_tlb_entries);
    update();
}

void TlbViewEx::verticalScroll(int units)
{
    m_viewFirstEntry = units;
    update();
}


