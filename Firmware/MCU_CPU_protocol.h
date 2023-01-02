#ifndef mcu_cpu_protocol
#define mcu_cpu_protocol

#define NOTSUPPORTCMD -1;

enum task {
   MainMod = 1,
   DrvRel,        //relative drive
   DrvCRel,       //counter relative drive
   DrvPCont,      //+ continuous pulse drive
   DrvMCont,      //- continuous pulse drive
   DrvAbs,        // Absolute drive
   DecStp,        // Decelerating stop
   InsStp,        // Instant stops
   IoSt,          //Io seting
   IoCtr,         //Io general output control
   LedCtr,        //On board LED control
   GetPos,        //Get Logical Position and Real Position
   GetDrvPara,    //Get Acc, Dec, InitVel, Pos
   SetLpRp,       //Set value logical position and real position for feedback
   SetLp,         //Set value logical position
   SetRp,         //Set value real position
   ChangeVel,     //Override velocity
   VelDrive,      //Velocity drive specialty
   JogDrive,      //Jog drive
   UsbDrvWait,    //Return 1 if Under Driving
   SetAxisTask,
   Test_func_var,
   ReadTest       //test usb read
   
};

#endif
