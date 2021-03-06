#include "stdint.h"
#include "process_can.h"

#ifdef SIMULATION
extern mboxdata tCanData;
#endif
int main(void)
{
	wbox_init();
	//no WBox is connected
	Verarbeitung_CAN_msg(&tCanData);
	// WBox at Address 00 is connected
	tCanData.ident = 0x00;
	tCanData.receive = 0x0100;
	Verarbeitung_CAN_msg(&tCanData);
	// WBox at Address 01 is connected
	tCanData.ident = 0x01;
	tCanData.receive = 0x0100;
	Verarbeitung_CAN_msg(&tCanData);
	// WBox at Address 02 is connected
	tCanData.ident = 0x02;
	tCanData.receive = 0x0100;
	Verarbeitung_CAN_msg(&tCanData);
	// WBox at Address 03 is connected
	tCanData.ident = 0x03;
	tCanData.receive = 0x0100;
	Verarbeitung_CAN_msg(&tCanData);
	// WBox at Address 01 is Full
	tCanData.ident = 0x01;
	tCanData.receive = 0x0101;
	Verarbeitung_CAN_msg(&tCanData);
	// WBox at Address 03 is Full
	tCanData.ident = 0x03;
	tCanData.receive = 0x0101;
	Verarbeitung_CAN_msg(&tCanData);
	// WBox at Address 00 is disconnect (was full)
	tCanData.ident = 0x00;
	tCanData.receive = 0x0001;
	Verarbeitung_CAN_msg(&tCanData);
	// WBox at Address 02 is disconnect (was not full)
	tCanData.ident = 0x02;
	tCanData.receive = 0x0000;
	Verarbeitung_CAN_msg(&tCanData);
	// WBox at Address 01 is disconnect (and full)
	tCanData.ident = 0x01;
	tCanData.receive = 0x0001;
	Verarbeitung_CAN_msg(&tCanData);
	// WBox at Address 03 is connected and full
	tCanData.ident = 0x03;
	tCanData.receive = 0x0001;
	Verarbeitung_CAN_msg(&tCanData);
  while(1);
}
