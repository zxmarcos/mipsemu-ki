#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QTimer>
#include "../mips3.h"
#include "../x64/mips3_x64.h"

class RegistersView;
class DasmViewEx;
class QDockWidget;
class QAction;
class QToolBar;
class TlbViewEx;
class VramView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void step();
    void runToCursor();
    void reset();
    void toogleRun();
private:
    void setupRegisterViews();
    void updateRegisterViews();
    mips::mips3 m_cpu;
    mips::mips3_x64 *m_drc_cpu;
    QToolBar *m_toolbar;
    RegistersView *m_regView;
    RegistersView *m_regCop0View;
    RegistersView *m_regCop1View;
    DasmViewEx *m_dasmView;
    TlbViewEx *m_tlbView;
    VramView *m_vramView;
    QScrollArea *m_scrollReg;
    QScrollArea *m_scrollRegCop0;
    QScrollArea *m_scrollRegCop1;
    QDockWidget *m_dockTlbView;
    QDockWidget *m_dockRegView;
    QDockWidget *m_dockVram;
    QDockWidget *m_dockRegCop0View;
    QDockWidget *m_dockRegCop1View;
    QAction *m_step;
    QAction *m_reset;
    QAction *m_runToCursor;
    QAction *m_gotoPC;
    QAction *m_run;
    QAction *m_toogleRecompiler;
    QTimer m_timer;
    QTimer m_vramTimer;
    bool m_isRunningToCursor;
    bool m_isRunning;
    mips::addr_t m_stopPC;
};

int ui_main(int argc, char **argv);

#endif // MAINWINDOW_H
