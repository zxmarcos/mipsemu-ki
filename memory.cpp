// ============================================================================
// Portable MIPS III emulator
//
// Marcos Medeiros
// ============================================================================
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <type_traits>
#include "memory.h"
#include "mips3.h"
#include "ide.h"

#include <QDebug>

#ifdef RUN_KI2
#define KI2_MAP 1
#else
#define KI2_MAP 0
#endif

namespace mips
{
namespace mem
{

using namespace ide;

int g_vram_base_addr = 0x30000;
int g_input_dips[3] = { ~0, ~0, ~0 };

static vector<mem_area> m_areas;

void register_area(const mem_area& area)
{
    for (const mem_area& va : m_areas) {
        if (area.has(va.address) || area.has(va.address + va.size)) {
            cout << "Não foi possível registrar a área" << endl;
            return;
        }
    }
    m_areas.push_back(area);
}

void unregister_area(const mem_area& area)
{
    for (auto it = m_areas.begin(); it != m_areas.end(); it++) {
        auto va = *it;
        if (area.has(va.address) || area.has(va.address + va.size)) {
            m_areas.erase(it);
            return;
        }
    }
}

#define IN_RANGE(x, s, e) (x >= s && x <= e)

uint32_t ide_read(addr_t address)
{
    if (IN_RANGE(address, 0x10000100, 0x1000013f))
        return g_ide.read((address - 0x10000100) / 8);
    if (IN_RANGE(address, 0x10000170, 0x10000173))
        return g_ide.read_alternate(6);
}

void ide_write(unsigned address, unsigned value)
{
    if (IN_RANGE(address, 0x10000100, 0x1000013f))
        g_ide.write((address - 0x10000100) / 8, value);
    else
        if (IN_RANGE(address, 0x10000170, 0x10000173))
            g_ide.write_alternate(6, value);
}

template<typename T>
inline T do_read(addr_t address) {
    address &= 0xFFFFFFFF;
    for (const mem_area& area : m_areas) {
        if (area.has(address))
            return area.fast_read<T>(address);
    }


    if (address >= 0x10000100 && address <= 0x10000173)
        return ide_read(address);

    if (IN_RANGE(address, 0x10000080, 0x100000ff)) {
#if KI2_MAP
        const int suffle[8] = {   2,   4,   1,   0,   3,   5,   6,   7 };
        int idx = ((address & 0xFF) - 0x80) / 8;
        switch (suffle[idx]) {

        case 0:
            return g_input_dips[0];
        case 1:
            return g_input_dips[1];
        case 2:
            return 2;
        case 3:
            return 0;
        case 4:
            //qDebug() << "Invalid read" << QString::number(address, 16);
            return g_input_dips[2] & ~0x00003e00;
        }
#else
        switch (address & 0xFF) {
        case 0x90:
            return 2;
        case 0x80:
            return g_input_dips[0];
        case 0x88:
            return g_input_dips[1];
        case 0xa0:
            //qDebug() << "Invalid read" << QString::number(address, 16);
            return g_input_dips[2] & ~0x00003e00;
        }
#endif
    }

    //qDebug() << "Invalid read" << QString::number(address, 16);
    return 0;
}

template<typename T>
inline void do_write(addr_t address, T value) {
    address &= 0xFFFFFFFF;
    for (mem_area& area : m_areas) {
        if (area.has(address))  {
            area.fast_write<T>(address, value);
            return;
        }
    }

    if (address >= 0x10000100 && address <= 0x10000173) {
        ide_write(address, value);
        return;
    }

    if (IN_RANGE(address, 0x10000080, 0x100000ff)) {
        switch (address & 0xFF) {
#if KI2_MAP
        case 0x98:
#else
        case 0x80:
#endif
            g_vram_base_addr = (value & 4) ? 0x58000 : 0x30000;
            return;
        default:
            return;
        }
    }

    qDebug() << "Invalid write" << QString::number(address, 16) << QString::number(value, 16);
}

uint8_t read_byte(addr_t address)
{
    return do_read<uint8_t>(address);
}

uint16_t read_half(addr_t address)
{
    return do_read<uint16_t>(address);
}

uint32_t read_word(addr_t address)
{
    return do_read<uint32_t>(address);
}

uint64_t read_dword(addr_t address)
{
    return do_read<uint64_t>(address);
}


void write_byte(addr_t address, uint8_t value)
{
    do_write(address, value);
}

void write_half(addr_t address, uint16_t value)
{
    do_write(address, value);
}

void write_word(addr_t address, uint32_t value)
{
    do_write(address, value);
}

void write_dword(addr_t address, uint64_t value)
{
    do_write(address, value);
}

const vector<mem_area> *get_areas()
{
    return &m_areas;
}

}
}
