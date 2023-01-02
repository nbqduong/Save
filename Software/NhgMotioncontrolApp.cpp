// NhgMoCrlApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "NhgMotionControlLib.h"
#define MAX_STR 255
int PosReader() {
    INT32 ErrCode;
    UINT32 LP, RP, CV;
    static BOOL waitdrv;
    waitdrv = NhgDrvWait();
    while (waitdrv == 1) {
        ErrCode = NhgStatusGetBasDrv(&LP, &RP, &CV);
        if (ErrCode != 0) {
            std::wcout << "ErrCode: " << ErrCode << "\n";
            std::wcout << "Check USB connection" << "\n";
            return ErrCode;
        }
        std::cout << "LP: " << (INT32)LP << " " << "RP: " << (INT32)RP << " " << "CV: " << (INT32)CV << "\n";
        waitdrv = NhgDrvWait();
    }

    return 0;
}

INT32 ErrPromt(INT32 ErrCode) {
    if (ErrCode == 0) return 0;
    std::wcout << "ErrCode: " << ErrCode << "\n";
    std::wcout << "Check USB connection" << "\n";
    return ErrCode;
}

//28/4/2021
//WRITE FUNCTIONS FOR RESET AND SET LP AND RP
int main()
{
    INT32 ErrCode = 0;
    INT16 Mode3 = 0;
    INT32 InitSpd = 10;
    INT32 Vel = 1000;

    INT32 Acc = 2000;
    INT32 Jerk = 1000;
    INT32 Pos = 0;
    INT32 Dec = 0;

    INT32 gInitSpd;
    INT32 gVel;
    INT32 gAcc;
    INT32 gJerk;
    INT32 gPos;
    INT32 gDec;

    wchar_t _Manufacturer[MAX_STR];
    wchar_t _Product[MAX_STR];
    wchar_t _SerialNumber[MAX_STR];

    // First, check error
    ErrCode = NhgIsOpen(_Manufacturer, _Product, _SerialNumber);
    if (ErrCode != 0) {
        std::wcout << "ErrCode: " << ErrCode << "\n";
        std::wcout << "Check USB connection" << "\n";
        return ErrCode;
    }

    // If there is no error, so print system information
    std::wcout << "Manufacturer: " << _Manufacturer << "\n";
    std::wcout << "Product: " << _Product << "\n";
    std::wcout << "SerialNumber: " << _SerialNumber << "\n";
    Mode3 = AUTODEC | DECSYMM | SCURVE | ONEPULONEDIR | POSPULLOGICAL | NOINVENCINPUT | ENCQUADPULSGLEDGE | POSENCLOGICAL | INVENCINPUT | NOINVLMTINPUT | DISSTOPTRI | ONCETIMER;
    ErrCode = NhgMode(0, 0, Mode3);
    if (ErrCode != 0) {
        std::wcout << "ErrCode: " << ErrCode << "\n";
        return 0;
    }
    Nhgtestfunc(1);
    //NhgSpdDec(100);

    ErrCode = NhgIoSetup(PIO0SOUT|PIO1SOUT|PIO6GOUT|PIO7GOUT, MPGJOGMOD);
    ErrCode = NhgIoGOutCtr(PIO6L | PIO7L);
    //ErrCode = NhgLedCtr(LED2L | LED3L | LED4L | LED5L);

//get code to control led
    //std::cin.get();
    /*for (int i = 0; i < 5; i++) {
        ErrCode = NhgLedCtr(LED2H | LED3L | LED4L | LED5L);
        Sleep(10);
        ErrCode = NhgLedCtr(LED2L | LED3H | LED4L | LED5L);
        Sleep(10);
        ErrCode = NhgLedCtr(LED2L | LED3L | LED4H | LED5L);
        Sleep(10);
        ErrCode = NhgLedCtr(LED2L | LED3L | LED4L | LED5H);
        Sleep(10);
    }*/

    /*ErrCode = NhgIoGOutCtr(PIO6H | PIO7H);
    std::wcout << "Servo ON" << "\n";
    std::wcout << "Press Enter to continue" << "\n";
    ErrCode = NhgIoGOutCtr(PIO6H | PIO7L);*/
    //ErrCode = NhgIoGOutCtr(PIO6H | PIO7L);

    static int  RunCmd = 0;
    static INT32 PosDis = 0;
    static int MovFlag = 0;
    static int CmdFlag = 1;
    static BOOL ReadPos, InitPos;
    static INT32 InitLp, InitRp;
	static int axis=0;
    std::wcout << "\n\nRuncmd list:" << "\n" << "0: Zero current position" << "\n";
    std::wcout << "1: Set new position" << "\n" << "2: OFF Servo" << "\n" << "3: ON servo" << "\n";
    std::wcout << "4: Read Position Unstill stop Drive\n" << "5: Read Position\n";
    std::wcout <<"6: Set logic/real Position\n" << "7: Set axis\n" ;
    std::wcout << "8: Test_func\n" << "-1: Exit\n";
    while (CmdFlag == 1) {
//get code for command
        std::wcout << "Input RunCmd: ";
        std::cin >> RunCmd;
        switch (RunCmd) {
        case 0:  std::cout  << "Reset Position" << "\n"; PosDis = 0;
            break;
        case 1:  std::cout  << "Relative move: "; std::cin >> Pos; PosDis += Pos; MovFlag = 1;
            break;
        case 2:  std::wcout << "Servo OFF" << "\n"; ErrCode = NhgIoGOutCtr(PIO6L | PIO7L);
            break;
        case 3:  std::wcout << "Servo ON" << "\n"; ErrCode = NhgIoGOutCtr(PIO6H | PIO7H);
            break;
        case 4:  std::wcout << "Read Position Unstill stop Drive" << "\n"; PosReader();
            break;
        case 5:  std::wcout << "Read Position" << "\n"; ReadPos = 1;
            break;
        case 6: std::wcout << "Set Logic Postion: "; std::cin >> InitLp; std::wcout << "\n" << "Set Real Postion: ";  std::cin >> InitRp; InitPos = 1;
            break;
		case 7: std::wcout << "Set Axis \n0:None \n1:X \n2:Y \n3:Z \n4:U \n5:All \n"; std::cin >> axis;  SetAxis( axis) ;
            break;
        case 8: std::wcout << "Test...";  Test_func1();
            break;
        case -1: std::wcout << "Press Enter to exit" << "\n"; CmdFlag = 0;
            break;
        default: std::wcout << "Command not support" << "\n";
            break;
        }
        RunCmd = -2;
        if (MovFlag == 1) {
            std::wcout << "InitSpd: " << InitSpd << " Vel: " << Vel << " Acc: " << Acc << " Jerk: " << Jerk << " Pos: " << Pos << " Dec: " << Dec << "\n";
            ErrCode = NhgDrvRel(InitSpd, Vel, Acc, Jerk, Pos, Dec);
            Pos = 0;
            std::wcout << "Current Position: " << PosDis << " deg" << "\n";
            MovFlag = 0;
        }

        else if (ReadPos == 1) {
            static UINT32 LP, RP, CV;
            ErrCode = NhgStatusGetBasDrv(&LP,&RP, &CV);
            if (ErrCode != 0) {
                std::wcout << "ErrCode: " << ErrCode << "\n";
                std::wcout << "Check USB connection" << "\n";
                return ErrCode;
            }
            std::cout << "LP: " << (UINT32)LP << " " << "RP: " << (UINT32)RP << " " << "CV: " << (UINT32)CV << "\n";
            ReadPos = 0;
        }

        else if (InitPos == 1) {
            ErrCode = NhgSetLpRp(InitLp, InitRp);
            if (ErrCode != 0) {
                std::wcout << "ErrCode: " << ErrCode << "\n";
                std::wcout << "Check USB connection" << "\n";
                return ErrCode;
            }
            InitPos = 0;
        }
              
        if (ErrCode != 0) {
            std::wcout << "ErrCode: " << ErrCode << "\n";
            std::wcout << "Check USB connection" << "\n";
            return 0;
        }


   }//end while CmdFlag


    std::cin.get();
    
    std::wcout << "Pos: " << Pos << "\n";
    ErrCode = NhgDrvRel(InitSpd, Vel, Acc, Jerk, Pos, Dec);
    if (ErrCode != 0)
        std::wcout << "ErrCode: " << ErrCode << "\n";
    ErrCode = NhgEcho(&gInitSpd, &gVel, &gAcc, &gJerk, &gPos, &gDec);
    std::wcout << "InitSpd: " << gInitSpd << " Vel: " << gVel << " Acc: " << gAcc << " Jerk: " << gJerk << " Pos: " << gPos << " Dec: " << gDec << "\n";
    std::cin.get();
    std::wcout << "Pos: " << -Pos << "\n";
    ErrCode = NhgDrvRel(InitSpd, Vel, Acc, Jerk, -Pos, Dec);
    if (ErrCode != 0)
        std::wcout << "ErrCode: " << ErrCode << "\n";
    

    //SERVO OFF
    /*std::cin.get();
    ErrCode = NhgIoGOutCtr(PIO6L);
    std::wcout << "Servo OFF" << "\n";*/
} //end main

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
