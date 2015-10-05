TEMPLATE = app
CONFIG += c++11 exceptions
CONFIG -= app_bundle

QT += widgets

#DEFINES += RUN_KI2

SOURCES += main.cpp \
    mips3.cpp \
    memory.cpp \
    boot.cpp \
    dasm.cpp \
    ui/mainwindow.cpp \
    ui/registersview.cpp \
    ui/dasmviewex.cpp \
    cop0.cpp \
    ui/tlbviewex.cpp \
    ui/vramview.cpp \
    ui/hexspinbox.cpp \
    ide.cpp \
    cop1.cpp \
    x64/mips3_x64.cpp \
    x64/udis86/decode.c \
    x64/udis86/input.c \
    x64/udis86/itab.c \
    x64/udis86/syn.c \
    x64/udis86/syn-att.c \
    x64/udis86/syn-intel.c \
    x64/udis86/udis86.c


HEADERS += \
    mips3.h \
    boot.h \
    memory.h \
    common.h \
    mipsdef.h \
    ui/mainwindow.h \
    ui/registersview.h \
    ui/dasmviewex.h \
    ui/tlbviewex.h \
    ui/vramview.h \
    ui/hexspinbox.h \
    ide.h \
    mips3_rw.h \
    mips3_branch.h \
    mips3_bitops.h \
    mips3_arithm.h \
    mips3_shift.h \
    mips3_misc.h \
    x64/mips3_x64.h \
    x64/xbyak/xbyak.h \
    x64/xbyak/xbyak_bin2hex.h \
    x64/xbyak/xbyak_mnemonic.h \
    x64/xbyak/xbyak_util.h \
    x64/mips3_x64_defs.h \
    x64/mips3_x64_bitops.h \
    x64/mips3_x64_arithm.h \
    x64/mips3_x64_branch.h \
    x64/mips3_x64_misc.h \
    x64/mips3_x64_rw.h \
    x64/mips3_x64_shift.h \
    x64/udis86/decode.h \
    x64/udis86/extern.h \
    x64/udis86/input.h \
    x64/udis86/itab.h \
    x64/udis86/syn.h \
    x64/udis86/types.h \
    x64/udis86/udis86.h \
    x64/mips3_x64_cop1.h

QMAKE_CXXFLAGS += -fno-operator-names -Wno-unused-parameter

FORMS +=

RESOURCES += \
    ui/rscr.qrc

DISTFILES +=

