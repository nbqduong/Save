#include "MCU_MCX_protocol.h"

int16 THIS_AXIS;

void SET_AXIS(int8 axis) //to set  current axis because each command can control one axis at a time
{
   THIS_AXIS = axis;
}

int16 RETURN_AXIS()
{
      return THIS_AXIS;
}


void Output_addr(unsigned int8 addr) // Define A0 A1 A2 pins for using register
{
   unsigned int1 _addr = 0;
   _addr = ( addr&0b001)       ;    output_bit(A0, _addr);
   _addr = ((addr&0b010)   >>1);    output_bit(A1, _addr);
   _addr = ((addr&0b100)   >>2);    output_bit(A2, _addr);
   output_bit(PIN_E3,0);
}

void Send_data(unsigned int16 data16)// Send data from Pic24 to MCX via D0-D15 -- Decription in 5.2 
{ 
//While WRN is Low, CSN and A3~A0 must be determined. Around when
//WRN is up, the levels of D15~D0 must be determined because the
//data is latched in the write register when WRN is up
   CSN_L;
   WRN_L; //its level is Low while data is being written to MC
   output_b(data16);
   WRN_H;
   CSN_H;
}

unsigned int16 Get_data() // Get data from MCX -- Decription in 5.2
{
   //Set CSN at Low and RDN at Low, and while RDN is Low, the read register 
   //data selected by A3~A0 address signals is output to the data bus
   CSN_L;
   RDN_L;
   unsigned int16 data16 = 0;
   data16 = input_b();
   RDN_H;
   CSN_H;
   return data16;
}

void Write_reg(unsigned int8 addr,unsigned int16 data16) //Write data to registers
{
   Output_addr(addr); // Config A0 A1 A2 pins first
   Send_data(data16);
}

unsigned int16 Read_reg(unsigned int8 addr)   //Read data from registers
{
   Output_addr(addr);// Config A0 A1 A2 pins first
   return Get_data();
}

//Cung cap giao thuc de MCU viet vao MCX và doc du lieu tu MCX
void Write_MCX514(int32 addr,int16 *ram, int words){
  write_reg(addr,*ram);
}

void Read_MCX514(int32 addr,int16 *ram, int words){
  *ram = Read_reg(addr);
}


// 0x0F because if 0x07 not enough address space to declare variable.
addressmod (MCX514_reg, Read_MCX514, Write_MCX514, 0x00, 0x0F);

unsigned int16 MCX514_reg REG0; //auto generate addr at 0x00
unsigned int16 MCX514_reg REG1; //auto generate addr at 0x02
unsigned int16 MCX514_reg REG2; //auto generate addr at 0x04
unsigned int16 MCX514_reg REG3; //auto generate addr at 0x06
unsigned int16 MCX514_reg REG4; //auto generate addr at 0x08
unsigned int16 MCX514_reg REG5; //auto generate addr at 0x0A
unsigned int16 MCX514_reg REG6; //auto generate addr at 0x0C
unsigned int16 MCX514_reg REG7; //auto generate addr at 0x0E

// Relocate absolute addr
#locate REG0 = 0x00
#locate REG1 = 0x01
#locate REG2 = 0x02
#locate REG3 = 0x03
#locate REG4 = 0x04
#locate REG5 = 0x05
#locate REG6 = 0x06
#locate REG7 = 0x07


void Read_CMD(int32 addr,int32 *ram, int bytes){
   volatile unsigned int16 dwordL,dwordH;
   unsigned int32 temp32;
   addr = ((THIS_AXIS << 8)|addr);
   REG0 = (unsigned int16)addr;
   dwordL = REG6;
   dwordH = REG7;
   temp32 = dwordH;
   temp32 = (temp32<<16)|dwordL;
   *ram = temp32;
}

//Khi gia tri ghi vao lon hon 16bit (vi pic24F la bit 16bit) 
//compiler se tu ngat ra thanh 2 doan low dword va high dword.
void Write_CMD(int32 addr,int16 *ram, int bytes){
   volatile int16 dwordL, dwordH;
   dwordL = *ram++;
   REG6 = dwordL;
   if(bytes==4){
      dwordH = *ram;
      REG7 = dwordH;
   }
   addr = ((THIS_AXIS << 8)|addr);
   REG0 = (unsigned int16)addr;
}

addressmod (MCX514_cmd, Read_CMD, Write_CMD, 0x00, 0xFF);
//CMD FOR WRITING DATA
int32 MCX514_cmd wJK;
int32 MCX514_cmd wDJ;
int32 MCX514_cmd wAC;
int32 MCX514_cmd wDC;
int32 MCX514_cmd wSV;
int32 MCX514_cmd wDV;
int32 MCX514_cmd wTP;
int32 MCX514_cmd wDP;
int32 MCX514_cmd wLP;
int32 MCX514_cmd wRP;
int32 MCX514_cmd wSP;
int32 MCX514_cmd wSM;
int16 MCX514_cmd wAO;
int32 MCX514_cmd wLX;
int32 MCX514_cmd wRX;
int32 MCX514_cmd wMR0;
int32 MCX514_cmd wMR1;
int32 MCX514_cmd wMR2;
int32 MCX514_cmd wMR3;
int32 MCX514_cmd wHV;
int32 MCX514_cmd wIV;
int32 MCX514_cmd wTM;
int32 MCX514_cmd wSP1;
int16 MCX514_cmd wSP2;


#locate wJK =  0x0000   
#locate wDJ =  0x0001
#locate wAC =  0x0002
#locate wDC =  0x0003
#locate wSV =  0x0004
#locate wDV =  0x0005
#locate wTP =  0x0006
#locate wDP =  0x0007
#locate wCP =  0x0007
#locate wLP =  0x0009
#locate wRP =  0x000A
#locate wSP =  0x000B
#locate wSM =  0x000C
#locate wAO =  0x000D
#locate wLX =  0x000E

#locate wRX =  0x000F
#locate wMR0 = 0x0010
#locate wMR1 = 0x0011
#locate wMR2 = 0x0012
#locate wMR3 = 0x0013
#locate wHV =  0x0014
#locate wIV =  0x0015
#locate wTM =  0x0016
#locate wSP1 = 0x0017
#locate wSP2 = 0x0018


//CMD FOR WRITING MODE
int16 MCX514_cmd wMRM;
int16 MCX514_cmd wP1M;
int16 MCX514_cmd wP2M;
int16 MCX514_cmd wH1M;
int16 MCX514_cmd wH2M;
int16 MCX514_cmd wFLM;
int16 MCX514_cmd wS0M;
int16 MCX514_cmd wS1M;
int16 MCX514_cmd wS2M;
int16 MCX514_cmd wS3M;

#locate wMRM = 0x0020
#locate wP1M = 0x0021
#locate wP2M = 0x0022
#locate wH1M = 0x0023
#locate wH2M = 0x0024
#locate wFLM = 0x0025
#locate wS0M = 0x0026
#locate wS1M = 0x0027
#locate wS2M = 0x0028
#locate wS3M = 0x0029

//CMD FOR READING DATA
unsigned   int32 MCX514_cmd rLP;
unsigned   int32 MCX514_cmd rRP;
unsigned   int32 MCX514_cmd rCV;
unsigned   int32 MCX514_cmd rCA;
unsigned   int32 MCX514_cmd rMR0;
unsigned   int32 MCX514_cmd rMR1;
unsigned   int32 MCX514_cmd rMR2;
unsigned   int32 MCX514_cmd rMR3;
unsigned   int32 MCX514_cmd rCT;
unsigned   int16 MCX514_cmd rWR1;
unsigned   int16 MCX514_cmd rWR2;
unsigned   int16 MCX514_cmd rWR3;
unsigned   int16 MCX514_cmd rMRM;
unsigned   int16 MCX514_cmd rP1M;
unsigned   int16 MCX514_cmd rP2M;
unsigned   int32 MCX514_cmd rAC;
unsigned   int32 MCX514_cmd rSV;
unsigned   int32 MCX514_cmd rDV;
unsigned   int32 MCX514_cmd rTP;
unsigned   int32 MCX514_cmd rSP1;
   
#locate rLP =  0x0030
#locate rRP =  0x0031
#locate rCV =  0x0032
#locate rCA =  0x0033
#locate rMR0 = 0x0034
#locate rMR1 = 0x0035
#locate rMR2 = 0x0036
#locate rMR3 = 0x0037
#locate rCT =  0x0038
#locate rWR1 = 0x003D
#locate rWR2 = 0x003E
#locate rWR3 = 0x003F
#locate rMRM = 0x0040
#locate rP1M = 0x0041
#locate rP2M = 0x0042
#locate rAC =  0x0043
#locate rSV =  0x0044
#locate rDV =  0x0045
#locate rTP =  0x0046
#locate rSP1 = 0x0047

void Read_CMD_2(int32 addr,int32 *ram, int bytes){
   
}

void Write_CMD_2(int32 addr,int16 *ram, int bytes){
   addr = ((THIS_AXIS << 8)|addr);
   REG0 = (unsigned int16)addr;
}

addressmod (MCX514_CMD_2, Read_CMD_2, Write_CMD_2, 0x00, 0xFF);

unsigned int16 MCX514_CMD_2 DRVRL;
unsigned int16 MCX514_CMD_2 DRVNR;
unsigned int16 MCX514_CMD_2 DRVVP;
unsigned int16 MCX514_CMD_2 DRVVM;
unsigned int16 MCX514_CMD_2 DRVAB;
unsigned int16 MCX514_CMD_2 DRVSBRK;
unsigned int16 MCX514_CMD_2 DRVFBRK;
unsigned int16 MCX514_CMD_2 DIRCP;
unsigned int16 MCX514_CMD_2 DIRCM;
unsigned int16 MCX514_CMD_2 HMSRC;

#locate      DRVRL     =   0x0050      // Relative position driving
#locate      DRVNR     =   0x0051      // Counter relative position driving
#locate      DRVVP     =   0x0052      // + Direction continuous pulse driving
#locate      DRVVM     =   0x0053      // ? Direction continuous pulse driving
#locate      DRVAB     =   0x0054      // Absolute position driving
#locate      DRVSBRK   =   0x0056      // Decelerating stop
#locate      DRVFBRK   =   0x0057      // Instant stop
#locate      DIRCP     =   0x0058      // Direction signal + setting
#locate      DIRCM     =   0x0059      // Direction signal ? setting
#locate      HMSRC     =   0x005A      // Automatic home search execution

unsigned int16 MCX514_CMD_2 VINC;
unsigned int16 MCX514_CMD_2 VDEC;
unsigned int16 MCX514_CMD_2 DCC;
unsigned int16 MCX514_CMD_2 TMSTA;
unsigned int16 MCX514_CMD_2 TMSTP;
unsigned int16 MCX514_CMD_2 SPSTA;
unsigned int16 MCX514_CMD_2 DHOLD;
unsigned int16 MCX514_CMD_2 DFREE;
unsigned int16 MCX514_CMD_2 R2CLR;
unsigned int16 MCX514_CMD_2 RR3P0;
unsigned int16 MCX514_CMD_2 RR3P1;
unsigned int16 MCX514_CMD_2 MFPC;
unsigned int16 MCX514_CMD_2 NOP;
unsigned int16 MCX514_CMD_2 MRST;

#locate       VINC    =     0x0070      // Speed increase
#locate      VDEC    =     0x0071      // Speed decrease
#locate       DCC     =     0x0072      // Deviation counter clear output
#locate      TMSTA   =     0x0073      // Timer-start
#locate      TMSTP   =     0x0074      // Timer-stop
#locate       SPSTA   =     0x0075      // Split pulse start
#locate       SPSTP   =     0x0076      // Split pulse stop
#locate       DHOLD   =     0x0077      // Drive start holding
#locate       DFREE   =     0x0078      // Drive start holding release
#locate       R2CLR   =     0x0079      // Error / Finishing status clear
#locate       RR3P0   =     0x007A      // RR3 Page0 display
#locate       RR3P1   =     0x007B      // RR3 Page1 display
#locate       MFPC    =     0x007C      // Maximum finish point clear
#locate       NOP     =     0x001F      // NOP
#locate       MRST    =      0x00FF      // Command reset



