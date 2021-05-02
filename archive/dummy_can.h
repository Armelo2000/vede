#ifndef DUMMY_CAN_H
#define DUMMY_CAN_H

#include "stdint.h"



#ifndef YES
#define YES 	1
#endif
#ifndef NO
#define NO		0
#endif

//#define NULL  ((void*)0)

#define DUMMY

typedef enum Plug_In_State
{
  DISCONNECTED,		/* WBox is disconnected */
  CONNECTED				/* WBox is connected */
}ePlug_In_State;

typedef enum Charge_State
{
  NOT_FULL,			/* WBox is NOT FULL */
  FULL					/* WBox is FULL */
}eCharge_State;


// Structure for WBox Object
typedef struct _wbox
{
	ePlug_In_State eConnectionState;
	eCharge_State  eChargeState;
	uint8_t ucIndex;
}WBox;

#ifdef DUMMY

typedef struct _data
{
	uint32_t ident;
	uint16_t receive;
	uint8_t data_2_send[8];
}can_data;

#endif

void Handle_msg(void);

#endif
