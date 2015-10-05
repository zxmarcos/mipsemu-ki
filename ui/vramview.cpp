#include <QtWidgets>
#include "vramview.h"
#include "../memory.h"

extern uint8_t *g_rambase;

struct KeyMapping {
    int qtk;
    int input;
    int mask;
    int toogle;
} g_input_mapping[] = {
    // P1
    { Qt::Key_Up,       0, 0x00040, 0 },
    { Qt::Key_Down,     0, 0x00080, 0 },
    { Qt::Key_Left,     0, 0x00100, 0 },
    { Qt::Key_Right,    0, 0x00200, 0 },
    { Qt::Key_A,        0, 0x00001, 0 },
    { Qt::Key_S,        0, 0x00002, 0 },
    { Qt::Key_D,        0, 0x00004, 0 },
    { Qt::Key_Z,        0, 0x00008, 0 },
    { Qt::Key_X,        0, 0x00010, 0 },
    { Qt::Key_C,        0, 0x00020, 0 },
    { Qt::Key_1,        0, 0x00400, 0 },
    { Qt::Key_2,        0, 0x00800, 0 },
    { Qt::Key_F1,       0, 0x01000, 0 },
    // P2,
    { Qt::Key_Home,     1, 0x00040, 0 },
    { Qt::Key_End,      1, 0x00080, 0 },
    { Qt::Key_Delete,   1, 0x00100, 0 },
    { Qt::Key_PageDown, 1, 0x00200, 0 },
    { Qt::Key_G,        1, 0x00001, 0 },
    { Qt::Key_H,        1, 0x00002, 0 },
    { Qt::Key_J,        1, 0x00004, 0 },
    { Qt::Key_B,        1, 0x00008, 0 },
    { Qt::Key_N,        1, 0x00010, 0 },
    { Qt::Key_M,        1, 0x00020, 0 },
    { Qt::Key_3,        1, 0x00400, 0 },
    { Qt::Key_4,        1, 0x00800, 0 },
    { Qt::Key_F2,       1, 0x01000, 0 },
    // ETC
    { Qt::Key_F12,      2, 0x08000, 1 },
};

const int g_input_size = sizeof(g_input_mapping) / sizeof(KeyMapping);

VramView::VramView(QWidget *parent) : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(320, 240);
}

VramView::~VramView()
{

}

void VramView::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    if (g_rambase == nullptr)
        return;

    QImage img(320, 240, QImage::Format_RGB555);
    uint16_t *gp = (uint16_t*)&g_rambase[mips::mem::g_vram_base_addr];
    for (int y = 0; y < 240; y++) {
        uint16_t *dptr = (uint16_t*)img.scanLine(y);
        for (int x = 0; x < 320; x++) {
            uint16_t v = *gp;
            *dptr = (v & 0x3E0) | ((v >> 10) & 0x1F) | ((v & 0x1F) << 10);
            gp++;
            dptr++;
        }
    }

    p.drawImage(rect(), img);
}

void VramView::keyPressEvent(QKeyEvent *event)
{
    for (int i = 0; i < g_input_size; i++) {
        if (g_input_mapping[i].qtk == event->key()) {
            int off = g_input_mapping[i].input;
            if (g_input_mapping[i].toogle)
                mips::mem::g_input_dips[off] ^= g_input_mapping[i].mask;
            else
                mips::mem::g_input_dips[off] &= ~g_input_mapping[i].mask;
        }
    }
}

void VramView::keyReleaseEvent(QKeyEvent *event)
{
    for (int i = 0; i < g_input_size; i++) {
        if (g_input_mapping[i].qtk == event->key()) {
            int off = g_input_mapping[i].input;
            if (!g_input_mapping[i].toogle)
                mips::mem::g_input_dips[off] |= g_input_mapping[i].mask;
        }
    }
}

