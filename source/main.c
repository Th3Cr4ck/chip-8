#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy
#include <time.h>

#define NNN(instr) ((uint16_t)(instr & 0x0FFF))
#define N(instr) ((uint8_t)(instr & 0x000F))
#define X(instr) ((uint8_t)((instr & 0x0F00) >> 8))
#define Y(instr) ((uint8_t)((instr & 0x00F0) >> 4))
#define KK(instr) ((uint8_t)(instr & 0x00FF))

uint16_t fetch(uint16_t pc, uint8_t *mem);
void print_regs(uint8_t *regs);
void print_mem(uint8_t *mem, size_t n);

int main() {

  uint16_t pc; // 12 bits for 4096 memory locations

  srand(time(NULL));

  // Memoria y registros
  uint16_t i_reg;
  uint8_t *v_regs = malloc(sizeof(uint8_t) * 16);  // Registers V0 .. VF
  uint8_t *mem = malloc(sizeof(uint8_t) * 4096);   // 4KB RAM
  uint16_t *stack = malloc(sizeof(uint16_t) * 16); // Subroutines stack

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
  memcpy(&mem[0x200], tests, sizeof(tests));
  print_mem(mem, sizeof(tests));

  // Program starts at 0x0200
  pc = 0x0200;

  printf("INICIO\n");
  print_regs(v_regs);
  printf("\n");

  for (int i = 0; i < sizeof(tests)/2; i++) {

    // Fetch instruction
    uint8_t opcode_high = mem[pc];
    uint8_t opcode_low = mem[pc + 1];
    uint16_t op = (opcode_high << 8) | opcode_low;

    printf("Opcode: %4x\n", op);

    // Decode and execute
    uint16_t b1 = (op & 0xF000);
    uint16_t b2 = (op & 0x0F00);
    uint16_t b3 = (op & 0x00F0);
    uint16_t b4 = (op & 0x000F);

    if (b1 == 0x6000) {
      // (6xkk) LD, Vx, byte
      v_regs[X(op)] = KK(op);

    } else if (b1 == 0x7000) {
      // (7xkk) ADD Vx, byte
      v_regs[X(op)] += KK(op);

    } else if (b1 == 0x8000) {

      switch (b4) {
      case 0x0000:
        // (8xy0) LD Vx, Vy
        v_regs[X(op)] = v_regs[Y(op)];
        break;

      case 0x0001:
        // (8xy1) OR Vx, Vy
        v_regs[X(op)] |= v_regs[Y(op)];
        break;

      case 0x0002:
        // (8xy2) AND Vx, Vy
        v_regs[X(op)] &= v_regs[Y(op)];
        break;

      case 0x0003:
        // (8xy3) XOR Vx, Vy
        v_regs[X(op)] ^= v_regs[Y(op)];
        break;

      case 0x0004:
        // (8xy4) ADD Vx, Vy
        v_regs[0xF] = (((uint16_t)(v_regs[X(op)] + (uint16_t)v_regs[Y(op)])) & 0xFF00) > 0; // VF = carry
        v_regs[X(op)] += v_regs[Y(op)];
        break;

      case 0x0005:
        // (8xy5) SUB Vx, Vy
        v_regs[0xF] = X(op) > Y(op); // VF = Not borrow
        v_regs[X(op)] -= v_regs[Y(op)];
        break;

      case 0x0006:
        // (8xy6) SHR Vx {, Vy}
        v_regs[0xF] = v_regs[X(op)] & 0x01; // VF = LSB shifted
        v_regs[X(op)] >>= 1;
        break;

      case 0x0007:
        // (8xy7) SUBN Vx, Vy
        v_regs[0xF] = Y(op) > X(op); // VF = Not borrow
        v_regs[X(op)] = Y(op) - X(op);
        break;

      case 0x000E:
        // (8xyE) SHL Vx {, Vy}
        v_regs[0xF] = v_regs[X(op)] & 0x80; // VF = MSB shifted
        v_regs[X(op)] <<= 1;
        break;

      default:
        printf("OPCODE:%4X inexistent\n", op);
        break;
      }

    } else if (b1 == 0xC000) {
      // (Cxkk) RND Vx, byte
      uint8_t rnd = rand() % 256;
      v_regs[X(op)] = rnd & KK(op);

    } else {
      printf("OPCODE:%4X not implemented yet\n", op);
    }

    printf("RESULTADO:\n");
    print_regs(v_regs);
    printf("\n");

    pc += 2;
  }

  return 0;
}

void print_regs(uint8_t *regs) {
  printf("-----------------------------\n");
  for (int i = 0; i < 16; i++) {
    printf("V%1x: %02X\n", i, regs[i]);
  }
}

void print_mem(uint8_t *mem, size_t n) {
  printf("-----------------------------\n");
  printf("MEM:\n");
  for (int i = 0; i < n; i++) {
    uint16_t addr = 0x200 + i;
    printf("mem[0x%3X] = %02X \n", addr, mem[addr]);
  }
}

uint16_t fetch(uint16_t pc, uint8_t *mem) {
  uint8_t opcode_high = mem[pc];
  uint8_t opcode_low = mem[pc + 1];

  return (opcode_high << 8) | opcode_low;
}
