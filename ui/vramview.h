#ifndef VRAMVIEW_H
#define VRAMVIEW_H

#include <QWidget>

class VramView : public QWidget
{
    Q_OBJECT
public:
    explicit VramView(QWidget *parent = 0);
    ~VramView();

signals:

public slots:
protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void paintEvent(QPaintEvent *event);
};

#endif // VRAMVIEW_H
