/*
 * process_can.h
 *
 *  Created on: 26 avr. 2021
 *      Author: tadon
 */

#ifndef MY_INC_PROCESS_CAN_H_
#define MY_INC_PROCESS_CAN_H_

#include "stdint.h"
// #include <LMV1_app_definitions.h>

#define MAX_WBOX_ANZAHL      16             // Maximale Anzahl der Wbox

typedef enum Plug_In_State
{
  DISCONNECTED,     /* Auto ist nicht angeschlossen */
  CONNECTED         /* Auto ist angeschlossen */
}ePlug_In_State;

typedef enum Charge_State
{
  NOT_FULL,         /* Auto ist nicht voll geladen */
  FULL              /* Auto ist voll geladen */
}eCharge_State;

#ifdef DUMMY

#define NULL	((void *)0)
#define true	1
#define false 0

#define bool unsigned char

typedef struct _data
{
	uint32_t ident;
	uint16_t receive;
	uint8_t data_2_send[8];
}mboxdata;

typedef struct _global
{
	uint8_t Anzahl_FullCharged;
	uint8_t Anzahl_Connected;
	uint8_t Anzahl_zu_ladenden_Autos;
	uint8_t lcd_fehler_exist;
	
}tglobalvar;
#endif

// Struktur für WBox-Objekt
typedef struct WBox
{
    ePlug_In_State eConnectionState;        /* Zustand Auto angesteckt oder nicht */
    eCharge_State  eChargeState;            /* Zustand Auto voll geladen oder nicht */
    uint8_t Index_Wbox;
    bool Vorhanden;
	  char lcdmsg;

}WBox;

void Verarbeitung_CAN_msg(mboxdata *DATA);
void Anzeige_Wbox_Zustand(WBox* Zustand);
void Fehler_Anzeige_wbox(void);
void wbox_init(void);

#endif /* MY_INC_PROCESS_CAN_H_ */
