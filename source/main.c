#include "executer.h"
#include "mem_regs.h"
#include <stdint.h>
#include <stdio.h>

uint16_t fetch(uint16_t pc, uint8_t *mem);
void print_regs(uint8_t *regs);
void print_mem(uint8_t *mem, size_t n);

int main() {

  // Test Program
  uint8_t tests[] = {
      0x65, 0x83, // LD Vx, byte
      0x66, 0xF0, // LD Vx, byte
      0x84, 0x50, // LD Vx, Vy
      0x75, 0x01, // ADD Vx, byte
      0x87, 0x61, // OR Vx, Vy
      0x87, 0x52, // AND Vx, Vy
      0x85, 0x44, // ADD Vx, Vy
      0x84, 0x55, // SUB Vx, Vy
      0x87, 0x56, // SHR Vx
      0x87, 0x5E, // SHL Vx
  };

  mem_init();
  mem_load(START_ADDR, tests, sizeof(tests));
  mem_print(START_ADDR, sizeof(tests));

  exec_init();

  printf("INICIO\n");
  regs_print();
  printf("\n");

  exec_program(START_ADDR + sizeof(tests));

  return 0;
}
