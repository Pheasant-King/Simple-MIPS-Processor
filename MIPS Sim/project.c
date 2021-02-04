#include "spimcore.h"

/*
  Group: 42
  William Askew
  Terrence Chung
  Maduabuchi Udokwu
*/

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    if(ALUControl == '0')
      *ALUresult = A + B;
    else if(ALUControl == '1')
      *ALUresult = A - B;
    else if(ALUControl == '2')
    {
      if((int)A < (int)B)
      {
        *Zero = '1';
        *ALUresult = 1;
      }
      else
      {
        *Zero = '0';
        *ALUresult = 0;
      }
    }
    else if(ALUControl == '3')
    {
      if(A < B)
      {
        *Zero = '1';
        *ALUresult = 1;
      }
      else
      {
        *Zero = '0';
        *ALUresult = 0;
      }
    }
    else if(ALUControl == '4')
      *ALUresult = A & B;
    else if(ALUControl == '5')
      *ALUresult = A | B;
    else if(ALUControl == '6')
      *ALUresult = B << 16;
    else if(ALUControl == '7')
      *ALUresult = ~A;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
  if((PC%4)!= 0)
    return 1;
  if((PC>>2) > 16383)
    return 1;

  *instruction = Mem[PC>>2];

  return 0;
}

/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
 *op = (instruction & 0xFC000000) >> 26;
 *r1 = (instruction & 0x3E00000) >> 21;
 *r2 = (instruction & 0x1F0000) >> 16;
 *r3 = (instruction & 0x0F800) >> 11;
 *funct = (instruction & 0x3F);
 *offset = (instruction & 0x0FFFF);
 *jsec = (instruction & 0x3FFFFFF);
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
  if(op == 0) // R - Type
  {
    controls->ALUOp = '7';
    controls->ALUSrc = '0';
    controls->Branch = '0';
    controls->Jump = '0';
    controls->MemRead = '0';
    controls->MemtoReg = '0';
    controls->MemWrite = '0';
    controls->RegDst = '1';
    controls->RegWrite = '1';

    return 0;
  }
  else if(op == 8) // addi
  {
    controls->ALUOp = '0';
    controls->ALUSrc = '1';
    controls->Branch = '0';
    controls->Jump = '0';
    controls->MemRead = '0';
    controls->MemtoReg = '0';
    controls->MemWrite = '0';
    controls->RegDst = '0';
    controls->RegWrite = '1';

    return 0;
  }
  else if(op == 15) // lui
  {
    controls->ALUOp = '6';
    controls->ALUSrc = '1';
    controls->Branch = '0';
    controls->Jump = '0';
    controls->MemRead = '2';
    controls->MemtoReg = '2';
    controls->MemWrite = '0';
    controls->RegDst = '0';
    controls->RegWrite = '1';

    return 0;
  }
  else if(op == 10) // slti
  {
    controls->ALUOp = '2';
    controls->ALUSrc = '1';
    controls->Branch = '0';
    controls->Jump = '0';
    controls->MemRead = '0';
    controls->MemtoReg = '0';
    controls->MemWrite = '0';
    controls->RegDst = '0';
    controls->RegWrite = '1';

    return 0;
  }
  else if(op == 11) // sltiu
  {
    controls->ALUOp = '3';
    controls->ALUSrc = '1';
    controls->Branch = '0';
    controls->Jump = '0';
    controls->MemRead = '0';
    controls->MemtoReg = '0';
    controls->MemWrite = '0';
    controls->RegDst = '0';
    controls->RegWrite = '1';

    return 0;
  }
  else if(op == 35) // Load word I -Type
  {
    controls->ALUOp = '0';
    controls->ALUSrc = '1';
    controls->Branch = '0';
    controls->Jump = '0';
    controls->MemRead = '1';
    controls->MemtoReg = '1';
    controls->MemWrite = '0';
    controls->RegDst = '0';
    controls->RegWrite = '1';

    return 0;
  }
  else if(op == 43) // Store word I - Type
  {
    controls->ALUOp = '0';
    controls->ALUSrc = '1';
    controls->Branch = '0';
    controls->Jump = '0';
    controls->MemRead = '0';
    controls->MemtoReg = '2';
    controls->MemWrite = '1';
    controls->RegDst = '2';
    controls->RegWrite = '0';

    return 0;
  }
  else if(op == 4) // Branch I - Type
  {
    controls->ALUOp = '1';
    controls->ALUSrc = '0';
    controls->Branch = '1';
    controls->Jump = '0';
    controls->MemRead = '0';
    controls->MemtoReg = '2';
    controls->MemWrite = '0';
    controls->RegDst = '2';
    controls->RegWrite = '0';

    return 0;
  }
  else if(op == 2) // Jump J - type
  {
    controls->ALUOp = '1';
    controls->ALUSrc = '0';
    controls->Branch = '0';
    controls->Jump = '1';
    controls->MemRead = '0';
    controls->MemtoReg = '2';
    controls->MemWrite = '0';
    controls->RegDst = '2';
    controls->RegWrite = '0';

    return 0;
  }

  return 1;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
  *data1 = Reg[r1];

  *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
  if((offset & 0x8000)>>15 == 1)
    *extended_value = offset + 0xFFFF0000; //fill with 1's in 17-31 bits.
  else
    *extended_value = offset;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
  if(ALUOp == '7')
  {
    if(funct == 32) //add
      ALU(data1, data2, '0', ALUresult, Zero);
    else if(funct == 34) //sub
      ALU(data1, data2, '1', ALUresult, Zero);
    else if(funct == 36) //and
      ALU(data1, data2, '4', ALUresult, Zero);
    else if(funct == 37) //or
      ALU(data1, data2, '5', ALUresult, Zero);
    else if(funct == 42) //slt
      ALU(data1, data2, '2', ALUresult, Zero);
    else if(funct == 43) //sltu
      ALU(data1, data2, '3', ALUresult, Zero);
    else if(funct == 39) //not
      ALU(data1, data2, '7', ALUresult, Zero);
    else
      return 1;
  }
  else if(ALUOp == '0')
    ALU(data1, extended_value, '0', ALUresult, Zero);
  else if(ALUOp == '1')
  {
    ALU(data1, data2, '1', ALUresult, Zero);
      if(*ALUresult == 0)
        *Zero = '1';
      else
        *Zero = '0';
    }
  else if(ALUOp == '2')
    ALU(data1, data2, '2', ALUresult, Zero);
  else if(ALUOp == '3')
    ALU(data1, data2, '3', ALUresult, Zero);
  else if(ALUOp == '6')
    ALU(data1, extended_value, '6', ALUresult, Zero);
  else
    return 1;

  return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
  if(MemWrite == '1')
    {
      if((ALUresult)%4 != 0)
        return 1;
      else if(ALUresult>>2 > 16383)
        return 1;
      else
        ;

      Mem[ALUresult>>2] = data2;
      return 0;
    }
  else if(MemRead == '1')
    {
    if((ALUresult)%4 != 0)
      return 1;
    else if(ALUresult>>2 > 16383)
        return 1;
    else
      ;

      *memdata = Mem[ALUresult>>2];
      return 0;
    }

  return 0;
}

/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
  if(RegWrite == '1' && MemtoReg == '1')
    {
      if(RegDst == '1')
        Reg[r3] = memdata;
      else
        Reg[r2] = memdata;
    }
  else if(RegWrite == '1' && MemtoReg == '0')
    {
      if(RegDst == '1')
        Reg[r3] = ALUresult;
      else
        Reg[r2] = ALUresult;
    }
    else if(RegWrite == '1' && MemtoReg == '2')
    {
      if(RegDst == '1')
        Reg[r3] = ALUresult;
      else
        Reg[r2] = ALUresult;
    }
    else
      ;
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
  if(Branch == '1')
  {
    *PC += 4;
    if(Zero == '1')
      *PC = *PC + (extended_value<<2);
  }
  else if(Jump == '1')
    *PC = (*PC & 0xF0000000) + (jsec<<2);
  else
    *PC += 4;
}
