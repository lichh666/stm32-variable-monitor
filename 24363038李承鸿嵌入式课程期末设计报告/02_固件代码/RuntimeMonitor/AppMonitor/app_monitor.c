#include "app_monitor.h"
#include "board_port.h"
#include "monitor_protocol.h"

static uint32_t g_tick_100ms = 0;
static uint32_t g_tick_s = 0;
static uint16_t g_led_period_100ms = 5;
static int32_t g_temp_x10 = 250;
static int32_t g_alarm_limit_x10 = 360;
static uint16_t g_virtual_adc_mv = 1230;
static uint8_t g_alarm = 0;
static uint8_t g_beep_enable = 1;
static uint32_t g_key_count = 0;
static uint8_t g_run = 1;

static mon_var_t g_vars[] = {
    {"tick_s", MON_VAR_U32, &g_tick_s, 0, "s", "system_seconds"},
    {"temp_x10", MON_VAR_I32, &g_temp_x10, 1, "0.1C", "virtual_temperature"},
    {"alarm_limit", MON_VAR_I32, &g_alarm_limit_x10, 1, "0.1C", "alarm_threshold"},
    {"adc_mv", MON_VAR_U16, &g_virtual_adc_mv, 1, "mV", "virtual_adc_value"},
    {"led_period", MON_VAR_U16, &g_led_period_100ms, 1, "100ms", "led_toggle_period"},
    {"beep_en", MON_VAR_U8, &g_beep_enable, 1, "bool", "beeper_enable"},
    {"alarm", MON_VAR_U8, &g_alarm, 0, "bool", "alarm_state"},
    {"key_count", MON_VAR_U32, &g_key_count, 0, "cnt", "key_press_count"},
    {"run", MON_VAR_U8, &g_run, 1, "bool", "system_run_enable"},
};

void AppMonitor_Init(void)
{
    Board_Init();
    Monitor_Init(Board_UartPuts, g_vars, sizeof(g_vars) / sizeof(g_vars[0]));
    Board_LcdShowStatus(g_temp_x10, g_alarm_limit_x10, g_tick_s, g_alarm);
}

void AppMonitor_OnUartChar(char ch)
{
    Monitor_OnChar(ch);
}

void AppMonitor_Tick100ms(void)
{
    uint8_t key;
    if (!g_run) return;

    g_tick_100ms++;
    if ((g_tick_100ms % 10) == 0) g_tick_s++;

    /* Virtual process value: slow triangular wave for demo without external sensor. */
    if ((g_tick_100ms % 3) == 0) {
        static int8_t dir = 1;
        g_temp_x10 += dir * 2;
        if (g_temp_x10 > 420) dir = -1;
        if (g_temp_x10 < 240) dir = 1;
        g_virtual_adc_mv = (uint16_t)(800 + (g_temp_x10 - 200) * 12);
    }

    key = Board_KeyScan();
    if (key) {
        g_key_count++;
        if (key == 1 && g_alarm_limit_x10 < 600) g_alarm_limit_x10 += 10;
        if (key == 2 && g_alarm_limit_x10 > 100) g_alarm_limit_x10 -= 10;
        if (key == 3) g_beep_enable = !g_beep_enable;
    }

    g_alarm = (g_temp_x10 >= g_alarm_limit_x10) ? 1 : 0;
    if (g_led_period_100ms == 0) g_led_period_100ms = 1;
    if ((g_tick_100ms % g_led_period_100ms) == 0) {
        static uint8_t led = 0;
        led = !led;
        Board_LedSet(0, led);
    }
    Board_LedSet(1, g_alarm);
    Board_BeepSet(g_alarm && g_beep_enable);
    Board_LcdShowStatus(g_temp_x10, g_alarm_limit_x10, g_tick_s, g_alarm);
}

void AppMonitor_Loop(void)
{
    /*
     * For a simple demo, call AppMonitor_Tick100ms() every 100ms in main.
     * For a real project, call AppMonitor_Tick100ms() in TIM interrupt or SysTick hook.
     */
    AppMonitor_Tick100ms();
    Monitor_Periodic100ms();
    Board_DelayMs(100);
}
