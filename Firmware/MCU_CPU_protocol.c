#include "MCU_CPU_protocol.h"
//function holder for later use
int FncHlder(unsigned int8* _cmd){
return -1;
}

int16 command_reciver(unsigned int8* _cmd){
//////////////////11/11/2021 undercontruction
            //memcpy(tx_data, rx_data, USB_CONFIG_HID_TX_SIZE);
            //usb_put_packet(USB_HID_ENDPOINT, tx_data, USB_CONFIG_HID_TX_SIZE, USB_DTS_TOGGLE);
unsigned int16 user_cmd;
user_cmd = _cmd[0];
user_cmd = (user_cmd<<8) ^ _cmd[1];

switch(user_cmd){
   case MainMod      : SetModeFnc(_cmd)      ; break;
   case DrvRel       : DriveFnc(_cmd)        ; break;
   case DrvCRel      : FncHlder(_cmd)        ; break;
   case DrvPCont     : FncHlder(_cmd)        ; break;     
   case DrvMCont     : FncHlder(_cmd)        ; break; 
   case DrvAbs       : DriveAbs(_cmd)        ; break;
   case IoSt         : IoSetupFnc(_cmd)      ; break;
   case IoCtr        : IoCtrFnc(_cmd)        ; break;
   case DecStp       : DecStpFnc(_cmd)       ; break; 
   case InsStp       : InsStpFnc(_cmd)       ; break;
   case LedCtr       : LedCtrFnc(_cmd)       ; break;
   case GetPos       : GetPosFnc()           ; break;
   case GetDrvPara   : GetDrvParaFnc()       ; break;
   case SetLpRp      : SetLpRpFnc(_cmd)      ; break;
   case SetLp        : SetLpFnc(_cmd)        ; break;
   case SetRp        : SetRpFnc(_cmd)        ; break;   
   case ChangeVel    : ChangeVelFnc(_cmd)    ; break; 
   case VelDrive     : VelDriveFnc(_cmd)     ; break;
   case JogDrive     : JogDriveFnc(_cmd)     ; break;
   case UsbDrvWait   : UsbDrvWaitFnc()       ; break;
   case SetAxisTask  : SetAxisCOM(_cmd)      ; break;
   case Test_func_var: Test_fun_com()        ; break;
   default           : return NOTSUPPORTCMD;
   }                  
}



