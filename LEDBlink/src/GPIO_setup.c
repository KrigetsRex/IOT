/*
 * GPIO_setup.c
 *
 *  Created on: Jan 20, 2018
 *      Author: Luke
 */

#include "stm32l4xx.h"
#include "GPIO_setup.h"

void GPIO_setup(char letter, char pin, char mode, char type, char speed, char pupd){
	long long addr = 0x48000000;

	switch (letter){
	case 'A':
		addr = 0x48000000;
		break;
	case 'B':
		addr = 0x48000400;
		break;
	case 'C':
		addr = 0x48000800;
		break;
	case 'D':
		addr = 0x48000C00;
		break;
	case 'E':
		addr = 0x48001000;
		break;
	case 'F':
		addr = 0x48001400;
		break;
	case 'G':
		addr = 0x48001800;
		break;
	case 'H':
		addr = 0x48001C00;
		break;
	}

		int modes = (mode << (pin*2));
		int modec = (0x11 << (pin*2));
		int types = (type << (pin));
		int speeds = (speed << (pin*2));
		int speedc = (11 << (pin*2));
		int pupds = (pupd << (pin*2));
		int pupdc = (11 << (pin*2));

	/*unlock LCKR
	MODIFY_REG(*(long long*)(addr+0x1C), 0xff, 0x100);
	MODIFY_REG(*(long long*)(addr+0x1C), 0xff, 0x000);
	MODIFY_REG(*(long long*)(addr+0x1C), 0xff, 0x100);
	READ_REG(*(long long*)(addr+0x1C));
    */
	
	//enable clock
	SET_BIT(*(long long*)(0x4002104c), 0x1);  //gpiob? 
	
	MODIFY_REG(*(long long*)addr, modec, modes);
	SET_BIT(*(long long*)(addr+0x4), types);
	MODIFY_REG(*(long long*)(addr+0x8), speedc, speeds);
	MODIFY_REG(*(long long*)(addr+0xC), pupdc, pupds);

}
