#ifndef MCX501_functions
#define MCX501_functions

#define MAX_ACC 536870911
#define ONE_R   16384 //2^14 OR 2^20/64
#define MAX_QUEUE 2048
#define LED_MSK      0b0000000001111000
#define LED_MSK_OFF  0b0000000000000000

unsigned int32 Combine32(unsigned int8 *data8bit);

unsigned int16 Combine16(unsigned int8 *data8bit);

void Div(unsigned int32 in32bit, unsigned int8* out8bit);

int8 TestFnc();
int32 stemp32_1;

//!Duong's edit
void Test_fun_com();
void change_speed_OLD();
void reset_f();
void initial_f();
void SET_CMD_AXIS(INT CMD_AXIS, INT CMD_SET);
void SET_CMD( INT CMD_SET);
void set_direction( int8 dir );
void stop_f();
void change_direction();
void change_speed();
void home_search() ;
void change_speed2();

int8 WaitDrive();
void SetAxisCOM(unsigned int8 *_cmd);
int8 SetModeFnc(unsigned int8 *_cmd);
int8 DriveAbs(unsigned int8 *_cmd);
int8 DriveFnc(unsigned int8 *_cmd);
int8 VelDriveFnc(unsigned int8 *_cmd);
int8 JogDriveFnc(unsigned int8 *_cmd);
int8 DecStpFnc(unsigned int8 *_cmd);
int8 InsStpFnc(unsigned int8 *_cmd);
int8 IoSetupFnc(unsigned int8 *_cmd);
int8 IoCtrFnc(unsigned int8 *_cmd);
int8 LedCtrFnc(unsigned int8 *_cmd);
int8 GetPosFnc();
int8 GetDrvParaFnc();
void SetLpRpFnc(unsigned int8* _cmd);
void SetLpFnc(unsigned int8* _cmd);
void SetRpFnc(unsigned int8* _cmd);
int8 ChangeVelFnc(unsigned int8* _cmd);
void UsbDrvWaitFnc();
int8 TestFnc();
int8 ReadTestFnc();
unsigned int32 Combine32(unsigned int8 *data8bit);
unsigned int16 Combine16(unsigned int8 *data8bit);
void Div(unsigned int32 in32bit, unsigned int8* out8bit);

#endif
