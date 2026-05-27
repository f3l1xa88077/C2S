/*
 * lcd.c
 *
 *  Created on: Dec 24, 2025
 *      Author: felix
 *
 *  Note that sLCD means Small LCD (16x2) and lLCD means Large LCD (128x64)
 */

#include "lcd.h"

int prev_cfg_id = 255;

/**
 * @brief Initialises the I2C LCD and displays the startup message.
 * @param hi2c Pointer to the I2C handle.
 */
void sLCD_Init(I2C_HandleTypeDef *hi2c)
{
    HD44780_Init(2);
    HD44780_Clear();
    HD44780_Backlight();

    HD44780_SetCursor(0, 0);
    HD44780_PrintStr("C2Sv0.1");

    HD44780_SetCursor(0, 1);
    HD44780_PrintStr("Initialising");
}

/**
 * @brief Displays the current config settings on the 16x2 LCD.
 * @param cfg_id Specifies the configuration setting (0 = Shutter Speed, 1 = ISO, 2 = Burst Amount).
 * @param val Value of the configuration setting (0-15).
 */
void sLCD_CFG_DISPLAY(int cfg_id, int val)
{
    const char *arr[] = {"Shutter Speed", "ISO", "Burst Amount"};
    const char *ss[] = {"5\"  ", "4\"  ", "3\"  ", "2\"  ", "1\"  ", "1/2  ", "1/4  ", "1/8  ",
                        "1/15 ", "1/30 ", "1/60 ", "1/125", "1/250", "1/320", "1/400", "1/600"};
    const char *iso[] = {"50   ", "100  ", "200  ", "400  ", "640  ", "800  ", "1000 ", "1250 ",
                         "1600 ", "2000 ", "2500 ", "3200 ", "4000 ", "6000 ", "8000 ", "10000"};
    const char *burst[] = {"1 ", "2 ", "3 ", "4 ", "5 ", "6 ", "7 ", "8 ", "9 ", "10", "11", "12", "13", "14", "15", "16"};
    const char **vals[] = {ss, iso, burst};

    char string1[17];
    char string2[17];

    if (cfg_id < 0 || cfg_id >= 3 || val < 0 || val > 15) return;

    if (prev_cfg_id != cfg_id)
    {
        sLCD_CLEAR();
        prev_cfg_id = cfg_id;

        HD44780_SetCursor(0, 0);
        snprintf(string1, sizeof(string1), "%s", arr[cfg_id]);
        HD44780_PrintStr(string1);

        HD44780_SetCursor(0, 1);
        snprintf(string2, sizeof(string2), "Value: %s", vals[cfg_id][val]);
    }
    else
    {
        HD44780_SetCursor(7, 1);
        snprintf(string2, sizeof(string2), "%s", vals[cfg_id][val]);
    }

    HD44780_PrintStr(string2);
}

/**
 * @brief Displays the current CSA settings on the 16x2 LCD.
 * @param cfg_id Specifies the configuration setting (0 = Average Power, 1 = Current, 2 = Voltage Drop).
 * @param val Value of the configuration setting (0-15).
 */
void sLCD_CSA_DISPLAY(int cfg_id, float *vals)
{
    const char *arr[] = {"Average Power", "Current", "Voltage Drop"};
    const char *units[] = {"(mW)", "(mA)", "(mV)"};

    char string1[17];
    char string2[17];

    if (prev_cfg_id != cfg_id)
    {
        sLCD_CLEAR();
        prev_cfg_id = cfg_id;

        HD44780_SetCursor(0, 0);
        snprintf(string1, sizeof(string1), "%s", arr[cfg_id]);
        HD44780_PrintStr(string1);
    }

	HD44780_SetCursor(0, 1);
	snprintf(string2, sizeof(string2), "%d %s", (int)vals[cfg_id], units[cfg_id]);

    HD44780_PrintStr(string2);
}

/**
 * @brief Clears the 16x2 LCD screen.
 */
void sLCD_CLEAR()
{
    HD44780_Clear();
}
