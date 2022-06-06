#include "memory.h"
#include "register.h"
#include "decode.h"
#include <stdio.h>
#include "ALU.h"



extern unsigned int REGISTER[32];
// register.c�� �ִ� PC�� HI, LO ���� ������
extern int PC;
extern int HI;
extern int LO;
extern int* Z;

enum FROM_ALU {
    Add = 8,
    Sub = 9,
    And = 12,
    Or = 13,
    Xor = 14,
    Nor = 15,
    SetLess = 4
};

int add(int rd, int rs, int rt) {
    REGISTER[rd] = ALU(REGISTER[rs], REGISTER[rt], Add, &Z);
    return 0;
}


int addi(int rt, int rs, int imm) {
    REGISTER[rt] = ALU(REGISTER[rs], imm, Add, &Z);
    return 0;
}

int sub(int rd, int rs, int rt) {
    REGISTER[rd] = ALU(REGISTER[rs], REGISTER[rt], Sub, &Z);
    return 0;
}

int aNd(int rd, int rs, int rt) {
    REGISTER[rd] = ALU(REGISTER[rs], REGISTER[rt], And, &Z);

    return 0;
}

int oR(int rd, int rs, int rt) {
    REGISTER[rd] = ALU(REGISTER[rs], REGISTER[rt], Or, &Z);

    return 0;
}

int xOr(int rd, int rs, int rt) {
    REGISTER[rd] = ALU(REGISTER[rs], REGISTER[rt], Xor, &Z);

    return 0;
}

int nor(int rd, int rs, int rt) {
    REGISTER[rd] = ALU(REGISTER[rs], REGISTER[rt], Nor, &Z);
    return 0;
}

//SET ON LESS THAN
int slt(int rd, int rs, int rt) {
    REGISTER[rd] = ALU(REGISTER[rs], REGISTER[rt], SetLess, &Z);
    return 0;
}

//jump and link
//���� ��ɾ��� �ּ�(PC+4)�� $ra�� �����ϰ� �ش� address�� ����
int jal(int address) {
    int $ra = 31;
    REGISTER[$ra] = PC;
    PC = ((PC + 4) & 0xf0000000) | (address << 2);
    return 0;
}

// 0���� ���� ��� �б� �Ѵ�.
int bltz(int rs, int rt, int imm) {
    if (REGISTER[rs] < REGISTER[rt])
        PC += (imm * 4);
    return 0;
}

// beq�� �� �ǿ����ڸ� ���� ������ ���ο� �ּҷ� �̵�, ���� ������ ������ ��ɾ ����
int beq(int rs, int rt, int imm) {
    if (REGISTER[rs] == REGISTER[rt])
        PC += (imm * 4);
    return 0;
}

// bne�� �� �ǿ����ڸ� ���� �ٸ��� ���ο� �ּҷ� �̵�, ������ ������ ��ɾ ����
int bne(int rs, int rt, int imm) {
    if (REGISTER[rs] != REGISTER[rt])
        PC += (imm * 4);
    return 0;
}



int slti(int rt, int rs, int imm) {
    int ret;
    ret = REG(rs, 0, 0);
    REGISTER[rt] = (ALU(ret, imm, 4, NULL));
    return 0;
}

int andi(int rt, int rs, int imm) {
    int ret;
    ret = REG(rs, 0, 0);
    ALU(ret, imm, 0b1100, NULL);
    REG(rt, ret, 1);
    return 0;
}
int ori(int rt, int rs, int imm) {
    int ret;
    ret = REG(rs, 0, 0);
    ret = ALU(ret, imm, 0b1101, NULL);
    REG(rt, ret, 1);
    return 0;
}
int xori(int rt, int rs, int imm) {
    int ret;
    ret = REG(rs, 0, 0);
    ret = ALU(ret, imm, 0b1110, NULL);
    REG(rt, ret, 1);
    return 0;
}
int lui(int rt, int imm) {//load upper immediate
    REGISTER[rt] = (imm & 0xffff) << 16;

    return 0;
}

int lw(int rt, int imm, int rs) {
    int ret;
    ret = MEM(REG(rs, 0, 0) + imm, 0, 0, 2);
    REG(rt, ret, 1);
    return 0;
}
int sw(int rt, int imm, int rs) {
    MEM(REG(rs, 0, 0) + imm, REG(rt, 0, 0), 1, 2);
    return 0;

}
int lb(int rt, int imm, int rs) {
    int ret;
    ret = MEM(REG(rs, 0, 0) + imm, 0, 0, 0);
    REG(rt, ret, 1);
    return 0;

}
int sb(int rt, int imm, int  rs) {
    MEM(REG(rs, 0, 0) + imm, REG(rt, 0, 0), 1, 0);
    return 0;

}
int lbu(int rt, int imm, int rs) {
    int ret;
    ret = MEM(REG(rs, 0, 0) + imm, 0, 0, 0);
    REG(rt, ret, 1);
    return 0;

}

int sll(int rd, int rt, int sh) {
    REGISTER[rd] = ALU(sh, REGISTER[rt], 1, &Z);
    return 0;
}

int srl(int rd, int rt, int sh) {
    REGISTER[rd] = ALU(sh, REGISTER[rt], 2, NULL);
    return 0;
}

int sra(int rd, int rt, int sh) {
    REGISTER[rd] = ALU(sh, REGISTER[rt], 3, NULL);
    return 0;
}

int jr(int rs) {
    PC = REGISTER[rs];
    return 0;
}
int j(int address) {
    PC = ((PC + 4) & 0xf0000000) | (address << 2);
    return 0;
}
int syscall() {
    switch (REGISTER[2]) {
    case 1:  printf("%d\n", REGISTER[4]); break;
    case 10: printf("\n\n*** EXIT THE PROGRAM ***\n\n"); return 1;
    case 11: printf("%c\n", REGISTER[4]); break;
    }
    return 0;
}

int mfhi(int rd) {
    REGISTER[rd] = HI;
    return 0;
}

int mflo(int rd) {

    REGISTER[rd] = LO;
    return 0;
}

int mul(int rs, int rt) {

    long long int result = (long long int)REGISTER[rs] * (long long int)REGISTER[rt];
    HI = result >> 32;
    LO = (result << 32) >> 32;

    return 0;
}