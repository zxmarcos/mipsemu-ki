#include <iostream>
#include <cstdio>
#include "mips3.h"
#include "memory.h"
#include "boot.h"
#include "ide.h"
#include "x64/mips3_x64.h"

#include "ui/mainwindow.h"
#include <QDebug>

using namespace std;
using namespace ide;

int main(int argc, char **argv)
{
#ifdef RUN_KI2
    if (boot_load_rom("ki2-l14.u98")) {
        cout << "ROM carregada com sucesso!" << endl;
    } else {
        cout << "ROM não foi carregada!" << endl;
        exit(-1);
    }
    if (!g_ide.load_disk_image("kinst2.img"))
        return 1;
#else
    if (boot_load_rom("ki-l15d.u98")) {
        cout << "ROM carregada com sucesso!" << endl;
    } else {
        cout << "ROM não foi carregada!" << endl;
        exit(-1);
    }
    if (!g_ide.load_disk_image("kinst.img"))
        return 1;
#endif
#if 1
    return ui_main(argc, argv);
#else
    return mips::drc_main();
#endif
}

