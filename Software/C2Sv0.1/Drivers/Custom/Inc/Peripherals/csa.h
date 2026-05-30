/*
 * csa.h
 *
 *  Created on: Dec 13, 2025
 *      Author: felix
 */

#ifndef CUSTOM_INC_CSA_H_
#define CUSTOM_INC_CSA_H_

typedef struct {
    float Rshunt;           // Shunt resistor value [Ω]
    float Vcc;              // ADC reference / supply voltage [V]
    float Vbus;             // Bus voltage if needed [V]
    int gain;               // CSA gain
    volatile float adc; 	// ADC value read from pin
    volatile float voltage; // Measured voltage across shunt [V]
    volatile float current; // Calculated current [A]
    volatile float power;   // Calculated power across load [W]
    volatile float sys_power; // System power [W]
} CSA_HandleTypeDef;

void CSA_ADC(ADC_HandleTypeDef *hadc, CSA_HandleTypeDef *csa);

void CSA_VOLTAGE(CSA_HandleTypeDef *csa);

void CSA_CURRENT(CSA_HandleTypeDef *csa);

void CSA_POWER(CSA_HandleTypeDef *csa);

void CSA_SYSTEM_POWER(CSA_HandleTypeDef *csa);

// Initialize CSA struct
void CSA_Init(CSA_HandleTypeDef *csa, float Rshunt, float Vcc, float Vbus, int gain);

// Read ADC, calculate voltage, current, power
void CSA_Update(CSA_HandleTypeDef *csa, ADC_HandleTypeDef *hadc);

#endif /* CUSTOM_INC_CSA_H_ */
