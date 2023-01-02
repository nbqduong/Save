#ifndef MCU_MCX_protocol
#define MCU_MCX_protocol


///////////////////////////////////////////////////////////////////////////////
//DEFINE PINS
/////////////////////////////////////////////////////////////////////////////// 


#define PASS          0
#define CMD_ERR      -1
#define ERR2         -2

#define SDA       PIN_D9
#define CSN       PIN_D10
#define WRN       PIN_D7
#define RDN       PIN_D6
#define INTN      PIN_D0
#define INTN1     PIN_D1


#define A0        PIN_E0
#define A1        PIN_E1
#define A2        PIN_E2

///////////////////////////////// 
// DEFINE PINS FUNCTIONS
///////////////////////////////// 
#define WRN_L     output_bit(WRN,0);
#define WRN_H     output_bit(WRN,1);
#define RDN_L     output_bit(RDN,0);
#define RDN_H     output_bit(RDN,1);
#define CSN_L     output_bit(CSN,0);
#define CSN_H     output_bit(CSN,1);


///////////////////////////////////////////////////////////////////////////////
//MCX514
/////////////////////////////////////////////////////////////////////////////// 
#ifndef NULL
#define NULL    ((void *)0)
#endif

////////////////////////////////////////////////////////////////////////////////
// Command code definition
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////
// Commands for writing data
//////////////////////////////
#define      MCX514_CMD00_JK        0x0000      // Jerk setting
#define      MCX514_CMD01_DJ        0x0001      // Deceleration increasing rate setting
#define      MCX514_CMD02_AC        0x0002      // Acceleration setting
#define      MCX514_CMD03_DC        0x0003      // Deceleration setting
#define      MCX514_CMD04_SV        0x0004      // Initial speed setting
#define      MCX514_CMD05_DV        0x0005      // Drive speed setting
#define      MCX514_CMD06_TP        0x0006      // Drive pulse number / Finish point setting
#define      MCX514_CMD07_DP        0x0007      // Manual deceleration point setting
#define      MCX514_CMD08_CP        0x0008      // Circular center point setting
#define      MCX514_CMD09_LP        0x0009      // Logical position counter setting
#define      MCX514_CMD0A_RP        0x000A      // Real position counter setting
#define      MCX514_CMD0B_SP        0x000B      // Software limit + setting
#define      MCX514_CMD0C_SM        0x000C      // Software limit ? setting
#define      MCX514_CMD0D_AO        0x000D      // Acceleration counter offsetting
#define      MCX514_CMD0E_LX        0x000E      // Logical position counter maximum value setting
#define      MCX514_CMD0F_RX        0x000F      // Real position counter maximum value setting
#define      MCX514_CMD10_MR0       0x0010      // Multi-purpose register 0 setting
#define      MCX514_CMD11_MR1       0x0011      // Multi-purpose register 1 setting
#define      MCX514_CMD12_MR2       0x0012      // Multi-purpose register 2 setting
#define      MCX514_CMD13_MR3       0x0013      // Multi-purpose register 3 setting
#define      MCX514_CMD14_HV        0x0014      // Home search speed setting
#define      MCX514_CMD15_IV        0x0015      // Speed increasing / decreasing value setting
#define      MCX514_CMD16_TM        0x0016      // Timer value setting
#define      MCX514_CMD17_SP1       0x0017      // Split pulse setting 1
#define      MCX514_CMD18_SP2       0x0018      // Split pulse setting 2
#define      MCX514_CMD19_TX        0x0019      // Interpolation / Finish point maximum value setting 
#define      MCX514_CMD1A_HLN       0x001A      // Helical rotation number setting 
#define      MCX514_CMD1B_HLV       0x001B       // Helical calculation value setting

//////////////////////////////
// Commands for writing mode 
//////////////////////////////
#define      MCX514_CMD20_MRM      0x0020      // Multi-purpose register mode setting
#define      MCX514_CMD21_P1M      0x0021      // PIO signal setting 1
#define      MCX514_CMD22_P2M      0x0022      // PIO signal setting 2?Other settings
#define      MCX514_CMD23_H1M      0x0023      // Automatic home search mode setting 1
#define      MCX514_CMD24_H2M      0x0024      // Automatic home search mode setting 2
#define      MCX514_CMD25_FLM      0x0025      // Input signal filter mode setting
#define      MCX514_CMD26_S0M      0x0026      // Synchronous action SYNC0 setting
#define      MCX514_CMD27_S1M      0x0027      // Synchronous action SYNC1 setting
#define      MCX514_CMD28_S2M      0x0028      // Synchronous action SYNC2 setting
#define      MCX514_CMD29_S3M      0x0029      // Synchronous action SYNC3 setting
#define      MCX514_CMD2A_IPM      0x002A      // Interpolation mode setting

//////////////////////////////
// Commands for reading data
//////////////////////////////
#define      MCX514_CMD30_LP        0x0030      // Logical position counter reading
#define      MCX514_CMD31_RP        0x0031      // Real position counter reading
#define      MCX514_CMD32_CV        0x0032      // Current drive speed reading
#define      MCX514_CMD33_CA        0x0033      // Current acceleration / deceleration reading
#define      MCX514_CMD34_MR0       0x0034      // Multi-purpose register 0 reading
#define      MCX514_CMD35_MR1       0x0035      // Multi-purpose register 1 reading
#define      MCX514_CMD36_MR2       0x0036      // Multi-purpose register 2 reading
#define      MCX514_CMD37_MR3       0x0037      // Multi-purpose register 3 reading
#define      MCX514_CMD38_CT        0x0038      // Current timer value reading
#define      MCX514_CMD39_TX        0x0039      // Interpolation / Finish point maximum value reading 
#define      MCX514_CMD3A_CHLN      0x003A      // Current helical rotation number reading
#define      MCX514_CMD3B_HLV       0x003B      // Helical calculation value reading 
#define      MCX514_CMD3D_WR1       0x003D      // WR1 setting value reading
#define      MCX514_CMD3E_WR2       0x003E      // WR2 setting value reading
#define      MCX514_CMD3F_WR3       0x003F      // WR3 setting value reading
#define      MCX514_CMD40_MRM       0x0040      // Multi-purpose register mode setting reading
#define      MCX514_CMD41_P1M       0x0041      // PIO signal setting 1 reading
#define      MCX514_CMD42_P2M       0x0042      // PIO signal setting 2?Other settings reading
#define      MCX514_CMD43_AC        0x0043      // Acceleration setting value reading
#define      MCX514_CMD44_SV        0x0044      // Initial speed setting value reading
#define      MCX514_CMD45_DV        0x0045      // Drive speed setting value reading
#define      MCX514_CMD46_TP        0x0046      // Drive pulse number/Finish point setting value reading
#define      MCX514_CMD47_SP1       0x0047      // Split pulse setting 1 reading
#define      MCX514_CMD48_UI        0x0048      // General purpose input value reading
 
//////////////////////////////
// Driving commands
//////////////////////////////
#define      MCX514_CMD50_DRVRL        0x0050      // Relative position driving
#define      MCX514_CMD51_DRVNR        0x0051      // Counter relative position driving
#define      MCX514_CMD52_DRVVP        0x0052      // + Direction continuous pulse driving
#define      MCX514_CMD53_DRVVM        0x0053      // ? Direction continuous pulse driving
#define      MCX514_CMD54_DRVAB        0x0054      // Absolute position driving
#define      MCX514_CMD56_DRVSBRK      0x0056      // Decelerating stop
#define      MCX514_CMD57_DRVFBRK      0x0057      // Instant stop
#define      MCX514_CMD58_DIRCP        0x0058      // Direction signal + setting
#define      MCX514_CMD59_DIRCM        0x0059      // Direction signal ? setting
#define      MCX514_CMD5A_HMSRC        0x005A      // Automatic home search execution

//////////////////////////////
// Interpolation commands
//////////////////////////////
#define      MCX514_CMD60_LHK1         0x0060      // 1-axis linear interpolation driving (multichip)
#define      MCX514_CMD61_LHK2         0x0061      // 2-axis linear interpolation driving
#define      MCX514_CMD62_LHK3         0x0062      // 3-axis linear interpolation driving
#define      MCX514_CMD63_LHK4         0x0063      // 4-axis linear interpolation driving
#define      MCX514_CMD64_CHKCW        0x0064      // CW circular interpolation driving
#define      MCX514_CMD65_CHKCCW       0x0065      // CCW circular interpolation driving
#define      MCX514_CMD66_BHK2         0x0066      // 2-axis bit pattern interpolation driving
#define      MCX514_CMD67_BHK3         0x0067      // 3-axis bit pattern interpolation driving
#define      MCX514_CMD68_BHK4         0x0068      // 4-axis bit pattern interpolation driving
#define      MCX514_CMD69_HLCW         0x0069      // CW helical interpolation driving
#define      MCX514_CMD6A_HLCCW        0x006A      // CCW helical interpolation driving
#define      MCX514_CMD6B_HLPCW        0x006B      // CW helical calculation
#define      MCX514_CMD6C_HLPCCW       0x006C      // CCW helical calculation
#define      MCX514_CMD6D_DECEN        0x006D      // Deceleration enabling
#define      MCX514_CMD6E_DECDIS       0x006E      // Deceleration disabling
#define      MCX514_CMD6F_CLRSTEP      0x006F      // Interpolation interrupt clear / Single-step interpolation 

//////////////////////////////
// Synchronous action operation commands
//////////////////////////////
#define      MCX514_CMD81_SYNC0EN      0x0081      // Synchronous action SYNC0 enable setting
#define      MCX514_CMD82_SYNC1EN      0x0082      // Synchronous action SYNC1 enable setting
#define      MCX514_CMD84_SYNC2EN      0x0084      // Synchronous action SYNC2 enable setting
#define      MCX514_CMD88_SYNC3EN      0x0088      // Synchronous action SYNC3 enable setting
#define      MCX514_CMD91_SYNC0DIS     0x0091      // Synchronous action SYNC0 disable setting
#define      MCX514_CMD92_SYNC1DIS     0x0092      // Synchronous action SYNC1 disable setting
#define      MCX514_CMD94_SYNC2DIS     0x0094      // Synchronous action SYNC2 disable setting
#define      MCX514_CMD98_SYNC3DIS     0x0098      // Synchronous action SYNC3 disable setting
#define      MCX514_CMDA1_SYNC0ACT     0x00A1      // Synchronous action SYNC0 activation
#define      MCX514_CMDA2_SYNC1ACT     0x00A2      // Synchronous action SYNC1 activation
#define      MCX514_CMDA4_SYNC2ACT     0x00A4      // Synchronous action SYNC2 activation
#define      MCX514_CMDA8_SYNC3ACT     0x00A8      // Synchronous action SYNC3 activation


//////////////////////////////
// Other Commands
//////////////////////////////
#define      MCX514_CMD70_VINC         0x0070      // Speed increase
#define      MCX514_CMD71_VDEC         0x0071      // Speed decrease
#define      MCX514_CMD72_DCC          0x0072      // Deviation counter clear output
#define      MCX514_CMD73_TMSTA        0x0073      // Timer-start
#define      MCX514_CMD74_TMSTP        0x0074      // Timer-stop
#define      MCX514_CMD75_SPSTA        0x0075      // Split pulse start
#define      MCX514_CMD76_SPSTP        0x0076      // Split pulse stop
#define      MCX514_CMD77_DHOLD        0x0077      // Drive start holding
#define      MCX514_CMD78_DFREE        0x0078      // Drive start holding release
#define      MCX514_CMD79_R2CLR        0x0079      // Error / Finishing status clear
#define      MCX514_CMD7A_RR3P0        0x007A      // RR3 Page0 display
#define      MCX514_CMD7B_RR3P1        0x007B      // RR3 Page1 display
#define      MCX514_CMD7C_MFPC         0x007C      // Maximum finish point clear
#define      MCX514_CMD1F_NOP          0x001F      // NOP
#define      MCX514_CMDFF_RST          0x00FF      // Command reset

////////////////////////////////////////////////////////////////////////////////
// Axis definition as in 6.4 
////////////////////////////////////////////////////////////////////////////////
#define      AXIS_X       0X1            // X axis
#define      AXIS_Y       0X2            // Y axis
#define      AXIS_Z       0X4            // Z axis
#define      AXIS_U       0X8            // U axis
#define      AXIS_ALL     0XF            // All axes
#define      AXIS_NONE    0X00            // No axis

////////////////////////////////////////////////////////////////////////////////
// Address definition
////////////////////////////////////////////////////////////////////////////////
#define      REG_ADDR      0x0000000            // Basic address
//RR0: MDIRCM 0ain status bit mask
#define MSK_DRIVE    0x0001
#define MSK_ERROR    0x0002
#define MSK_ASND     0x0004
#define MSK_CNST     0x0008
#define MSK_DSND     0x0010
#define MSK_AASND    0x0020
#define MSK_ACNST    0x0040
#define MSK_ADSND    0x0080
#define MSK_SYNC0    0x0100
#define MSK_SYNC1    0x0200
#define MSK_SYNC2    0x0400
#define MSK_SYNC3    0x0800
#define MSK_TIMER    0x1000
#define MSK_SPLIT    0x2000

unsigned int8 tx_data[USB_CONFIG_HID_TX_SIZE];
unsigned int8 rx_data[USB_CONFIG_HID_RX_SIZE];


#endif
