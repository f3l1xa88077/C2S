/*
 * encoder.h
 *
 *  Created on: Dec 13, 2025
 *      Author: felix
 */

#ifndef ENCODER_H
#define ENCODER_H

#include "main.h"
#include <stdint.h>

#define MAX_SETTINGS 3
#define STARTING_SETTING 0

typedef struct {
	int num; 					// Number of settings
    int arr[MAX_SETTINGS]; 		// Array of settings containing values
    volatile int idx;			// Settings index
} Settings_HandleTypeDef;

// Encoder handle structure
typedef struct {
	Settings_HandleTypeDef cfg;
    volatile uint8_t enc_a;
    volatile uint8_t enc_b;
    volatile uint8_t step;
    GPIO_TypeDef* pinA_port;
    uint16_t pinA;
    GPIO_TypeDef* pinB_port;
    uint16_t pinB;
} Encoder_HandleTypeDef;

// Public functions
void Encoder_Update(Encoder_HandleTypeDef *encoder, uint16_t GPIO_Pin, uint8_t rising);

void Encoder_Init(Encoder_HandleTypeDef *encoder,
                  GPIO_TypeDef* pinA_port, uint16_t pinA,
                  GPIO_TypeDef* pinB_port, uint16_t pinB);

void Encoder_Settings_Init(Settings_HandleTypeDef *cfg);

#endif
