#include "executer.h"
#include "mem_regs.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NNN(instr) ((uint16_t)(instr & 0x0FFF))
#define N(instr) ((uint8_t)(instr & 0x000F))
#define X(instr) ((uint8_t)((instr & 0x0F00) >> 8))
#define Y(instr) ((uint8_t)((instr & 0x00F0) >> 4))
#define KK(instr) ((uint8_t)(instr & 0x00FF))
#define FLAG_REG 0xF

static uint16_t pc; // 12 bits for 4096 memory locations

void exec_init(void) {
  srand(time(NULL));
  pc = START_ADDR;
}

void exec_program(uint16_t endAddr) {

  while (pc < endAddr) {

    // Fetch instruction
    uint16_t op = mem_read(pc);
    printf("Opcode: %4X\n", op);

    // Decode and execute
    uint16_t b1 = (op & 0xF000);
    uint16_t b2 = (op & 0x0F00);
    uint16_t b3 = (op & 0x00F0);
    uint16_t b4 = (op & 0x000F);
    uint8_t vx_val, vy_val;

    if (b1 == 0x6000) {
      // (6xkk) LD, Vx, byte
      setVx(X(op), KK(op));

    } else if (b1 == 0x7000) {
      // (7xkk) ADD Vx, byte
      vx_val = readVx(X(op));
      setVx(X(op), KK(op) + vx_val);

    } else if (b1 == 0x8000) {

      switch (b4) {
      case 0x0000:
        // (8xy0) LD Vx, Vy
        vy_val = readVx(Y(op));
        setVx(X(op), vy_val);
        break;

      case 0x0001:
        // (8xy1) OR Vx, Vy
        vx_val = readVx(X(op));
        vy_val = readVx(Y(op));
        setVx(X(op), vx_val | vy_val);
        break;

      case 0x0002:
        // (8xy2) AND Vx, Vy
        vx_val = readVx(X(op));
        vy_val = readVx(Y(op));
        setVx(X(op), vx_val & vy_val);
        break;

      case 0x0003:
        // (8xy3) XOR Vx, Vy
        vx_val = readVx(X(op));
        vy_val = readVx(Y(op));
        setVx(X(op), vx_val ^ vy_val);
        break;

      case 0x0004:
        // (8xy4) ADD Vx, Vy
        vx_val = readVx(X(op));
        vy_val = readVx(Y(op));
        setVx(FLAG_REG, (((uint16_t)(vx_val + (uint16_t)vy_val)) & 0xFF00) >
                            0); // VF = carry
        setVx(X(op), vx_val + vy_val);
        break;

      case 0x0005:
        // (8xy5) SUB Vx, Vy
        vx_val = readVx(X(op));
        vy_val = readVx(Y(op));
        setVx(FLAG_REG, X(op) > Y(op)); // VF = Not borrow
        setVx(X(op), vx_val - vy_val);
        break;

      case 0x0006:
        // (8xy6) SHR Vx {, Vy}
        vx_val = readVx(X(op));
        setVx(FLAG_REG, vx_val & 0x01); // VF = LSB shifted
        setVx(X(op), vx_val >> 1);
        break;

      case 0x0007:
        // (8xy7) SUBN Vx, Vy
        vx_val = readVx(X(op));
        vy_val = readVx(Y(op));
        setVx(FLAG_REG, Y(op) > X(op)); // VF = Not borrow
        setVx(X(op), vy_val - vx_val);
        break;

      case 0x000E:
        // (8xyE) SHL Vx {, Vy}
        vx_val = readVx(X(op));
        setVx(FLAG_REG, vx_val & 0x80); // VF = MSB shifted
        setVx(X(op), vx_val << 1);
        break;

      default:
        printf("OPCODE:%4X inexistent\n", op);
        break;
      }

    } else if (b1 == 0xC000) {
      // (Cxkk) RND Vx, byte
      uint8_t rnd = rand() % 256;
      setVx(X(op), rnd & KK(op));

    } else {
      printf("OPCODE:%4X not implemented yet\n", op);
    }

    printf("RESULTADO:\n");
    regs_print();
    printf("\n");

    pc += 2;
  }
}
