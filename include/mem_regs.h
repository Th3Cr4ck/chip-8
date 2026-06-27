#ifndef MEM_REGS_H
#define MEM_REGS_H

#include <stdint.h>
#include <stdlib.h>

#define START_ADDR 0x200

/* MEM */
void mem_init(void);
void mem_load(uint16_t addr, uint8_t *src, size_t size);
uint16_t mem_read(uint16_t addr);
void mem_print(uint16_t addr, size_t size);

/* REGS */
void setVx(uint8_t Vx, uint8_t byte);
uint8_t readVx(uint8_t Vx);
void regs_print(void);

#endif
