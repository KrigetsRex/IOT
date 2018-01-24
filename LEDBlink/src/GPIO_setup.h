/*
 * GPIO_setup.h
 *
 *  Created on: Jan 20, 2018
 *      Author: Luke
 */

#ifndef GPIO_SETUP_H_
#define GPIO_SETUP_H_

void GPIO_setup(char letter, char pin, char mode, char type, char speed, char pupd);

//mode
#define INPUT (char) 00
#define OUTPUT (char) 01
#define ALTERNATE (char) 10
#define ANALOG (char) 11
//type
#define PUSH_PULL (char) 0
#define OPEN_DRAIN (char) 1
//speed
#define SLOW (char) 00
#define MEDIUM (char) 01
#define FAST (char) 10
#define VFAST (char) 11
//PUPD
#define NONE (char) 00
#define PULLUP (char) 01
#define PULLDN (char) 10

#endif /* GPIO_SETUP_H_ */
