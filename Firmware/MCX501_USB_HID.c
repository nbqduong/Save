#include <MCX501_USB_HID.h>
#include "MCU_MCX_protocol.c"
#include "MCX501_functions.c"
#include "MCU_CPU_protocol.c"

/*FIRMWARE FOR HARDWARE VERSION 2.2 LAST MODIFY 14/04/2021 */
/*Change Serial_num in MCX501_USB_HID.h for multiple devices */

void main()
{
   //for usb interface
  
  int8 send_timer=0;
   //define rx_data, tx_data in MCU_MCX_protocol.h
   usb_init();
   CSN_H;
   WRN_H;
   RDN_H;
   
   MCX501_REG0 = 0x00FF;
   MCX501_REG6 = 0x0000;
   MCX501_REG7 = 0x0000;
   
   output_bit(PIN_E3,0);
   output_bit(PIN_E4,0);      
   output_bit(PIN_E5,0);
   output_bit(PIN_E6,0); 
   
   //init
   MCX501_REG1 = 0x0000;
   MCX501_REG2 = 0x0000;
   //0,1: output: drive state 10 10
   //2,3: input: servo 00 00
   //4,5: input: external jog 00 00
   //6,7: output: for sofware control (servo on, etc.) 01 01
   
   //0101000000001010
   //wP1M = 0x500A;
   
      while(1) //usb_attached()
   {
      //USB interface
      usb_task();
      if (usb_enumerated())
      { 
         if (usb_kbhit(USB_HID_ENDPOINT))
         {
            static int1 usb_stat = 0;
            usb_stat ^= 1;
            output_bit(PIN_E6,usb_stat);
            usb_get_packet(USB_HID_ENDPOINT, rx_data, USB_CONFIG_HID_RX_SIZE);     
            
            //Echo RX data
           // memcpy(tx_data, rx_data, USB_CONFIG_HID_TX_SIZE);
           // usb_put_packet(USB_HID_ENDPOINT, tx_data, USB_CONFIG_HID_TX_SIZE, USB_DTS_TOGGLE);   
            
            command_reciver(rx_data);
            memset(rx_data,0,USB_CONFIG_HID_RX_SIZE);
            if(!send_timer){
               send_timer = 250;
            }
         }//end data avancelable check
         
         send_timer--;
         delay_ms(1);
      } //end USB interface
      
   }//end while loop

}//end main
