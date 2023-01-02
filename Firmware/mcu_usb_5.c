#include "MCU_USB_5.h"
//function holder for later use
int FncHlder(unsigned int8* _cmd){
return -1;
}


//!typedef struct{
//!   unsigned int8 data[USB_CONFIG_HID_RX_SIZE];
//!} com_buf;

//!com_buf tx_buf[32];
//!com_buf rx_buf[32];


//muc 4.9 cho cho toi khi drive het hoat dong, sau do moi nhan chi thi tiep theo
int8 WaitDrive(){
   int16 drive_flag = 0;
   drive_flag = REG0;        
   drive_flag &= MSK_DRIVE;
      while(drive_flag){
         drive_flag = REG0;
         drive_flag &= MSK_DRIVE;
      }
      return PASS;
}//end WaitDrive

/*----------------------------Setup mode funtion-----------------------------*/
// Cai nay dung de setup cac thanh ghi Register 1-3 tuy nhien _cmd la gi, toi nghia qua khong hieu
int8 SetModeFnc(unsigned int8 *_cmd){
      if(_cmd[3] != 0){
         return CMD_ERR;
      }
   // dll 64, pic 63, 5-1
      int8 offset = 4;
      int16 temp16 = 0;
   //Reg1
      temp16 = _cmd[offset];
      temp16 = (temp16<<8)^(_cmd[offset+1]);
      REG1 = temp16;
   //Reg2
      offset += 2;
      temp16 = 0;
      temp16 = _cmd[offset];
      temp16 = (temp16<<8)^(_cmd[offset+1]);
      REG2 = temp16;
   //Reg3
      offset += 2;
      temp16 = 0;
      temp16 = _cmd[offset];
      temp16 = (temp16<<8)^(_cmd[offset+1]);
      REG3 = temp16;
      return 0;
}//end SetMode

/*----------------------------------Drive function---------------------------*/
int8 DriveFnc(unsigned int8 *_cmd){
                     
   //!output_bit(PIN_E3,0);
   //!output_bit(PIN_E4,1);
   //!output_bit(PIN_E5,0);
   //!output_bit(PIN_E6,0);
   static int1 e3sta = 0;
   static int1 e4sta = 1;
   e3sta ^= 1;
   output_bit(PIN_E3, e3sta);
   //check drive command input
   if(_cmd[3]<0x50 || _cmd[3]>0x5A){ 
      return CMD_ERR;
   }
    unsigned int32 temp32 = 0;
    unsigned int16 temp16 = 0;
    unsigned int8 offset = 18;
    /*&&&&&&&&&&&&&&&&&&&&*/
    //unsigned int8 dir=0; /*khai bao chieu quay*/
   //set init speed
   temp32 = Combine32(&_cmd[offset]); //18    // 
   
   //dir = _cmd[4]; /*&&&&&&&&&&&&&&&&&&&&*/
   
   
   if(temp32 == 69){
   TestFnc();                  
   return 0;
   }
   
   wSV = temp32;
   //wSV = 10;
      /*Fix from here 13/01/2021*/
   //set speed
   offset += 4;
   temp32 = Combine32(&_cmd[offset]); //22
   wDV    = temp32;
   //wDV = 144000;
   
   //set acceleration   
   //temp32 = 0;
   offset += 4;
   temp32 = Combine32(&_cmd[offset]); //26
   wAC = temp32;
   //wAC = MAX_ACC;
     
   //set jerk
   //temp32 = 0;
   offset += 4;
   temp32 = Combine32(&_cmd[offset]); //30
    wJK = temp32;
    //!wJK = 89300;
        
   //set position
   //temp32 = 0;
   offset += 4;
   temp32 = Combine32(&_cmd[offset]); //34
   wTP = temp32;
    //!wTP = 36000;
       
   //set deceleration
   //temp32 = 0;
   offset += 4;
   temp32 = Combine32(&_cmd[offset]); //38
   wDC = temp32;
   //wDC = 0;
      
   //set command drive   
      temp16 = _cmd[3];
   REG0 = temp16;
    //MCX501_REG0 = 0x0050;
      
   //!   usb_put_packet(USB_HID_ENDPOINT, _cmd, USB_CONFIG_HID_TX_SIZE, USB_DTS_TOGGLE);
      
   //!status3 ^=1;
   //!status4 ^=1;
   e4sta ^= 1;
      output_bit(PIN_E4,e4sta);
   //!WaitDrive();
}// End DriveFnc

int8 VelDriveFnc(unsigned int8 *_cmd){
   unsigned int32 temp32 = 0;
   static int32 stemp32;
   int16 temp16;
   
   //get vel
   temp32 = Combine32(&_cmd[22]); //22
   stemp32 = (int32)temp32;

   //sign check
   if((stemp32 > 0 && stemp32_1 < 0) || (stemp32 < 0 && stemp32_1 > 0)){
      REG0 = 0x57; //instant stop if sign changed.
   }
   wSV = 1;
   wAC = 536870911; //maximum acc
   wDC = 536870911;
   wTP = 1;
   //set dir
   if(stemp32 > 0){
      temp16 = 0x52; //+ Dir
   }
   else if(stemp32 < 0){
      temp16 = 0x53;//- Dir
   }
   else {
      REG0 = 0x57; //instant stop
      return 0;
   }
   //set init speed   
   temp32 = abs(stemp32); //clear sign
   wDV = temp32;
   REG0 = temp16;
   stemp32_1 = stemp32;
}// End VelDriveFnc

int8 JogDriveFnc(unsigned int8 *_cmd){
   unsigned int32 temp32 = 0;
   static int32 stemp32;
   int16 temp16;
//!   static int1 state = 0;
   temp32 = Combine32(&_cmd[26]); //26
   wAC = temp32;
   temp32 = Combine32(&_cmd[38]); //38
   wDC = temp32;
   //get vel
   temp32 = Combine32(&_cmd[22]);
   stemp32 = (int32)temp32;
   wSV = 1;
   wTP = 1;
   //change sign
   //(y > 0 && old_y < 0) || (y < 0 && old_y > 0)
   //Change sign check.
   if((stemp32 > 0 && stemp32_1 < 0) || (stemp32 < 0 && stemp32_1 > 0)){
//!      ledstate ^= 1;
//!      output_bit(PIN_E5,ledstate);
      REG0 = 0x57;
   }
   

   //set dir
   if(stemp32 > 0){
      temp16 = 0x52; //+ Dir
   }
   else if(stemp32 < 0){
      temp16 = 0x53;//- Dir
   }
   else {
      REG0 = 0x57; //instant stop
      return 0;
   }
   //set init speed   
   temp32 = abs(stemp32); //clear sign
   wDV = temp32;
   REG0 = temp16;
   stemp32_1 = stemp32;
}// End JogDriveFnc

//stop with decelaration
int8 DecStpFnc(unsigned int8 *_cmd){
   if(_cmd[3] != 0x56){
      return CMD_ERR;
   }
    unsigned int16 temp16;
    temp16 = _cmd[3];
    REG0 = temp16;
}

//instant stop
int8 InsStpFnc(unsigned int8 *_cmd){
   if(_cmd[3] != 0x57){
      return CMD_ERR;
   }
    unsigned int16 temp16;
    temp16 = _cmd[3];
    REG0 = temp16;
}

/*---------------------------IO setup Funciton-------------------------------*/
int8 IoSetupFnc(unsigned int8 *_cmd){
static int1 ledstate = 0;
ledstate ^= 1;
output_bit(PIN_E3,ledstate);
//!output_bit(PIN_E6,0);
   if(_cmd[3]<0x41 || _cmd[3]>0x42)
      return CMD_ERR;
   
   int16 temp16 = 0;
   temp16 = _cmd[18];
   temp16 = (temp16<<8)^_cmd[19];
   wP1M = temp16;
   
   temp16 = 0;
   temp16 = _cmd[20];
   temp16 = (temp16<<8)^_cmd[21];
   wP2M = temp16;
   
   return 0;
}

int8 IoCtrFnc(unsigned int8 *_cmd){
static int1 ledstate = 0;
ledstate ^= 1;
output_bit(PIN_E4,ledstate);
//!output_bit(PIN_E6,0);
   if(_cmd[3] != 0)
      return CMD_ERR;

   int16 temp16 = 0;
   temp16 = _cmd[11];
   
   REG4 = temp16;
   return 0;
}

int8 LedCtrFnc(unsigned int8 *_cmd){
if(_cmd[3] != 0){
return CMD_ERR;
}
   int16 temp16 = 0;
   temp16 = _cmd[35];
   temp16 = (temp16 << 8) ^ _cmd[36];
   output_e(LED_MSK & temp16);  
   return 0;
}

int8 GetPosFnc(){
   unsigned int32 LP, RP, CV;
   unsigned int8 tx_data[USB_CONFIG_HID_TX_SIZE];
   int a=0;
   //unsigned int16 reg6, reg7;
//!   unsigned int8 rx_data[USB_CONFIG_HID_RX_SIZE];
   memset(tx_data,0,USB_CONFIG_HID_TX_SIZE);
   LP = rLP;
   RP = rRP;
   CV = rCV;
   //LP=100;
   //RP=100;
   //CV=300;
   output_e(LED_MSK_OFF);
   if(LP == 536879204 && RP == 536879204) 
   {
      output_e(LED_MSK);
      Div(LP, &tx_data[0]);
      Div(RP, &tx_data[4]);
      Div(CV, &tx_data[8]);
   a=usb_put_packet(USB_HID_ENDPOINT, tx_data, USB_CONFIG_HID_TX_SIZE, USB_DTS_TOGGLE);
   }
   else if (RP==96) {output_e(LED_MSK_OFF);}
   else{
//!   MCX501_REG0 = 0x30;
//!   reg6 = MCX501_REG6;
//!   reg7 = MCX501_REG7;
//!   rx_data[0]   = 0xFF & (reg7>>8);
//!   rx_data[1]   = 0xFF & (reg7);
//!   rx_data[2]   = 0xFF & (reg6>>8); 
//!   rx_data[3]   = 0xFF & (reg6);    
//!   
//!   MCX501_REG0 = 0x31;
//!   reg6 = MCX501_REG6;
//!   reg7 = MCX501_REG7;
//!   rx_data[4]   = 0xFF & (reg7>>8);
//!   rx_data[5]   = 0xFF & (reg7);
//!   rx_data[6]   = 0xFF & (reg6>>8); 
//!   rx_data[7]   = 0xFF & (reg6);   
//!   
//!   MCX501_REG0 = 0x32;
//!   reg6 = MCX501_REG6;
//!   reg7 = MCX501_REG7;
//!   rx_data[8]   = 0xFF & (reg7>>8);
//!   rx_data[9]   = 0xFF & (reg7);   
//!   rx_data[10]  = 0xFF & (reg6>>8);
//!   rx_data[11]  = 0xFF & (reg6);   
   //! SUA 20/01/20222
   Div(LP, &tx_data[0]);
   Div(RP, &tx_data[4]);
   Div(CV, &tx_data[8]);
   a=usb_put_packet(USB_HID_ENDPOINT, tx_data, USB_CONFIG_HID_TX_SIZE, USB_DTS_TOGGLE);
      if(a==0) // kiem tra xem gui duoc khong
      {
         output_e(LED_MSK);
      }
      else 
         output_e(LED_MSK_OFF);
}   
   return 0;
}

int8 GetDrvParaFnc(){
   unsigned int32 AC, DV, TP;
//!   unsigned int16 reg6, reg7;
//!   unsigned int8 rx_data[USB_CONFIG_HID_RX_SIZE];
   memset(tx_data,0,USB_CONFIG_HID_RX_SIZE);
   AC = rAC;
   DV = rDV;
   TP = rTP;
   Div(AC, &rx_data[0]);
   Div(DV, &rx_data[4]);   
   Div(TP, &rx_data[8]); 
   usb_put_packet(USB_HID_ENDPOINT, tx_data, USB_CONFIG_HID_TX_SIZE, USB_DTS_TOGGLE);
   return 0;
}

void SetLpRpFnc(unsigned int8* _cmd){
   //unsigned int32 temp32;
   unsigned int32 _LP = 0;
   unsigned int32 _RP = 0;
   _LP =Combine32(&_cmd[4]);
   wLP = _LP;
   _RP =Combine32(&_cmd[8]);
   wRP = _RP;
   if(_LP == 100 && _RP == 100)
   {
      output_e(LED_MSK);
   }
   else output_e(LED_MSK_OFF);
}

void SetLpFnc(unsigned int8* _cmd){
   unsigned int32 temp32;
   temp32 = Combine32(&_cmd[4]);
   wLP = temp32;
}

void SetRpFnc(unsigned int8* _cmd){
   unsigned int32 temp32;
   temp32 = Combine32(&_cmd[8]);
   wRP = temp32;
}

int8 ChangeVelFnc(unsigned int8* _cmd){

   if(_cmd[3]<0x70 || _cmd[3]>0x71){
      return CMD_ERR;
   }
   const unsigned int32 MaxInc = 1000000;
   const unsigned int32 MinInc = 1;
   unsigned int32 temp32 = 0;
   unsigned int16 temp16 = 0;
   

   temp32 = Combine32(&_cmd[4]);
   if(temp32 > MaxInc) temp32 = MaxInc;
   else if(temp32 == 0) temp32 = MinInc;
   
   wIV = temp32;
      
   //set command drive   
   temp16 = _cmd[3];
   REG0 = temp16;
   return 0;
}

void UsbDrvWaitFnc(){
   unsigned int8 rx_data[USB_CONFIG_HID_RX_SIZE];
   //memset(rx_data,0,USB_CONFIG_HID_RX_SIZE);
   int16 drive_flag = 0;
   
   drive_flag = MCX501_REG0;
   drive_flag &= MSK_DRIVE;
   rx_data[1] = drive_flag;
   usb_put_packet(USB_HID_ENDPOINT, rx_data, USB_CONFIG_HID_TX_SIZE, USB_DTS_TOGGLE);
 
}

int8 TestFnc(){
   wP1M = 0x5000; // 0b0101 0000 0000 0000 PIO6,7 output
   wP2M = 0x0000;
//!   MCX501_REG4 = 0x0000; //0000 0000 11000000 io6,7 output low

//!   MCX501_REG4 = 0x0040; //0000 0000 01000000 io7 low, io6 high
//!   //acc s-curve mode, 1 pulse / 1 dir
//!   MCX501_REG3 = 0x000C;
   //acc linear mode, Independent pulse 
//!   MCX501_REG3 = 0x0000;
   //acc s-curve mode, Independent pulse 
//!   MCX501_REG3 = 0x0004;

   for(int i = 1; i>0; i--)
   {
      output_bit(PIN_E3,0);
      //parameter
      wSV = 10;
      wDV = 144000;
      wAC = MAX_ACC;
      wJK = 89300;
      wTP = 72000;
      wLP = 0;
   
      //relative drive
      MCX501_REG0 = 0x0050;
      waitdrive();
      output_bit(PIN_E3,1);
      delay_ms(100);
      output_bit(PIN_E4,0);
      //new parameter
      wSV = 10;
      wDV = 144000;
      wAC = MAX_ACC;
      wJK = 89300;
      wTP = -72000;
      wLP = 0;
      //relative drive
      REG0 = 0x0050;
      waitdrive();
      output_bit(PIN_E4,1);
      delay_ms(100);
   }// end for loop
   return 0;
}

int8 ReadTestFnc()
{
   unsigned int8 tx_data[USB_CONFIG_HID_RX_SIZE];
   memset(tx_data,0,USB_CONFIG_HID_RX_SIZE);
   int1 a=0;
   unsigned int32 LP,RP,CV;
   LP=100;
   RP=100;
   CV=200;
   output_e(LED_MSK_OFF);
   Div(LP, &tx_data[0]);
   Div(RP, &tx_data[4]);
   Div(CV, &tx_data[8]);
   a=usb_put_packet(USB_HID_ENDPOINT, tx_data, USB_CONFIG_HID_TX_SIZE, USB_DTS_TOGGLE);
   if(a==0)
   {
      output_e(LED_MSK);
   }
   else 
   {
      output_e(LED_MSK_OFF);
   } 
   return 0;
}

unsigned int32 Combine32(unsigned int8 *data8bit){
  unsigned int32 temp32;
temp32 = data8bit[0];              
temp32 = (temp32<<8)|data8bit[1];
temp32 = (temp32<<8)|data8bit[2];
temp32 = (temp32<<8)|data8bit[3];
   return temp32;
}

unsigned int16 Combine16(unsigned int8 *data8bit){
  unsigned int16 temp16;
temp16 = data8bit[0];              
temp16 = (temp16<<8)|data8bit[1];
   return temp16;
}

void Div(unsigned int32 in32bit, unsigned int8* out8bit){
   out8bit[0] = 0xFF & (in32bit>>24);
   out8bit[1] = 0xFF & (in32bit>>16);
   out8bit[2] = 0xFF & (in32bit>>8); 
   out8bit[3] = 0xFF & (in32bit);       
}
