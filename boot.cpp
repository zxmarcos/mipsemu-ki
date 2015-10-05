// ============================================================================
// Portable MIPS III emulator
//
// Marcos Medeiros
// ============================================================================
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <cstdint>
#include <cstring>
#include "boot.h"
#include "memory.h"
#include "mips3.h"

using namespace std;

uint8_t *g_boot_rom = nullptr;
uint8_t *g_rambase[2] = { nullptr, nullptr };

static const mips::addr_t BOOT_ADDR = 0x1FC00000;
static const size_t BOOT_SIZE = 0x80000;

static const mips::addr_t RAMBASE_ADDR = 0;
static const size_t RAMBASE_SIZE = 0x80000;

static const mips::addr_t RAMBASE2_ADDR = 0x08000000;
static const size_t RAMBASE2_SIZE = 0x800000;

namespace mem = mips::mem;

bool boot_load_rom(const string &romname)
{
    ifstream is(romname, ios_base::binary);
    if (is.is_open()) {
        try {
            g_boot_rom = new uint8_t[BOOT_SIZE];
            memset(g_boot_rom, 0, BOOT_SIZE);
            is.read((char*) g_boot_rom, BOOT_SIZE);

            // mapeia a rom de boot
            mem::mem_area boot_area;
            boot_area.address = BOOT_ADDR;
            boot_area.size = BOOT_SIZE;
            boot_area.base_ptr = g_boot_rom;
            boot_area.flags = mem::MEM_AREA_ROM;
            mem::register_area(boot_area);

            g_rambase[0] = new uint8_t[RAMBASE_SIZE];
            g_rambase[1] = new uint8_t[RAMBASE2_SIZE];

            // mapeia a RAM
            mem::mem_area rambase;
            rambase.address = RAMBASE_ADDR;
            rambase.size = RAMBASE_SIZE;
            rambase.base_ptr = g_rambase[0];
            rambase.flags = mem::MEM_AREA_RAM;
            mem::register_area(rambase);

            mem::mem_area rambase2;
            rambase2.address = RAMBASE2_ADDR;
            rambase2.size = RAMBASE2_SIZE;
            rambase2.base_ptr = g_rambase[1];
            rambase2.flags = mem::MEM_AREA_RAM;
            mem::register_area(rambase2);

            return true;
        } catch (const bad_alloc& e) {
            throw runtime_error("Não foi possível alocar memória para a ROM de boot");
        }

    }
    return false;
}

void boot_save_rom()
{
    ofstream os("/home/marcos/outrom.bin", ios_base::binary);
    if (os.is_open()) {
        os.write((char*) g_boot_rom, BOOT_SIZE);
    }
}
