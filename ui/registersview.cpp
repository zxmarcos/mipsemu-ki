#include <QtWidgets>
#include "registersview.h"

RegistersView::RegistersView(QWidget *parent) : QWidget(parent)
{
    m_font = QFont("Monaco", 10);
    QFontMetrics metrics(m_font);

    m_selectedLine = 0;
    m_lineHeight = metrics.height();
    m_charWidth = metrics.width('X');
    m_descent = metrics.descent();

    setAutoFillBackground(false);

    setFont(m_font);
    m_columns = 34;

    m_editor = new HexSpinDialog(this);
    m_viewMode = HEX_MODE;
}

RegistersView::~RegistersView()
{

}

void RegistersView::append(const RegistersView::RegisterDesc &desc)
{
    m_registers.push_back(desc);
    resize(m_charWidth * m_columns, m_lineHeight * m_registers.size());
    update();
}

void RegistersView::clear()
{
    m_registers.clear();
    resize(m_charWidth * m_columns, m_lineHeight * m_registers.size());
    update();
}



void RegistersView::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    unsigned toogle = 0;

    QColor alternate(255, 255, 220);
    QColor active(180, 255, 180);

    for (int i = 0; i < m_registers.size(); i++) {
        int ypos = (i + 1) * m_lineHeight - m_descent;

        QRect lineRect(0, i * m_lineHeight, width(), m_lineHeight);

        if (i == m_selectedLine) {
            p.fillRect(lineRect.adjusted(0, 0, -1, 0), active);
        } else {
            if (toogle)
                p.fillRect(lineRect, alternate);
            else
                p.fillRect(lineRect, Qt::white);
        }

        toogle ^= 1;

        RegisterDesc desc = m_registers[i];
        QString number;
        p.drawText(5, ypos, desc.name);
        switch (m_viewMode) {
        case HEX_MODE:
            number = QString::number(*desc.ptr, 16);
            p.drawText(5 + m_charWidth * 10, ypos, QString("0x%1").arg(number, 16, '0'));
            break;
        case FLOAT_MODE:
            number = QString::number(*reinterpret_cast<float*>(desc.ptr), 'f', 16);
            p.drawText(5 + m_charWidth * 10, ypos, QString("%1").arg(number, 16, '0'));
            break;
        case DOUBLE_MODE:
            number = QString::number(*reinterpret_cast<double*>(desc.ptr), 'f', 24);
            p.drawText(5 + m_charWidth * 10, ypos, number);//QString("%1").arg(number, 20, '0'));
            break;
        }


    }

}

void RegistersView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        m_viewMode++;
        if (m_viewMode >= MAX_MODE)
            m_viewMode = 0;

    } else {
        m_selectedLine = event->y() / m_lineHeight;
    }
    update();
}

void RegistersView::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_selectedLine = event->y() / m_lineHeight;
    if (m_selectedLine >= m_registers.size())
        return;

    m_editor->setValue(*m_registers[m_selectedLine].ptr);
    if (m_editor->exec() == QDialog::Accepted) {
        *m_registers[m_selectedLine].ptr = m_editor->value();
    }
}


