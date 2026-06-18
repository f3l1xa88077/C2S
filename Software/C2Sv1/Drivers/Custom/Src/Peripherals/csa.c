/*
 * csa.c
 *
 *  Created on: Dec 13, 2025
 *      Author: felix
 */

#include"main.h"
#include"csa.h"

/**
 * Reads the ADC value from the CSA output and writes it to the CSA struct
 *
 * @param *hadc ADC hal handle for the CSA pin.
 * @param csa Instance of the CSA
 *
 */
void CSA_ADC(ADC_HandleTypeDef *hadc, CSA_HandleTypeDef *csa)
{
    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
    csa->adc = (uint16_t)HAL_ADC_GetValue(hadc);
    HAL_ADC_Stop(hadc);
}

/**
 * Calculates the voltage difference across the CSA shunt resistor
 *
 * @param csa Instance of the CSA
 *
 */
void CSA_VOLTAGE(CSA_HandleTypeDef *csa)
{
	// Calculate the output voltage of the CSA
	(csa->voltage) = (csa->adc) * (csa->Vcc) / (16383.0f);
}

/**
 * Calculates the current through the CSA shunt resistor
 *
 * @param csa Instance of the CSA
 *
 */
void CSA_CURRENT(CSA_HandleTypeDef *csa)
{
	// Calculate current
	(csa->current) = (csa->voltage) / ((csa->Rshunt) * (csa->gain));
}

/**
 * Calculates the power dissipates by the CSA shunt resistor
 *
 * @param csa Instance of the CSA
 *
 */
void CSA_POWER(CSA_HandleTypeDef *csa)
{
	// Calculate power dissipated by shunt resistor
    (csa->power) = (csa->current) * (csa->current) * (csa->Rshunt);
}

/**
 * Estimates the power consumed by the entire PCB using P=VI
 *
 * @param csa Instance of the CSA
 *
 */
void CSA_SYSTEM_POWER(CSA_HandleTypeDef *csa)
{
	// Calculate power from the calculated current through the shunt resistor
    (csa->sys_power) = (csa->Vbus) * (csa->current);
}

/**
 * Inserts constant (unchanging) variables into CSA struct
 *
 * @param csa CSA Instance
 * @param Rshunt Resistance value of shunt resistor in Ohms
 * @param Vcc Voltage of the main board (3.3) in Volts
 * @param Vbus Voltage from USBC / before the 3.3V LDO (5V)
 * @param gain Gain of the CSA (INA181A has gain of 50)
 *
 *
 */
void CSA_Init(CSA_HandleTypeDef *csa, float Rshunt, float Vcc, float Vbus, int gain)
{
    csa->Rshunt = Rshunt;
    csa->Vcc = Vcc;
    csa->Vbus = Vbus;
    csa->gain = gain;
    csa->voltage = 0.0f;
    csa->current = 0.0f;
    csa->power = 0.0f;
    csa->sys_power = 0.0f;
}

/**
 * Collates most functions from above ^^ into one single call
 *
 * @param *hadc ADC hal handle for the CSA pin.
 * @param csa Instance of the CSA
 *
 */
void CSA_Update(CSA_HandleTypeDef *csa, ADC_HandleTypeDef *hadc)
{
	CSA_ADC(hadc, csa);
	CSA_VOLTAGE(csa);
	CSA_CURRENT(csa);
	CSA_POWER(csa);
	CSA_SYSTEM_POWER(csa);
}
