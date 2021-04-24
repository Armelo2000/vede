#include "dummy_can.h"

static uint8_t fg_ucCountConnected = 0;
static uint8_t fg_ucCountFullCharged = 0;
WBox *fg_ptAllWBox[16] = {(WBox *)NULL};
WBox fg_tAllWBox[16];
can_data tCanData;
can_data * DATA = &tCanData;
// Handle new message
void Handle_msg(void)
{
	if(DATA != NULL)
	{ 
		//check if the address is a valid Wbox address
		if((DATA->ident >= 0x00000000) && (DATA->ident <= 0x0000000F))
		{
	    uint8_t ucFirstByte = (DATA->receive >> 8) & 0x01;
	    uint8_t ucSecondByte = DATA->receive & 0x01;
			//The address is valid
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
			
			if(ucFirstByte == 0x01)
			{
				// The WBox is connected
				//Check if the was already connected
				if(fg_ptAllWBox[DATA->ident] == (WBox *)NULL)
				{
					//save the connected WBox
					fg_ptAllWBox[DATA->ident] = &fg_tAllWBox[DATA->ident];
				}
				//WBox is connected
				if(fg_ptAllWBox[DATA->ident]->eConnectionState == DISCONNECTED){
				  //increment only if it was disconnected 
				  fg_ucCountConnected++;
				}
				fg_ptAllWBox[DATA->ident]->eConnectionState = CONNECTED;
				
				// Check the second byte
				if(ucSecondByte == 0x01)
				{
				  //WBox is full charged
				  if(fg_ptAllWBox[DATA->ident]->eChargeState == NOT_FULL)
					{
					  //increment only if it was not full charged 
					  fg_ucCountFullCharged++;
					}				
					fg_ptAllWBox[DATA->ident]->eChargeState = FULL;
					
					//Stop the transmission at the address "DATA->ident" (Call the function to stop charge process)
				}
				else{
					// The WBox is not full charged.
				    if(fg_ptAllWBox[DATA->ident]->eChargeState == FULL){
					  //decrement only if it was full charged 
					  fg_ucCountFullCharged--;
					}					
					fg_ptAllWBox[DATA->ident]->eChargeState = NOT_FULL;
					
					//Start the transmission at the address "DATA->ident" (Call the function to start charge process)
				}

			}
			else
			{
				//WBox is disconnected. Stop power transmission and reset flag to disconnect and free the array
				if(fg_ptAllWBox[DATA->ident] != (WBox *)NULL)
				{
				    if(fg_ptAllWBox[DATA->ident]->eConnectionState == CONNECTED){
					  //decrement only if it was connected 
					  fg_ucCountConnected--;
					}
					if(fg_ptAllWBox[DATA->ident]->eChargeState == FULL){
					  //decrement only if it was full charged 
					  fg_ucCountFullCharged--;
					}
				  
				  fg_ptAllWBox[DATA->ident]->eConnectionState = DISCONNECTED;
				  fg_ptAllWBox[DATA->ident]->eChargeState = NOT_FULL;
				  fg_ptAllWBox[DATA->ident] = NULL;
				  
				  
				  //Stop the transmission at the address "DATA->ident" (Call the function to stop charge process)
				}
			}
				
		}
	}
}

