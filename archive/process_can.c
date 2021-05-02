/*
 * process_can.c
 *
 *  Created on: 26 avr. 2021
 *      Author: tadon
 */

#include "process_can.h"

WBox *p_Anzahl_WBox[MAX_WBOX_ANZAHL] = {(WBox *)NULL};
WBox Alle_WBox[MAX_WBOX_ANZAHL];

void Verarbeitung_CAN_msg(mboxdata *DATA)                                   // Verarbeitung neuer Nachricht
{
    if(DATA != NULL)                                                        // if pointer nicht null
    {
        if((DATA->ident >= 0x00000000) && (DATA->ident <= 0x0000000F))      // prüfen, ob die Adresse eine gültige Wbox-Adresse ist
        {
             uint8_t erstes_Byte = DATA->DATA_A.byte.BYTE0;
             uint8_t zweites_Byte = DATA->DATA_A.byte.BYTE1;

            if(erstes_Byte == 0x01)                                         // Prüfen des ersten Bytes, Auto ist angeschlossen?
            {
                if(p_Anzahl_WBox[DATA->ident] == (WBox *)NULL)              //Prüfen, ob es bereits angeschlossen war
                {
                    p_Anzahl_WBox[DATA->ident] = &Alle_WBox[DATA->ident];   // Abspeichern des angeschlossenen Auto
                }

//                p_Anzahl_WBox[DATA->ident]->Index_Wbox = DATA->ident;       // Wbox_Adresse in Index_Wbox speichern für Zustand Anzeige

                if(p_Anzahl_WBox[DATA->ident]->eConnectionState == DISCONNECTED)    // Auto ist angeschlossen,
                {
                    GLOBALS.Anzahl_Connected++;                             // nur inkrementieren, wenn das Auto vorher nicht angeschlossen war
                }
                p_Anzahl_WBox[DATA->ident]->eConnectionState = CONNECTED;


                if(zweites_Byte == 0x01)                                    // Prüfen des zweiten Bytes, Auto ist voll geladen?
                {
                  if(p_Anzahl_WBox[DATA->ident]->eChargeState == NOT_FULL)
                    {
                      GLOBALS.Anzahl_FullCharged++;                         // nur inkrementieren, wenn das Auto vorher nicht voll geladen war
                    }
                    p_Anzahl_WBox[DATA->ident]->eChargeState = FULL;

                    // Übertragung an der Adresse "DATA->ident" abbrechen
                    // (Aufruf der Funktion zum Stoppen des Ladevorgangs)???
                    // (Nichts machen, Auto wird nicht mehr laden)
                }
            else if (p_Anzahl_WBox[DATA->ident]->eChargeState == FULL)      // Auto ist nicht voll geladen
                {
                    GLOBALS.Anzahl_FullCharged--;   // nur dekrementieren, wenn es voll geladen war
                }

//                p_Anzahl_WBox[DATA->ident]->eChargeState = NOT_FULL;

            }
            else
            {
                // Kein Auto ist angeschlossen.
                // Flag zurücksetzen und Array freigeben
                if(p_Anzahl_WBox[DATA->ident] != (WBox *)NULL)
                {
                    if(p_Anzahl_WBox[DATA->ident]->eConnectionState == CONNECTED)
                    {
                        GLOBALS.Anzahl_Connected--;                 //nur dekrementieren, wenn das Auto angeschlossen war
                    }
                    if(p_Anzahl_WBox[DATA->ident]->eChargeState == FULL)
                    {
                        GLOBALS.Anzahl_FullCharged--;               //nur dekrementieren, wenn das Auto voll geladen war
                    }

                  p_Anzahl_WBox[DATA->ident]->eConnectionState = DISCONNECTED;
                  p_Anzahl_WBox[DATA->ident]->eChargeState = NOT_FULL;
                  p_Anzahl_WBox[DATA->ident] = NULL;

                }
            }

            p_Anzahl_WBox[DATA->ident]->Index_Wbox = DATA->ident;   // Wbox_Adresse in Index_Wbox speichern für Zustand Anzeige
            start_Übertragung_Stromsollwert();                      // Übertragung an der Adresse "DATA->ident" starten
            Anzeige_Wbox_Zustand(&Alle_WBox[DATA->ident]);

            DINT;

            GLOBALS.Anzahl_zu_ladenden_Autos = GLOBALS.Anzahl_Connected - GLOBALS.Anzahl_FullCharged;

            EINT;

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
void Anzeige_Wbox_Zustand(WBox *Zustand)//p_Anzahl_WBox[DATA->ident]
{
    if(Zustand != NULL)
    {
        uint8_t lcd_Spalte = Zustand->Index_Wbox + 1;       // Index mit der Wbox_adresse in lcd_Spalte speichern
        GLOBALS.lcd_fehler_exist = false;               // Flag zurücksetzten

        if(lcd_Spalte < 3)                              // Für 3 Wboxen
        {
            if (Zustand->eConnectionState == DISCONNECTED)                  // Auto ist nicht angesteckt?
            {
                hd44780_write_string("D",4,lcd_Spalte,NO_CR_LF);          // Für die jeweilige Wbox "D" auf dem Display schreiben
            }
            if (Zustand->eConnectionState == CONNECTED)                     // Auto ist angesteckt?
            {
                if (Zustand->eChargeState == FULL)                          // Dann Auto voll geladen?
                {
                    hd44780_write_string("F",4,lcd_Spalte,NO_CR_LF);      // Für die jeweilige Wbox "F" auf dem Display schreiben
                }
                else if (Zustand->eChargeState == NOT_FULL)                 // Auto nicht voll geladen?
                {
                    hd44780_write_string("L",4,lcd_Spalte,NO_CR_LF);      // Für die jeweilige Wbox "F" auf dem Display schreiben
                }
            }

        }
    }
}
