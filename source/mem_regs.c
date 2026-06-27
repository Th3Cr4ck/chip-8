#include "mem_regs.h"
#include <stdio.h>
#include <string.h>

// Memory and registers
static uint16_t i_reg;
static uint8_t *v_regs = NULL; // Registers V0 .. VF
static uint8_t *mem = NULL;    // 4KB RAM
static uint16_t *stack = NULL; // Subroutines stack

void mem_init(void) {
  v_regs = malloc(sizeof(uint8_t) * 16);
  mem = malloc(sizeof(uint8_t) * 4096);
  stack = malloc(sizeof(uint16_t) * 16);
}

void mem_load(uint16_t addr, uint8_t *src, size_t size) {
  memcpy(&mem[addr], src, size);
}

uint16_t mem_read(uint16_t addr) { 
  uint8_t highByte = mem[addr];
  uint8_t lowByte = mem[addr+1];
  return ((uint16_t)highByte << 8) | lowByte;
}

void mem_print(uint16_t addr, size_t size) {
  printf("-----------------------------\n");
  printf("MEM:\n");
  for (int i = 0; i < (int)size; i++) {
    printf("mem[0x%3X] = %02X \n", addr+i, mem[addr+i]);
  }
}

void regs_print(void) {
  printf("-----------------------------\n");
  // printf("REGS:\n");
  for (int i = 0; i < 16; i++) {
    printf("V%1x: %02X\n", i, v_regs[i]);
  }
}

void setVx(uint8_t Vx, uint8_t byte) { v_regs[Vx] = byte; }

uint8_t readVx(uint8_t Vx) { return v_regs[Vx]; }
