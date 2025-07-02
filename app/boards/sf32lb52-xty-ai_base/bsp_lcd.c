#include "bsp_board.h"


#ifdef BSP_USING_LCD
#define LCD_RESET_PIN           (0)         // GPIO_A00
#define LCD_BL_PIN              (42)         // GPIO_A09



/***************************LCD ***********************************/
extern void BSP_PIN_LCD(void);
void BSP_LCD_Reset(uint8_t high1_low0)
{
    BSP_GPIO_Set(LCD_RESET_PIN, high1_low0, 1);
}

void BSP_LCD_PowerDown(void)
{
    // TODO: LCD power down
    BSP_GPIO_Set(LCD_BL_PIN, 0, 1);
}

void BSP_LCD_PowerUp(void)
{
    // TODO: LCD power up
    BSP_GPIO_Set(LCD_BL_PIN, 1, 1);
    HAL_Delay_us(500);      // lcd power on finish ,need 500us
    BSP_PIN_LCD();
}

#endif
