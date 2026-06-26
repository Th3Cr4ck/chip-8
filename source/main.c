#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy

#define NNN(instr) ((uint16_t)(instr & 0x0FFF))
#define N(instr) ((uint8_t)(instr & 0x000F))
#define X(instr) ((uint8_t)((instr & 0x0F00) >> 8))
#define Y(instr) ((uint8_t)((instr & 0x00F0) >> 4))
#define KK(instr) ((uint8_t)(instr & 0x00FF))

uint16_t fetch(uint16_t pc, uint8_t *mem);
void print_regs(uint8_t *regs);
void print_mem(uint8_t *mem, size_t n);
void decode(uint16_t instruction);
void execute(void);

int main() {

  uint16_t pc; // 12 bits for 4096 memory locations

  // Memoria y registros
  uint16_t i_reg;
  uint8_t *v_regs = malloc(sizeof(uint8_t) * 16);  // Registers V0 .. VF
  uint8_t *mem = malloc(sizeof(uint8_t) * 4096);   // 4KB RAM
  uint16_t *stack = malloc(sizeof(uint16_t) * 16); // Subroutines stack

  // Test Program
  uint8_t tests[] = {
    0x65, 0x83,
    0x75, 0x01
  };
  memcpy(&mem[0x200], tests, sizeof(tests));
  print_mem(mem, sizeof(tests));

  // Program starts at 0x0200
  pc = 0x0200;

  printf("INICIO\n");
  print_regs(v_regs);
  printf("\n");

  for (int i = 0; i < 2; i++) {

    // Fetch instruction
    uint8_t opcode_high = mem[pc];
    uint8_t opcode_low = mem[pc + 1];
    uint16_t instr = (opcode_high << 8) | opcode_low;

    printf("Opcode: %4x\n", instr);

    // Decode and execute
    uint16_t b1 = (instr & 0xF000);
    uint16_t b2 = (instr & 0x0F00);
    uint16_t b3 = (instr & 0x00F0);
    uint16_t b4 = (instr & 0x000F);

    if (b1 == 0x6000) {
      // (6xkk) Set Vx = kk
      v_regs[X(instr)] = KK(instr);

    } else if (b1 == 0x7000) {
      // (7xkk) Set Vx = Vx + kk
      v_regs[X(instr)] += KK(instr);

    } else if (b1 == 0x8000) {

      switch (b4) {
      case 0x0000:
        // (8xy0) LD Vx, Vy
        v_regs[X(instr)] = v_regs[Y(instr)];
        break;

      case 0x0001:
        // (8xy1) OR Vx, Vy
        v_regs[X(instr)] |= v_regs[Y(instr)];
        break;

      case 0x0002:
        // (8xy2) AND Vx, Vy
        v_regs[X(instr)] &= v_regs[Y(instr)];
        break;

      case 0x0003:
        // (8xy3) XOR Vx, Vy
        v_regs[X(instr)] ^= v_regs[Y(instr)];
        break;

      case 0x0004:
        // (8xy4) ADD Vx, Vy
        v_regs[0xF] =
            ((uint16_t)(v_regs[X(instr)] + (uint16_t)v_regs[Y(instr)])) &
            0xF0; // VF = carry
        v_regs[X(instr)] += v_regs[Y(instr)];
        break;

      case 0x0005:
        // (8xy5) SUB Vx, Vy
        v_regs[0xF] = X(instr) > Y(instr); // VF = Not borrow
        v_regs[X(instr)] -= v_regs[Y(instr)];
        break;

      case 0x0006:
        // (8xy6) SHR Vx {, Vy}
        v_regs[0xF] = v_regs[X(instr)] & 0x01; // VF = LSB shifted
        v_regs[X(instr)] >>= 1;
        break;

      case 0x0007:
        // (8xy7) SUBN Vx, Vy
        v_regs[0xF] = Y(instr) > X(instr); // VF = Not borrow
        v_regs[X(instr)] = Y(instr) - X(instr);
        break;

      case 0x000E:
        // (8xyE) SHL Vx {, Vy}
        v_regs[0xF] = v_regs[X(instr)] & 0x80; // VF = MSB shifted
        v_regs[X(instr)] <<= 1;
        break;

      default:
        printf("OPCODE:%4X inexistent\n", instr);
        break;
      }
    } else {
      printf("OPCODE:%4X not implemented yet\n", instr);
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
  for (int i = 0; i < 15; i++) {
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
