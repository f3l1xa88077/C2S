/*
 * led.h
 *
 *  Created on: Dec 12, 2025
 *      Author: felix
 */

#ifndef LED_H
#define LED_H

#include "main.h"      // for GPIO port/pin definitions
#include <stdint.h>


void LED_TOGGLE_MCU();
void LED_WRITE_MCU(uint16_t PinState);
void LED_TOGGLE_SENSOR();
void LED_WRITE_SENSOR(uint16_t PinState);
void LED_TOGGLE_READ();
void LED_WRITE_READ(uint16_t PinState);
void LED_TOGGLE_WRITE();
void LED_WRITE_WRITE(uint16_t PinState);
void LED_TOGGLE_DEBUG();
void LED_WRITE_DEBUG(uint16_t PinState);
void LED_CONFIGURE_GND();

void LED_SyncEncoder(volatile uint8_t value);


#endif
