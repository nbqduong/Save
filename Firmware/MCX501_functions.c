#include "MCX501_functions.h"
#include <math.h>

typedef struct{
   unsigned int8 data[USB_CONFIG_HID_RX_SIZE];
} com_buf;
//!
com_buf tx_buf[32];
com_buf rx_buf[32];


//!---------------Axis and precommand zone------------------
void SET_CMD_AXIS(INT CMD_AXIS, INT CMD_SET)
{
   CMD_SET = ((CMD_AXIS << 8)|CMD_SET);
   REG0 = (unsigned int16)CMD_SET;  
}

void SET_CMD( INT CMD_SET)
{
   CMD_SET = ((THIS_AXIS << 8)|CMD_SET);
   REG0 = (unsigned int16)CMD_SET;  
}

void SetAxisCOM(unsigned int8 *_cmd){
   switch (_cmd[9])
   {
      case 0: SET_AXIS( AXIS_NONE); break;
      case 1: SET_AXIS( AXIS_X); break;
      case 2: SET_AXIS( AXIS_Y); break;
      case 3: SET_AXIS( AXIS_Z); break;
      case 4: SET_AXIS( AXIS_U); break;
      case 5: SET_AXIS( AXIS_ALL); break;
   }
}

INT8 CHECK_AXIS()
{
   if(THIS_AXIS==0) return 0;
   if(THIS_AXIS==AXIS_ALL) RETURN 2;
   if(THIS_AXIS==AXIS_X || THIS_AXIS==AXIS_Y || THIS_AXIS==AXIS_Z || THIS_AXIS==AXIS_U) return 1;
   return -1;
}

//!----------------------Functions zone--------------------

void Test_fun_com()
{
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
//!   return 0;
}

void reset_f()
{
   CSN_H;
   WRN_H;
   RDN_H;
   
   REG0=0x00ff; //reset MCX514
   
   wRP=0;
}

void initial_f()
{
   INT32 int_sp, drv_sp, pulse;
    int_sp=100;
    drv_sp=20000;
    
    wSV=int_sp; //SET START SPEED
    wDV=drv_sp; // Drive speed 2Kpps
    wAC=536870911; // Acceleration (maximum in specification)
    wJK= (4*(drv_sp-int_sp)/pow(5,2)); // Jerk 49750pps/sec2 
}

void set_direction( int8 dir )
{
   switch (dir)
   {
      case 1:  
         SET_CMD(0X52); // +pulse driving
         BREAK;
      
      
      case -1: 
         SET_CMD(0X53); // -pulse driving
         BREAK;
   }
}

void stop_f()
{
   SET_CMD(0X57); //STOP
}


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
int8 SetModeFnc(unsigned int8 *_cmd){
   if(_cmd[3] != 0){
      return CMD_ERR;
   }
   if(CHECK_AXIS()==0)
   {
      RETURN -1;
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
int8 DriveAbs(unsigned int8 *_cmd){
   if(_cmd[3]<0x54 || _cmd[3]>0x55)
   {
      return CMD_ERR;
   }
   else if (_cmd[3] == 0x54)
   {
      output_e(LED_MSK);
   }
   unsigned int32 temp32 = 0;
   unsigned int16 temp16;
   unsigned int8 offset = 18;
   //set init speed
   temp32 = Combine32(&_cmd[offset]); //18
   wSV = temp32;
   //wSV = 10;
      /*Fix from here 13/01/2021*/
   //set speed
   temp32 = 0;
   offset += 4;
   temp32 = Combine32(&_cmd[offset]); //22
   wDV    = temp32;
   //wDV = 144000;
   
   //set acceleration   
   temp32 = 0;
   offset += 4;
   temp32 = Combine32(&_cmd[offset]); //26
   wAC = temp32;
   //wAC = MAX_ACC;
     
   //set jerk
   temp32 = 0;
   offset += 4;
   temp32 = Combine32(&_cmd[offset]); //30
    wJK = temp32;
    //!wJK = 89300;
        
   //set position
   temp32 = 0;
   offset += 4;
   temp32 = Combine32(&_cmd[offset]); //34
   wTP = temp32;
    //!wTP = 36000;
       
   //set deceleration
   temp32 = 0;
   offset += 4;
   temp32 = Combine32(&_cmd[offset]); //38
   wDC = temp32;
   //Mode: Absolute Drive
   temp16 = _cmd[3];
//!   REG0 = temp16;
   SET_CMD(temp16);
}

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
   if(_cmd[3]<0x50 || _cmd[3]>0x5A)
   {
      return CMD_ERR;
   }
 unsigned int32 temp32 = 0;
 unsigned int16 temp16 = 0;
 unsigned int8 offset = 18;
 /*&&&&&&&&&&&&&&&&&&&&*/
 //unsigned int8 dir=0; /*khai bao chieu quay*/
//set init speed
   temp32 = Combine32(&_cmd[offset]); //18

//dir = _cmd[4]; /*&&&&&&&&&&&&&&&&&&&&*/


   if(temp32 == 69)
   {
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
   SET_CMD(temp16);
//!   REG0 = temp16;
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
//!      REG0 = 0x57; //instant stop if sign changed.
         SET_CMD(0X57);
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
//!   REG0 = temp16;
   SET_CMD(temp16);
   stemp32_1 = stemp32;
}// End VelDriveFnc

int8 JogDriveFnc(unsigned int8 *_cmd){
   unsigned int32 temp32 = 0;
   static int32 stemp32;
   int16 temp16;
//!   static int1 ledstate = 0;
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
//!      REG0 = 0x57;
         SET_CMD(0X57);
   }
   

   //set dir
   if(stemp32 > 0){
      temp16 = 0x52; //+ Dir
   }
   else if(stemp32 < 0){
      temp16 = 0x53;//- Dir
   }
   else {
//!      REG0 = 0x57; //instant stop
      SET_CMD(0X57);
      return 0;
   }
   //set init speed   
   temp32 = abs(stemp32); //clear sign
   wDV = temp32;
//!   REG0 = temp16;
   SET_CMD(temp16);
   stemp32_1 = stemp32;
}// End JogDriveFnc

//stop with decelaration
int8 DecStpFnc(unsigned int8 *_cmd){
   if(_cmd[3] != 0x56){
      return CMD_ERR;
   }
    unsigned int16 temp16;
    temp16 = _cmd[3];
//!    REG0 = temp16;
    SET_CMD(temp16);
}

//instant stop
int8 InsStpFnc(unsigned int8 *_cmd){
   if(_cmd[3] != 0x57){
      return CMD_ERR;
   }
    unsigned int16 temp16;
    temp16 = _cmd[3];
//!    REG0 = temp16;
   SET_CMD(temp16);
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

//!-----------READING CMD-------------

int8 GetPosFnc(){
   unsigned int32 LP, RP, CV;
   unsigned int8 tx_data[USB_CONFIG_HID_TX_SIZE];
   int a=0;
   
   IF(CHECK_AXIS() != 1) return -1;
   
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
   if(LP == 100 && RP == 100) 
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
   usb_put_packet(USB_HID_ENDPOINT, tx_data, USB_CONFIG_HID_TX_SIZE, USB_DTS_TOGGLE);
   
   }   
   return 0;
}

int8 GetDrvParaFnc(){

   IF(CHECK_AXIS() != 1) return -1;
   
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
//!   REG0 = temp16;
   SET_CMD(temp16);
   return 0;
}

void UsbDrvWaitFnc(){
   unsigned int8 rx_data[USB_CONFIG_HID_RX_SIZE];
   //memset(rx_data,0,USB_CONFIG_HID_RX_SIZE);
   int16 drive_flag = 0;
   
   drive_flag = REG0;
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
      REG0 = 0x0050;
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

//!----------data processing--------

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


void change_direction()
{
   reset_f();
   initial_f();
    
    
   // Relative position driving
   REG3= 0b0010100000001000;
   
   SET_DIRECTION(1); //, +pulse driving
   delay_ms(2000); 
   STOP_F(); //stop
   delay_us(100);
   SET_DIRECTION(-1); //axis Z, -pulse driving
   delay_ms(2000);
   
//!   STOP_F(); //stop
//!   delay_us(10000);
//!   REG0=0b1001010010; //axis Z, +pulse driving
//!   delay_ms(2000);
//!   
//!   REG0=0b1001010111; //stop
//!   delay_us(10000);
//!   REG0=0b1001010011; //axis Z, -pulse driving
//!   delay_ms(2000);
//!   
//!   REG0=0b1001010111; //stop
//!   delay_us(10000);
//!   REG0=0b1001010010; //axis Z, +pulse driving
   
}

void change_speed()
{
    
    INT32 int_sp, drv_sp, pulse;
    int_sp=100;
    drv_sp=5000;
    
    wSV=int_sp; //SET START SPEED
    wDV=drv_sp; // Drive speed 2Kpps
    wAC=536870911; // Acceleration (maximum in specification)
    wJK= (4*(drv_sp-int_sp)/pow(5,2)); // Jerk 49750pps/sec2 
    
    // Relative position driving
   REG3= 0b0010100000001000;
   
   
   
   SET_DIRECTION(1); //, +pulse driving
   
   delay_ms(2000);
   wDV=30000;
   
   delay_ms(2000);
   wDV=50000;

   delay_ms(2000);
   wDV=70000;
   
   delay_ms(2000);
   wDV=50000;

   delay_ms(2000);
   wDV=30000;
}


void home_search() 
{

   RESET_F();
   INT32 int_sp, drv_sp;
    int_sp=100;
//!    drv_sp=10000;
    
    wSV=int_sp; //SET START SPEED
    wDV=drv_sp; // Drive speed 2Kpps
    wAC=536870911; // Acceleration (maximum in specification)
    wJK= (4*(drv_sp-int_sp)/pow(5,2)); // Jerk 49750pps/sec2 
    
    
   // Relative position driving
   REG3= 0b0010100000001000;
   
//!   REG0=0b1001010010; //axis Y, +pulse driving
   
         // WR2 Register setting
      SET_CMD(0X1F);  // Select y axis
      REG2= 0x0800;  // Home signal logical setting: XSTOP1: Low active
      // Enables hardware limit
      // Input signal filter mode setting
      REG6 = 0x0A04;  // D11~D8 1010 Filter delay: 512µsec
      // D2 1 XSTOP1 signal: Enables the filter
      SET_CMD(0X25);  // Writes a command
      // Automatic home search mode setting 1
      REG6 = 0x8037;  
         // D15 1 Step 4 execution/non-execution: Execution
         // D14 0 Step 3 LP clear Disable
         // D13 0 Step 3 RP clear Disable
         // D12 0 Step 3 DCC output: Disable
         // D11 0 Step 3 search direction: -
         // D10 0 Step 3 execution/non-execution: Non-execution
         // D9 0 Step 2 LP clear Disable
         // D8 0 Step 2 RP clear Disable
         // D7 0 Step 2 DCC output: Disable
         // D6 0 Step 2 detection signal: STOP1
         // D5 1 Step 2 search direction: -direction
         // D4 1 Step 2 execution/non-execution : Execution
         // D3,2 0,1 Step 1 detection signal: STOP1
         // D1 1 Step 1 search direction: -direction
         // D0 1 Step 1 execution/non-execution: Execution
      SET_CMD(0X23);; // Writes a command
      // Automatic home search mode setting 2
      REG6= 0X0000;  // D15 0
      // D14 0
      // D13 0
      // D12 0
      // D11 0
      // D10~8 0 Timer value
      // D7 0 Timer between steps Disable
      // D6~4 0 DCC pulse width
      // D3 0 DCC pulse logic
      // D2 0 At the termination of home search, LP clear Disable
      // D1 0 At the termination of home search, RP clear Disable
      // D0 0 Step 2&3 Disable
      SET_CMD(0X24);  // Writes a command
      // High-speed home search and low-speed home search setting
      REG6  =0X7318;  // Acceleration/deceleration:95,000 PPS/SEC
      REG7  =0X0001 ;
      SET_CMD(0X2);
      REG6  =20000 ; // Initial speed:1000 PPS
      REG7  =0X0000 ;
      SET_CMD(0X4);
      REG6  =0X4E20 ; // Speed of step 1 and 4:20000 PPS
      REG7  =0X0000 ;
      SET_CMD(0X5);
      REG6  =500 ; // Speed of step 2:500 PPS
      REG7  =0X0000 ;
      SET_CMD(0X14);
      // Offset pulse setting
      REG6  =100;  // Offset driving pulse count:3500
      REG7  =0X0000 ;
      SET_CMD(0X6);
      // Starts execution of automatic home search
      SET_CMD(0X5A);


   // remember to edit axis REG0
}

void change_speed2()
{
   INT32 int_sp, drv_sp, pulse;
    int_sp=100;
    drv_sp=10000;
    
    wSV=int_sp; //SET START SPEED
    wDV=drv_sp; // Drive speed 2Kpps
    wAC=536870911; // Acceleration (maximum in specification)
    wJK= (4*(drv_sp-int_sp)/pow(5,2)); // Jerk 49750pps/sec2 
    
    wIV=50000; //set increase speed
   
   REG3= 0b0010100000001000; // Relative position driving
   SET_CMD(0X52); //axis Y, +pulse driving
   
   delay_ms(5000);
   SET_CMD(0X70); //increase velocity
   
   delay_ms(5000);
   SET_CMD(0X70);//increase velocity

   delay_ms(5000);
   SET_CMD(0X71); //decrease velocity

   delay_ms(5000);
   SET_CMD(0X70); //increase velocity
   
   delay_ms(5000);
   SET_CMD(0X71); //decrease velocity
   
   delay_ms(5000);
   SET_CMD(0X70); //increase velocity
}

void change_speed_OLD()
{
    INT32 int_sp, drv_sp, pulse;
    int_sp=100;
    drv_sp=5000;
    
    wSV=int_sp; //SET START SPEED
    wDV=drv_sp; // Drive speed 2Kpps
    wAC=536870911; // Acceleration (maximum in specification)
    wJK= (4*(drv_sp-int_sp)/pow(5,2)); // Jerk 49750pps/sec2 
    
    // Relative position driving
   REG3= 0b0010100000001000;
   
   REG0=0b111101010010; //axis Z, +pulse driving
   
   delay_ms(2000);
   wDV=30000;
   
   delay_ms(2000);
   wDV=50000;

   delay_ms(2000);
   wDV=70000;
   
   delay_ms(2000);
   wDV=50000;

   delay_ms(2000);
   wDV=30000;
}

