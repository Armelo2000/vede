/*
 * process_can.c
 *
 *  Created on: 26 avr. 2021
 *      Author: tadon
 */

#include "process_can.h"

//WBox *p_Anzahl_WBox[MAX_WBOX_ANZAHL] = {(WBox *)NULL};
WBox Alle_WBox[MAX_WBOX_ANZAHL];
#ifdef SIMULATION
mboxdata tCanData;
mboxdata * DATA = &tCanData;
#endif
tglobalvar GLOBALS;

void wbox_init(void)
{
	 uint8_t i = 0;
	 for(i=0; i<MAX_WBOX_ANZAHL; i++)
	 {
			Alle_WBox[i].eConnectionState = DISCONNECTED;
			Alle_WBox[i].eChargeState = NOT_FULL;
			Alle_WBox[i].Index_Wbox = 0xFF;
		  Alle_WBox[i].Vorhanden = 0;
		  Alle_WBox[i].lcdmsg = 'D';
	 }
}

/* ToDo :
	Check if the first byte to see if the WBox is connected or not
		- if it's connected 
			-- save the connected box in array if it was not connected.
			-- increment the number of connected WBox if it was disconnected.
			-- set the connection flag to CONNECTED
			Check if the second byte to see if the WBox is fully charged or not
				-- if it's fully charged
					--- increment the counter for full charged if it was not full charged
					--- set the flag to FULL
					--- Stop the power transmission

				-- if it's not fully charged
					--- decrement the counter for full charged if it was full charged
					--- set the flag to NOT_FULL.
					--- Send the power to charge the car (Start the power transmission)
				
		- if it's disconnected 
			-- decrement the number of connected WBox if it was connected.
			-- decrement the number of full charged WBox if it was full.
			-- reset the connection state to DISCONNECTED, rest the charge state to NOT_FULL
			-- delete the object of the list
			-- Stop the power transmission
*/

void Verarbeitung_CAN_msg(mboxdata *DATA)                                   // Verarbeitung neuer Nachricht
{
    if(DATA != NULL)                                                        // if pointer nicht null
    {
        if((DATA->ident >= 0x00000000) && (DATA->ident <= 0x0000000F))      // prüfen, ob die Adresse eine gültige Wbox-Adresse ist
        {
#ifdef SIMULATION
						 uint8_t erstes_Byte = (DATA->receive >> 8) & 0x01;
						 uint8_t zweites_Byte = DATA->receive & 0x01;
#else
             uint8_t erstes_Byte = DATA->DATA_A.byte.BYTE0;
             uint8_t zweites_Byte = DATA->DATA_A.byte.BYTE1;
#endif
            if(erstes_Byte == 0x01)                                         // Prüfen des ersten Bytes, Auto ist angeschlossen?
            {
                //Connected !
                if(Alle_WBox[DATA->ident].eConnectionState == DISCONNECTED)    // Auto ist angeschlossen,
                {
										Alle_WBox[DATA->ident].eConnectionState = CONNECTED;
                    GLOBALS.Anzahl_Connected++;                             // nur inkrementieren, wenn das Auto vorher nicht angeschlossen war
								}
                //Alle_WBox[DATA->ident].lcdmsg = 'C';


                if(zweites_Byte == 0x01)                                    // Prüfen des zweiten Bytes, Auto ist voll geladen?
                {
									// Full !
                  if(Alle_WBox[DATA->ident].eChargeState == NOT_FULL)
                    {
											Alle_WBox[DATA->ident].eChargeState = FULL;
                      GLOBALS.Anzahl_FullCharged++;                         // nur inkrementieren, wenn das Auto vorher nicht voll geladen war
                    } 
										Alle_WBox[DATA->ident].lcdmsg = 'F';

                    // Übertragung an der Adresse "DATA->ident" abbrechen
                    // (Aufruf der Funktion zum Stoppen des Ladevorgangs)???
                    // (Nichts machen, Auto wird nicht mehr laden)
                }
								else
								{
									  // Not Full !
										if (Alle_WBox[DATA->ident].eChargeState == FULL)      // Auto ist nicht voll geladen
										{
											Alle_WBox[DATA->ident].eChargeState = NOT_FULL;
											GLOBALS.Anzahl_FullCharged--;   // nur dekrementieren, wenn es voll geladen war
										}
										Alle_WBox[DATA->ident].lcdmsg = 'L';
								}

            }
            else
            {
                // Kein Auto ist angeschlossen.
                // Flag zurücksetzen und Array freigeben

                if(Alle_WBox[DATA->ident].eConnectionState == CONNECTED)
                {
									 Alle_WBox[DATA->ident].eConnectionState = DISCONNECTED;
                   GLOBALS.Anzahl_Connected--;                 //nur dekrementieren, wenn das Auto angeschlossen war
									 
									 // wenn das auto verbunden und voll war dann die anzahl von volle Auto reduzieren
									 if(Alle_WBox[DATA->ident].eChargeState == FULL)
									 {
									   GLOBALS.Anzahl_FullCharged--;
									 }
                }
								/*
                if(zweites_Byte == 0x01)                                    // Prüfen des zweiten Bytes, Auto ist voll geladen?
                {
									// Full !
                  if(Alle_WBox[DATA->ident].eChargeState == NOT_FULL)
                    {
											Alle_WBox[DATA->ident].eChargeState = FULL;
                      GLOBALS.Anzahl_FullCharged++;                         // nur inkrementieren, wenn das Auto vorher nicht voll geladen war
                    } 

                    // Übertragung an der Adresse "DATA->ident" abbrechen
                    // (Aufruf der Funktion zum Stoppen des Ladevorgangs)???
                    // (Nichts machen, Auto wird nicht mehr laden)
                }
								else
								{
									  // Not Full !
										if (Alle_WBox[DATA->ident].eChargeState == FULL)      // Auto ist nicht voll geladen
										{
											Alle_WBox[DATA->ident].eChargeState = NOT_FULL;
											GLOBALS.Anzahl_FullCharged--;   // nur dekrementieren, wenn es voll geladen war
										}
								}  */
                Alle_WBox[DATA->ident].lcdmsg = 'D';

            }

            Alle_WBox[DATA->ident].Index_Wbox = DATA->ident;   // Wbox_Adresse in Index_Wbox speichern für Zustand Anzeige
            //start_Übertragung_Stromsollwert();                      // Übertragung an der Adresse "DATA->ident" starten
            Anzeige_Wbox_Zustand(&Alle_WBox[DATA->ident]);

           // DINT;

            GLOBALS.Anzahl_zu_ladenden_Autos = GLOBALS.Anzahl_Connected - GLOBALS.Anzahl_FullCharged;

           // EINT;

        }
    }
}

// Funktion zu Fehleranzeige, wenn für die drei ersten Wbox nach einer Sekunde keine Nachricht empfangen worden ist
//void Fehler_Anzeige_wbox(void)
//{
//    if (GLOBALS.ms_counter > 1000 && GLOBALS.lcd_fehler_exist == false)
//    {
//        hd44780_write_string("E",4,1,NO_CR_LF);
//        hd44780_write_string("E",4,2,NO_CR_LF);
//        hd44780_write_string("E",4,3,NO_CR_LF);
//
//        GLOBALS.lcd_fehler_exist = true;
//    }
//
//}

// Funktion zur Zustandsanzeige des Autos für jede Wbox
void Anzeige_Wbox_Zustand(WBox *Zustand)//Alle_WBox[DATA->ident]
{
    if(Zustand != NULL)
    {
        uint8_t lcd_Spalte = (Zustand->Index_Wbox * 2) + 1;       // Index mit der Wbox_adresse in lcd_Spalte speichern
        GLOBALS.lcd_fehler_exist = false;               // Flag zurücksetzten

        if(Zustand->Index_Wbox < 3)                              // Für 3 Wboxen
        {
#ifndef SIMULATION					
					 hd44780_write_string(Zustand->lcdmsg,4,lcd_Spalte,NO_CR_LF); 
#endif
					/*
            if (Zustand->eConnectionState == DISCONNECTED)                  // Auto ist nicht angesteckt?
            {
                //hd44780_write_string("D",4,lcd_Spalte,NO_CR_LF);          // Für die jeweilige Wbox "D" auf dem Display schreiben
            }
            if (Zustand->eConnectionState == CONNECTED)                     // Auto ist angesteckt?
            {
                if (Zustand->eChargeState == FULL)                          // Dann Auto voll geladen?
                {
                    //hd44780_write_string("F",4,lcd_Spalte,NO_CR_LF);      // Für die jeweilige Wbox "F" auf dem Display schreiben
                }
                else if (Zustand->eChargeState == NOT_FULL)                 // Auto nicht voll geladen?
                {
                   // hd44780_write_string("L",4,lcd_Spalte,NO_CR_LF);      // Für die jeweilige Wbox "F" auf dem Display schreiben
                }
            }
					*/

        }
    }
}
