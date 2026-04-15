/*
 * fezui_config.c
 *
 *      Author: xq123
 */

#include "fezui.h"
#include "fezui_var.h"
#include "file_system.h"
#include "record.h"
#include "main.h"

#define MD_OLED_RST_Clr() GPIO_WriteBit(DISP_RES_GPIO_Port, DISP_RES_Pin, 0)
#define MD_OLED_RST_Set() GPIO_WriteBit(DISP_RES_GPIO_Port, DISP_RES_Pin, 1)

static fezui_animation_base_t frame_animation = {.duration = DEFAULT_ANIMATION_DURATION, .easing_func = fezui_animation_linear_ease, .mode = EASE_INOUT};

#define SCL_PORT OLED_SCL_GPIO_Port
#define SCL_PIN  OLED_SCL_Pin
#define SDA_PORT OLED_SDA_GPIO_Port
#define SDA_PIN  OLED_SDA_Pin
// U8g2 硬件接口回调函数
uint8_t u8x8_gpio_and_delay_stm32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch(msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            break;
        case U8X8_MSG_DELAY_MILLI:
            HAL_Delay(arg_int);
            break;
        case U8X8_MSG_DELAY_10MICRO:
            DWT_Delay_us(10);
            break;
        case U8X8_MSG_DELAY_100NANO:
            DWT_Delay_us(1);
            break;
        case U8X8_MSG_GPIO_I2C_CLOCK:
            if (arg_int) {
                HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_SET);
            } else {
                HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_RESET);
            }
            break;
        case U8X8_MSG_GPIO_I2C_DATA:
            if (arg_int) {
                HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_SET);
            } else {
                HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_RESET);
            }
            break;
        default:
            return 0;
    }
    return 1;
}

void fezui_init()
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&(fezui.u8g2), U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay_stm32);
    u8g2_InitDisplay(&(fezui.u8g2));
    u8g2_SetPowerSave(&(fezui.u8g2), 0);
    // u8g2_SetBitmapMode(&(fezui.u8g2), 1);
    u8g2_SetFontMode(&(fezui.u8g2), 1);
    u8g2_SetFont(&(fezui.u8g2), u8g2_font_tom_thumb_4x6_mf);
    u8g2_SetFontPosBottom(&fezui.u8g2);
    u8g2_SetBitmapMode(&fezui.u8g2, 1);

    fezui_POST();

   //debugpage_init();
   //menupage_init();
   //launcherpage_init();
   //settingspage_init();
   //calibrationpage_init();
   //advancedconfigpage_init();
   //keymappage_init();
   //keylistpage_init();
   //displayconfigpage_init();
   //panelpage_init();
   //advancedsettingspage_init();

    fezui_frame_init(&g_mainframe, &homepage, NULL);
    // Keybaord_SendReport_Enable=false;
    //fezui_frame_navigate(&g_mainframe, &calibrationpage);
    // fezui_frame_navigate(&g_mainframe, &oscilloscopepage);
}

static float screensaver_countdown = 0;
void fezui_timer_handler()
{
    fezui_frame_tick(&g_mainframe);
    //CONVERGE_TO(screensaver_countdown, (float)fezui.screensaver_countdown,0.02);
    fezui_notification_update(&fezui, &fezui_notification);
    fezui_cursor_move(&fezui, &g_fezui_cursor, &g_target_cursor);
    // fezui_animated_cursor_update(&animated_cursor);
    g_kps_history_max = loop_array_max(&g_kps_history);
    // fezui_save_counts();

    uint8_t key_pressed_num = 0;
    for (uint8_t i = 0; i < TOTAL_KEY_NUM; i++)
    {
        key_pressed_num += keyboard_get_key(i)->state;
    }
    if (g_kps_history_max || key_pressed_num)
    {
        fezui.screensaver_countdown = fezui.screensaver_timeout;
    }
}

void fezui_render_handler()
{
    u8g2_ClearBuffer(&(fezui.u8g2));
    u8g2_SetDrawColor(&(fezui.u8g2), 1);
    fezui_frame_draw(&g_mainframe);
    fezui_draw_notification(&fezui, &fezui_notification);
    if (fezui.invert)
    {
        u8g2_SetDrawColor(&(fezui.u8g2), 2);
        u8g2_DrawBox(&(fezui.u8g2), 0, 0, WIDTH, HEIGHT);
    }
    if (fezui.screensaver_timeout)
    {
        fezui_veil_full_screen(&(fezui), (screensaver_countdown) < 7 ? 7 - screensaver_countdown : 0);
        u8g2_SetPowerSave(&(fezui.u8g2), (screensaver_countdown<1));
    }
    if (fezui.show_fps)
    {
        u8g2_SetDrawColor(&(fezui.u8g2), 1);
        u8g2_DrawBox(&fezui.u8g2, 95 + 14, 0, WIDTH - 95 - 14, 11);
        u8g2_SetDrawColor(&(fezui.u8g2), 2);
        u8g2_SetFont(&(fezui.u8g2), u8g2_font_micro_tr);
        u8g2_DrawUTF8(&(fezui.u8g2), 95 + 15, 10, g_fpsstr);
    }
    u8g2_SendBuffer(&(fezui.u8g2));
    g_fezui_fps++;
}

#define CHAR_HEIGHT 8
#define CHAR_WIDTH 5
void fezui_POST()
{
    u8g2_SetFont(&fezui.u8g2, u8g2_font_3x3basic_tr);
    u8g2_DrawUTF8X2(&fezui.u8g2, 0, CHAR_HEIGHT, "FEZUI");
    u8g2_SetFont(&fezui.u8g2, u8g2_font_5x7_tf);
    u8g2_DrawHLine(&fezui.u8g2, 0, CHAR_HEIGHT, WIDTH);
    u8g2_SendBuffer(&fezui.u8g2);
    u8g2_DrawLog(&fezui.u8g2, 0, CHAR_HEIGHT * 2, &g_u8log);
}

#define ADD_STRING(str) strings[index++] = (str);
void keyid_prase(uint16_t id, char *str, uint16_t str_len)
{
    const char *strings[16] = {0};
    uint8_t index = 0;
    memset(str, 0, str_len);
    switch (id & 0xFF)
    {
    case MOUSE_COLLECTION:
        ADD_STRING(g_hid_usage_names[MOUSE_COLLECTION + 8 + KEYCODE_GET_SUB(id)]);
        break;
    case LAYER_CONTROL:
        ADD_STRING("Layer Control");
        break;
    case KEY_USER:
        ADD_STRING("User Defined");
        break;
    case KEYBOARD_OPERATION:
        ADD_STRING("Keyboard Defined");
        break;
    case KEY_TRANSPARENT:
        ADD_STRING("Transparent");
        break;
    default:
        for (uint8_t i = 0; i < 8; i++)
        {
            if ((id >> 8) & BIT(i))
            {
                ADD_STRING(g_hid_usage_names[i]);
            }
        }
        if (id & 0xFF)
        {
            
            ADD_STRING(g_hid_usage_names[(id & 0xFF) + 8]);
        }
        break;
    }
    if (index)
    {
        strcat(str, strings[0]);
        for (int i = 1; i < index; i++)
        {
            strcat(str, " + ");
            strcat(str, strings[i]);
        }
        
    }
    else
    {
        strcat(str, "None");
    }
}

void fezui_reset()
{
    fezui.invert = 0;
    fezui.contrast = 255;
    fezui.screensaver_countdown = 0;
    fezui.screensaver_timeout = 0;
    fezui.speed = 0.05;
    fezui.lang = LANG_EN;
    fezui.show_fps = true;
    fezui_apply(&fezui);
}
