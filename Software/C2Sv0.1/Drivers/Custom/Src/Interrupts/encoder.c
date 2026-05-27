/*
 * encoder.c
 *
 *  Created on: Dec 13, 2025
 *      Author: felix
 */

#include "encoder.h"

// Static encoder instance (one encoder example)
extern int selected_mode;
extern Encoder_HandleTypeDef scroll_encoder;

// -------------------------- Internal functions ---------------------------

/**
 * @brief Updates the rotary encoder state machine.
 *
 * Processes encoder transitions based on the triggering GPIO pin and edge
 * direction. Detects clockwise (CW) and counterclockwise (CCW) detents
 * and updates the active configuration value accordingly.
 *
 * @param encoder Pointer to the encoder handle structure.
 * @param GPIO_Pin GPIO pin that triggered the interrupt.
 * @param rising 1 if rising edge, 0 if falling edge.
 */
void Encoder_Update(Encoder_HandleTypeDef *encoder, uint16_t GPIO_Pin, uint8_t rising)
{
    // Read instantaneous encoder channel states
    encoder->enc_a = HAL_GPIO_ReadPin(encoder->pinA_port, encoder->pinA);
    encoder->enc_b = HAL_GPIO_ReadPin(encoder->pinB_port, encoder->pinB);

    // Expected clockwise transition sequence
    static const uint8_t CW_sequence[4][2] = {
        {0, 1}, {1, 1}, {0, 0}, {1, 0}
    };

    // Expected counterclockwise transition sequence
    static const uint8_t CCW_sequence[4][2] = {
        {1, 1}, {0, 1}, {1, 0}, {0, 0}
    };

    // Check clockwise transition
    if (GPIO_Pin == encoder->pinA &&
        rising == CW_sequence[encoder->step][1])
    {
        encoder->step++;
        if (encoder->step >= 4)
        {
            if (encoder->cfg.arr[encoder->cfg.idx] < 15)
            {
                encoder->cfg.arr[encoder->cfg.idx]++; // One CW detent
            }
            encoder->step = 0;
        }
        return;
    }

    // Check counterclockwise transition
    if (GPIO_Pin == encoder->pinB &&
        rising == CCW_sequence[encoder->step][1])
    {
        encoder->step++;
        if (encoder->step >= 4)
        {
            if (encoder->cfg.arr[encoder->cfg.idx] > 0)
            {
                encoder->cfg.arr[encoder->cfg.idx]--; // One CCW detent
            }
            encoder->step = 0;
        }
        return;
    }

    // Invalid transition, reset state machine
    encoder->step = 0;
}


/**
 * @brief Initialises the encoder handle structure.
 *
 * Configures GPIO ports and pins for encoder channels A and B,
 * resets internal state variables, and initialises the associated
 * settings structure.
 *
 * @param encoder Pointer to the encoder handle structure.
 * @param pinA_port GPIO port for channel A.
 * @param pinA GPIO pin for channel A.
 * @param pinB_port GPIO port for channel B.
 * @param pinB GPIO pin for channel B.
 */
void Encoder_Init(Encoder_HandleTypeDef *encoder,
                  GPIO_TypeDef* pinA_port, uint16_t pinA,
                  GPIO_TypeDef* pinB_port, uint16_t pinB)
{
    encoder->enc_a = 0;
    encoder->enc_b = 0;
    encoder->step = 0;
    encoder->pinA_port = pinA_port;
    encoder->pinA = pinA;
    encoder->pinB_port = pinB_port;
    encoder->pinB = pinB;

    // Initialise configuration settings
    Encoder_Settings_Init(&(encoder->cfg));
}


/**
 * @brief Initialises the encoder configuration settings.
 *
 * Sets the number of configurable settings, initial index,
 * and clears all stored setting values.
 *
 * @param cfg Pointer to the settings handle structure.
 */
void Encoder_Settings_Init(Settings_HandleTypeDef *cfg)
{
    cfg->num = MAX_SETTINGS;
    cfg->idx = STARTING_SETTING;

    for (int i = 0; i < MAX_SETTINGS; i++)
    {
        cfg->arr[i] = 0;
    }
}


/**
 * @brief Returns the currently selected encoder value.
 *
 * Provides access to the active setting value for external modules.
 *
 * @return Current setting value.
 */
int8_t Encoder_GetValue(void)
{
    return scroll_encoder.cfg.arr[scroll_encoder.cfg.idx];
}

