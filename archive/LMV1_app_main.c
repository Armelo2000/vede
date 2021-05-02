#include "LMV1_app_definitions.h"
#include "math.h"
#pragma CODE_SECTION(Eternal_loop, ".TI.ramfunc");

struct Cancomm CANCOMM;
struct Globals GLOBALS;
struct Adc ADC;
struct Regler REGLER;

//uint8_t symbol = 0xB8;
//extern void Fehler_Anzeige_wbox(void);

void main(void)
{
    Init_gpios();
  //GpioDataRegs.GPACLEAR.all = 0x0000003F;   // Databytes, RS und EN low

  Init_system();
  Init_variables();                     // Variablen vorbelegen
  Init_can();
  Setup_adc();
  ssd1803_init();

  EALLOW;

  EINT;                                 // Globale Interrupts an
  ERTM;                                 // Debugmode Interrupt an

  CpuTimer1Regs.TCR.bit.TSS = 0;        // Starte Timer 1
  CanbRegs.CAN_CTL.bit.DAR = 1;         // Disable automatic retransmission, damit sollte die Last am CAN_Controller sinken

  hd44780_clear_screen();               // Display löschen

  hd44780_write_string("L1:    V   A       W",1,1,NO_CR_LF);
  hd44780_write_string("L2:    V   A       W",2,1,NO_CR_LF);
  hd44780_write_string("L3:    V   A       W",3,1,NO_CR_LF);
  hd44780_write_string("       A   ¸:      W",4,1,NO_CR_LF);   // . entspricht Summenzeichen in Display-Schriftart (ALT 0148)

  GLOBALS.lcd_fehler_exist = false;     // Lcd-flag zur Fehleranzeige initialisieren bzgl. Empfang der CAN-Nachrichten
  GLOBALS.Aufruf = false;
  GREEN_LED_ON;

  Eternal_loop();                       // Zur ewigen Schleife wechseln
}

void Eternal_loop(void)
{
while (1)                               // in ewige Schleife übergehen
  {
    if (!CANCOMM.send_delay)            // Zeitverzögerte CAN-Nachricht für Bootloader-Identifikation
    {
      Send_boot_status();
      Send_fwdate();
      CANCOMM.send_delay = 0xFFFF;
    }
   Read_can_b();                        // Nachrichtenauswertung

    if (GLOBALS.lcd_flag) {
        hd44780_timer_isr();
        GLOBALS.lcd_flag = false;
    }
  }
}

interrupt void cpuTimer1ISR(void)        // Timer Interrupt alle 1ms
{
  static uint16_t timerLED = 0;
  static uint16_t timer_s = 0;
  static uint16_t timer_ms = 0;
  //EINT;                                // andere Interrupts zulassen, nicht nötig.


  GLOBALS.lcd_flag = true;               // LCD-Flag


/*Zur Leistungsberechnung*/
  ADC_Results();                         //Analoge werte berechnen
  Leistungsberechnung();                 //Berechnung der Eff._spannung, des Eff._stromes und der mittl._leistung
  Mittelung();                           //Mittelung


/* Zur Fehlerüberprüfung der empfangenen CAN-Nachrichten */
  timer_ms++;
  if (1 == timer_ms)
  {
      if (++timer_ms && GLOBALS.Aufruf == true) timer_ms = 0;
  }
  if (1000 < timer_ms)
  {
      GLOBALS.Aufruf = false;
      hd44780_write_string("E",4,1,NO_CR_LF);
      hd44780_write_string("E",4,2,NO_CR_LF);
      hd44780_write_string("E",4,3,NO_CR_LF);
      GLOBALS.lcd_fehler_exist = true;
  }
  else GLOBALS.lcd_fehler_exist = false;

/* && GLOBALS.lcd_fehler_exist = false */

  if (GLOBALS.timer_250) GLOBALS.timer_250--;
  GLOBALS.timer_250 = 250;
  if (((CANCOMM.send_delay) && (CANCOMM.send_delay)) < 0xFFFF) {
      CANCOMM.send_delay--;
  }

/* LED Ansteuerung */
  timerLED++;
  if (500 == timerLED) {
      timerLED = 0;
      // LED blinken lassen
      GREEN_LED_TOGGLE;
  }
//  timerLED++;
//  if (500 == timerLED) {
//      timerLED = 0;
//      if ( lroundf(GLOBALS.Summenleistung) < 5){
//          GREEN_LED_OFF;
//          RED_LED_TOGGLE;
//      }
//      else if (lroundf(GLOBALS.Summenleistung) > 0){
//          GREEN_LED_ON;
//          RED_LED_ON;
//      }
//      else {
//          RED_LED_OFF;
//      }
//  }
  ServiceDog();                     // Reset Watchdog

/* Bearbeitung Funktion, die sekündlich aufgerufen werden*/
  timer_s++;
  if (1000 == timer_s){
        timer_s = 0;
        Anzeige();                  //Anzeige der Leistungswerte auf dem Display
        Regelung();                 // Aufruf der Funktion zur Regelung der Ladeleistung
//        Send_cylic_messages();      //CAN-Nachrichten senden
    }
}

//
// End of file
//

