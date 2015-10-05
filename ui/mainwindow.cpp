#include <QApplication>
#include <QtWidgets>
#include "registersview.h"
#include "dasmviewex.h"
#include "tlbviewex.h"
#include "mainwindow.h"
#include "vramview.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_cpu.reset();
    m_drc_cpu = new mips::mips3_x64(&m_cpu);

    m_regView = new RegistersView(this);
    m_regCop0View = new RegistersView(this);
    m_regCop1View = new RegistersView(this);

    m_scrollReg = new QScrollArea();
    m_scrollReg->setWidget(m_regView);
    m_dockRegView = new QDockWidget("MIPS regs");
    m_dockRegView->setWidget(m_scrollReg);
    addDockWidget(Qt::LeftDockWidgetArea, m_dockRegView);

    m_scrollRegCop0 = new QScrollArea();
    m_scrollRegCop0->setWidget(m_regCop0View);
    m_dockRegCop0View = new QDockWidget("COP0 regs");
    m_dockRegCop0View->setWidget(m_scrollRegCop0);
    addDockWidget(Qt::RightDockWidgetArea, m_dockRegCop0View);


    m_tlbView = new TlbViewEx();
    m_tlbView->setCpu(&m_cpu);
    m_dockTlbView = new QDockWidget("TLB");
    m_dockTlbView->setWidget(m_tlbView);
    addDockWidget(Qt::BottomDockWidgetArea, m_dockTlbView);

    m_scrollRegCop1 = new QScrollArea();
    m_scrollRegCop1->setWidget(m_regCop1View);
    m_dockRegCop1View = new QDockWidget("COP1 regs");
    m_dockRegCop1View->setWidget(m_scrollRegCop1);
    addDockWidget(Qt::BottomDockWidgetArea, m_dockRegCop1View);


    m_vramView = new VramView();
    m_dockVram = new QDockWidget("VRAM");
    m_dockVram->setWidget(m_vramView);
    addDockWidget(Qt::BottomDockWidgetArea, m_dockVram);

    m_dasmView = new DasmViewEx();
    m_dasmView->setCpu(&m_cpu);
    m_dasmView->gotoAddress(m_cpu.m_state.pc);

    setupRegisterViews();

    setCentralWidget(m_dasmView);

    m_toolbar = new QToolBar("Execuçao", this);
    m_step = new QAction("Step", this);
    m_step->setIcon(QIcon(":/next-icon.png"));
    m_runToCursor = new QAction("Run to Cursor", this);
    m_runToCursor->setIcon(QIcon(":/run-to-cursor.png"));
    m_reset = new QAction("Reset", this);
    m_reset->setIcon(QIcon(":/reset.png"));
    m_gotoPC = new QAction("Goto PC", this);
    m_run = new QAction("Run", this);
    m_toogleRecompiler = new QAction("Use Recompiler", this);
    m_toogleRecompiler->setCheckable(true);
    m_toolbar->addAction(m_step);
    m_toolbar->addAction(m_runToCursor);
    m_toolbar->addAction(m_reset);
    m_toolbar->addAction(m_gotoPC);
    m_toolbar->addAction(m_run);
    m_toolbar->addAction(m_toogleRecompiler);

    addToolBar(Qt::TopToolBarArea, m_toolbar);

    connect(m_step, SIGNAL(triggered()), this, SLOT(step()));
    connect(m_runToCursor, SIGNAL(triggered()), this, SLOT(runToCursor()));
    connect(m_reset, SIGNAL(triggered()), this, SLOT(reset()));
    connect(m_gotoPC, SIGNAL(triggered()), m_dasmView, SLOT(gotoCurrentAddress()));
    connect(m_run, SIGNAL(triggered()), this, SLOT(toogleRun()));

    m_timer.setInterval(0);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(step()));

    m_vramTimer.setInterval(100);
    connect(&m_vramTimer, SIGNAL(timeout()), m_vramView, SLOT(update()));

    m_isRunningToCursor = false;
    m_isRunning = false;
}

MainWindow::~MainWindow()
{
}

void MainWindow::step()
{
    if (m_isRunning) {

        if (m_toogleRecompiler->isChecked()) {
            m_cpu.m_state.cpr[0][13] &= ~0x400;
            m_drc_cpu->run(1000000/4);
            m_cpu.m_state.cpr[0][13] |= 0x400;
            m_drc_cpu->run(1000000/4);
        } else {
            m_cpu.m_state.cpr[0][13] &= ~0x400;
            if (m_cpu.run(1000000/4, false))
                toogleRun();
            else {
                // IRQ0
                m_cpu.m_state.cpr[0][13] |= 0x400;
                if (m_cpu.run(1000000/4, false))
                    toogleRun();
            }
        }

        m_vramView->update();
    } else {
        auto pc = m_cpu.m_state.pc;

        if (m_isRunningToCursor) {
            m_cpu.run(1, false);
            if (m_cpu.m_state.pc == m_stopPC) {
                runToCursor();
            }
        } else {

            // o step pode ignorar os breakpoints
            m_cpu.run(1, true);
            updateRegisterViews();
            if (m_cpu.m_state.pc != pc)
                m_dasmView->gotoAddress(m_cpu.m_state.pc);
        }
    }
}

void MainWindow::runToCursor()
{
    if (m_timer.isActive()) {
        m_timer.stop();
        m_runToCursor->setIcon(QIcon(":/run-to-cursor.png"));
        m_isRunningToCursor = false;
        m_reset->setEnabled(true);
        updateRegisterViews();
        m_vramTimer.stop();
        m_dasmView->gotoAddress(m_cpu.m_state.pc);
    } else {
        m_stopPC = m_dasmView->selectedAddress();
        m_isRunningToCursor = true;
        m_runToCursor->setIcon(QIcon(":/cancel.png"));
        m_reset->setEnabled(false);
        m_timer.start();
        m_vramTimer.start();
    }
}

void MainWindow::reset()
{
    m_cpu.reset();
    m_dasmView->gotoAddress(m_cpu.m_state.pc);
    updateRegisterViews();
}

void MainWindow::toogleRun()
{
    if (m_isRunning) {
        m_timer.stop();
        m_step->setEnabled(true);
        m_reset->setEnabled(true);
        m_runToCursor->setEnabled(true);
        m_run->setText("Run");
        m_isRunning = false;
        m_dasmView->gotoAddress(m_cpu.m_state.pc);
        updateRegisterViews();
    } else {
        m_vramTimer.stop();
        m_step->setEnabled(false);
        m_reset->setEnabled(false);
        m_runToCursor->setEnabled(false);
        m_run->setText("Stop");
        m_isRunning = true;
        m_vramView->setFocus();
        m_timer.start();
    }
}

void MainWindow::setupRegisterViews()
{
    m_regView->append(RegistersView::RegisterDesc("pc", &m_cpu.m_state.pc));
    m_regView->append(RegistersView::RegisterDesc("lo", &m_cpu.m_state.lo));
    m_regView->append(RegistersView::RegisterDesc("hi", &m_cpu.m_state.hi));
    m_regView->append(RegistersView::RegisterDesc("n_pc", &m_cpu.m_next_pc));

    for (int i = 0; i < 32; i++) {
        m_regView->append(RegistersView::RegisterDesc(m_cpu.reg_names[i],
                                                      &m_cpu.m_state.r[i]));
    }

    for (int i = 0; i < 32; i++) {
        if (QString(m_cpu.cop0_reg_names[i]) == QString("--"))
            continue;
        m_regCop0View->append(RegistersView::RegisterDesc(m_cpu.cop0_reg_names[i],
                                                          &m_cpu.m_state.cpr[0][i]));
    }

    for (int i = 0; i < 32; i++) {
        m_regCop1View->append(RegistersView::RegisterDesc(QString("f%1").arg(i),
                                                          &m_cpu.m_state.cpr[1][i]));
    }

    for (int i = 0; i < 32; i++) {
        m_regCop1View->append(RegistersView::RegisterDesc(QString("fcr%1").arg(i),
                                                          &m_cpu.m_state.fcr[i]));
    }
}

void MainWindow::updateRegisterViews()
{
    m_regView->update();
    m_regCop0View->update();
    m_regCop1View->update();
    m_tlbView->viewport()->update();
}


int ui_main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MainWindow mw;
    mw.show();

    return app.exec();
}
