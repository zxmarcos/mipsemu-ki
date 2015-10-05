#ifndef MEMORY
#define MEMORY

#include "common.h"
#include <vector>

namespace mips
{

namespace mem
{

extern int g_vram_base_addr;
extern int g_input_dips[];

enum {
    MEM_AREA_RAM = 0,
    MEM_AREA_ROM = 1,
};

struct mem_area {
    addr_t address;
    void *base_ptr;
    size_t size;
    unsigned flags;

    mem_area() {
        address = 0;
        base_ptr = nullptr;
        size = 0;
        flags = 0;
    }

#if 0
    mem_area& operator=(const mem_area& rhs) {
        address = rhs.address;
        base_ptr = rhs.address;
        size = rhs.size;
        flags = ths.flags;
    }
#endif

    mem_area& operator=(const mem_area& rhs) = default;

    inline bool has(addr_t addr) const {
        return (addr >= address && addr <= (address + size));
    }

    template<typename T>
    T read(addr_t adr) const {
        if (has(address)) {
            return *((T*) ((uint8_t*) base_ptr + adr - address));
        }
        return 0;
    }

    template<typename T>
    void write(addr_t adr, T value) {
        if (has(address)) {
            T *ptr = ((T*) ((uint8_t*) base_ptr + adr - address));

            // verifica se a memória é apenas para leitura
            if (~flags & MEM_AREA_ROM)
                *ptr = value;
        }
    }

    template<typename T>
    inline T fast_read(addr_t adr) const {
        return *((T*)  ((uint8_t*) base_ptr + (adr - address)));
    }

    template<typename T>
    inline void fast_write(addr_t adr, T value) {
        T *ptr = ((T*)  ((uint8_t*) base_ptr + adr - address));
        // verifica se a memória é apenas para leitura
        if (~flags & MEM_AREA_ROM)
            *ptr = value;
    }
};

void register_area(const mem_area& area);
void unregister_area(const mem_area& area);
const vector<mem_area> *get_areas();

void write_byte(addr_t address, uint8_t value);
void write_half(addr_t address, uint16_t value);
void write_word(addr_t address, uint32_t value);
void write_dword(addr_t address, uint64_t value);

uint8_t read_byte(addr_t address);
uint16_t read_half(addr_t address);
uint32_t read_word(addr_t address);
uint64_t read_dword(addr_t address);


}

}

#endif // MEMORY

