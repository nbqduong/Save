#ifndef MCX501_USB_HID
#define MCX501_USB_HID

#include <24FJ256GB106.h>
#device ICD=2
#use delay(clock=32MHz,crystal=20000000, USB_FULL)

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES CKSFSM                   //Clock Switching is enabled, fail Safe clock monitor is enabled



#define USB_CONFIG_BUS_POWER 500
#define USB_STRINGS_OVERWRITTEN
#define USE_USB_SERIAL_NUMBER
//char USB_STRING_DESC_OFFSET[]={0,4,12};

//!char const USB_STRING_DESC[]={
//!   //string 0 - language
//!      4,  //length of string index
//!      0x03,  //descriptor type (STRING)
//!      0x09,0x04,  //Microsoft Defined for US-English
//!   //string 1 - manufacturer
//!      8,  //length of string index
//!      0x03,  //descriptor type (STRING)
//!      'N',0,
//!      'H',0,
//!      'G',0,
//!   //string 2 - product
//!      60,  //length of string index
//!      0x03,  //descriptor type (STRING)
//!      'S',0,
//!      'I',0,
//!      'N',0,
//!      'G',0,
//!      'L',0,
//!      'E',0,
//!      ' ',0,
//!      'A',0,
//!      'X',0,
//!      'I',0,
//!      'S',0,
//!      ' ',0,
//!      'M',0,
//!      'O',0,
//!      'T',0,
//!      'I',0,
//!      'O',0,
//!      'N',0,
//!      ' ',0,
//!      'C',0,
//!      'O',0,
//!      'N',0,
//!      'T',0,
//!      'R',0,
//!      'O',0,
//!      'L',0,
//!      'L',0,
//!      'E',0,
//!      'R',0
//!};

#define USB_CONFIG_HID_TX_SIZE 63
#define USB_CONFIG_HID_RX_SIZE 63
#include <pic24_usb.h>
#include <usb_desc_hid.h>

#define USB_STRING_ENGLISH 4, USB_DESC_STRING_TYPE, 9, 4
#define USB_STRING(x)   (sizeof(_unicode(x))+2), USB_DESC_STRING_TYPE, _unicode(x)
   
char const USB_STRING_DESC[] = 
{
   //string 0 (must always provide this string)
      USB_STRING_ENGLISH,
   //string 1 - vendor (this is optional, but we specified it's use in the device descriptor)
      USB_STRING("NHG"),
   //string 2 - product (this is optional, but we specified it's use in the device descriptor)
      USB_STRING("SINGLE AXIS MOTION CONTROLLER"),
   //string 3 - serial number (this is optional, but we specified it's use in the device descriptor)
      USB_STRING("12110") //1:motion cotroller slave | 2:USB | 1:MCX501 | 1: one axis | 0:product number 0 (1st product) , (change here for multiple connections).
};

#include <usb.c>
#build(stack=1024)

#endif

