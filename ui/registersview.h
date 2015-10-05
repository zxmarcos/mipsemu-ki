#ifndef REGISTERSVIEW_H
#define REGISTERSVIEW_H

#include <QWidget>
#include <QVector>
#include "hexspinbox.h"
#include "../mips3.h"

class RegistersView : public QWidget
{
    Q_OBJECT
public:
    explicit RegistersView(QWidget *parent = 0);
    ~RegistersView();

    struct RegisterDesc {
        QString name;
        mips::addr_t *ptr;
        RegisterDesc(QString name, mips::addr_t *ptr) :
            name(name), ptr(ptr) {
        }
        RegisterDesc() : ptr(nullptr) {
        }
    };

    void append(const RegisterDesc& desc);
    void clear();
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private:
    enum {
        HEX_MODE = 0,
        FLOAT_MODE,
        DOUBLE_MODE,
        MAX_MODE,
    };
    int m_viewMode;
    HexSpinDialog *m_editor;
    QVector<RegisterDesc> m_registers;
    QFont m_font;
    int m_selectedLine;
    int m_charWidth;
    int m_lineHeight;
    int m_descent;
    int m_columns;
};

#endif // REGISTERSVIEW_H
