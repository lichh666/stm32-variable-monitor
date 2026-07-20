#include "board_port.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/KEY/key.h"
#include <stdlib.h>
#include <string.h>

void Board_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;

    gpio_init_struct.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);

    gpio_init_struct.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);

    gpio_init_struct.Pin = GPIO_PIN_8;
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);

    key_init();

    Board_UartPuts("TEST LED0 PB5\r\n");
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    Board_DelayMs(800);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);

    Board_UartPuts("TEST LED1 PE5\r\n");
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
    Board_DelayMs(800);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);

    Board_UartPuts("TEST BEEP PB8\r\n");
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
    Board_DelayMs(800);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);

    Board_UartPuts("BOARD SELFTEST DONE\r\n");
}

void Board_LedSet(uint8_t index, uint8_t on)
{
    if (index == 0) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
    } else if (index == 1) {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }
}

void Board_BeepSet(uint8_t on)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

uint8_t Board_KeyScan(void)
{
    uint8_t key = key_scan(0);

    if (key == KEY0_PRES) {
        return 1;
    }

    if (key == KEY1_PRES) {
        return 2;
    }

    if (key == KEY2_PRES) {
        return 3;
    }

    return 0;
}

void Board_LcdShowStatus(int32_t temp_x10, int32_t limit_x10, uint32_t tick_s, uint8_t alarm)
{
    (void)temp_x10;
    (void)limit_x10;
    (void)tick_s;
    (void)alarm;
}

void Board_UartPuts(const char *s)
{
    if (s == 0) {
        return;
    }

    HAL_UART_Transmit(&g_uart1_handle, (uint8_t *)s, strlen(s), 1000);
}

void Board_DelayMs(uint32_t ms)
{
    delay_ms(ms);
}
