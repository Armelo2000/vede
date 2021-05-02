#include <stdio.h>
#include <stdlib.h>
#include "stdint.h"
#include "dummy_can.h"

extern can_data tCanData;

int main(void)
{
	//no WBox is connected
	Handle_msg();
	// WBox at Address 05 is connected
	tCanData.ident = 0x05;
	tCanData.receive = 0x0100;
	Handle_msg();
	// WBox at Address 08 is connected
	tCanData.ident = 0x08;
	tCanData.receive = 0x0100;
	Handle_msg();
	// WBox at Address 09 is connected
	tCanData.ident = 0x09;
	tCanData.receive = 0x0100;
	Handle_msg();
	// WBox at Address 02 is connected
	tCanData.ident = 0x02;
	tCanData.receive = 0x0100;
	Handle_msg();
	// WBox at Address 08 is Full
	tCanData.ident = 0x08;
	tCanData.receive = 0x0101;
	Handle_msg();
	// WBox at Address 05 is Full
	tCanData.ident = 0x05;
	tCanData.receive = 0x0101;
	Handle_msg();
	// WBox at Address 08 is disconnect (was full)
	tCanData.ident = 0x08;
	tCanData.receive = 0x0001;
	Handle_msg();
	// WBox at Address 02 is disconnect (was not full)
	tCanData.ident = 0x02;
	tCanData.receive = 0x0000;
	Handle_msg();

  while(1);
}

