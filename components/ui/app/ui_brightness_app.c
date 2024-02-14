#include "lvgl.h"
#include "ui_helpers.h"
#include "ui_options_screen.h"
#include "ui_brightness_app.h"
#include "esp_lvgl_port.h"
#include "display.h"

static brightness_app_t *brightness_app;

static void slider_event_cb(lv_event_t *e)
{
    brightness_app->slider = lv_event_get_target(e);
    lv_snprintf(brightness_app->slider_label_buf, sizeof(brightness_app->slider_label_buf), "%d %%",
                (int)lv_slider_get_value(brightness_app->slider));
    lv_label_set_text(brightness_app->slider_label, brightness_app->slider_label_buf);
    lv_obj_align_to(brightness_app->slider_label, brightness_app->slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

void brightness_event_callback(lv_event_t *e)
{
    options_screen_t *options_screen = (options_screen_t *)e->user_data;

    brightness_app = (brightness_app_t *)malloc(sizeof(brightness_app_t));
    options_screen->lock = true;

    lv_obj_add_flag(options_screen->list, LV_OBJ_FLAG_HIDDEN);
    brightness_app->contanier = lv_obj_create(options_screen->screen);
    lv_obj_set_size(brightness_app->contanier, 240, 240);
    lv_obj_add_style(brightness_app->contanier, &options_screen->style, 0);
    lv_obj_align(brightness_app->contanier, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_load_anim(brightness_app->contanier, options_screen->list, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 100);
    lv_label_set_text(options_screen->top_label, "屏幕亮度");
    lv_obj_load_anim(options_screen->top_label, options_screen->top_label, LV_SCR_LOAD_ANIM_FADE_IN, 200, 100);

    lv_obj_clear_flag(options_screen->return_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(options_screen->save_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_load_anim(options_screen->return_button, options_screen->return_button, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200,
                     100);
    lv_obj_load_anim(options_screen->save_button, options_screen->save_button, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 100);

    /*Create a transition*/
    static const lv_style_prop_t props[] = { LV_STYLE_BG_COLOR, 0 };
    static lv_style_transition_dsc_t transition_dsc;
    lv_style_transition_dsc_init(&transition_dsc, props, lv_anim_path_linear, 300, 0, NULL);

    lv_style_init(&brightness_app->style_main);
    lv_style_set_bg_opa(&brightness_app->style_main, LV_OPA_COVER);
    lv_style_set_bg_color(&brightness_app->style_main, lv_color_hex3(0xbbb));
    lv_style_set_radius(&brightness_app->style_main, LV_RADIUS_CIRCLE);
    lv_style_set_pad_ver(&brightness_app->style_main, -2); /*Makes the indicator larger*/

    lv_style_init(&brightness_app->style_indicator);
    lv_style_set_bg_opa(&brightness_app->style_indicator, LV_OPA_COVER);
    lv_style_set_bg_color(&brightness_app->style_indicator, lv_palette_main(LV_PALETTE_CYAN));
    lv_style_set_radius(&brightness_app->style_indicator, LV_RADIUS_CIRCLE);
    lv_style_set_transition(&brightness_app->style_indicator, &transition_dsc);

    lv_style_init(&brightness_app->style_knob);
    lv_style_set_bg_opa(&brightness_app->style_knob, LV_OPA_COVER);
    lv_style_set_bg_color(&brightness_app->style_knob, lv_palette_main(LV_PALETTE_CYAN));
    lv_style_set_border_color(&brightness_app->style_knob, lv_palette_darken(LV_PALETTE_CYAN, 3));
    lv_style_set_border_width(&brightness_app->style_knob, 2);
    lv_style_set_radius(&brightness_app->style_knob, LV_RADIUS_CIRCLE);
    lv_style_set_pad_all(&brightness_app->style_knob, 6); /*Makes the knob larger*/
    lv_style_set_transition(&brightness_app->style_knob, &transition_dsc);

    lv_style_init(&brightness_app->style_pressed_color);
    lv_style_set_bg_color(&brightness_app->style_pressed_color, lv_palette_darken(LV_PALETTE_CYAN, 2));

    /*Create a slider and add the style*/
    brightness_app->slider = lv_slider_create(brightness_app->contanier);
    lv_obj_remove_style_all(brightness_app->slider); /*Remove the styles coming from the theme*/
    lv_obj_set_size(brightness_app->slider, 170, 17);
    lv_obj_center(brightness_app->slider);
    brightness_app->slider_label = lv_label_create(brightness_app->contanier);
    lv_slider_set_value(brightness_app->slider, display_brightness_get(), LV_ANIM_ON);
    lv_snprintf(brightness_app->slider_label_buf, sizeof(brightness_app->slider_label_buf), "%d %%",
                (int)lv_slider_get_value(brightness_app->slider));
    lv_label_set_text(brightness_app->slider_label, brightness_app->slider_label_buf);
    lv_obj_align_to(brightness_app->slider_label, brightness_app->slider, LV_ALIGN_BOTTOM_MID, 0, 30);
    lv_obj_add_event_cb(brightness_app->slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_add_style(brightness_app->slider, &brightness_app->style_main, LV_PART_MAIN);
    lv_obj_add_style(brightness_app->slider, &brightness_app->style_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(brightness_app->slider, &brightness_app->style_pressed_color,
                     LV_PART_INDICATOR | LV_STATE_PRESSED);
    lv_obj_add_style(brightness_app->slider, &brightness_app->style_knob, LV_PART_KNOB);
    lv_obj_add_style(brightness_app->slider, &brightness_app->style_pressed_color, LV_PART_KNOB | LV_STATE_PRESSED);
}

void return_save_button_event_callback(lv_event_t *e)
{
    options_screen_t *options_screen = (options_screen_t *)e->user_data;

    lv_obj_t *target = lv_event_get_target(e);
    if (target == options_screen->save_button) {
        uint32_t brightness_percent = lv_slider_get_value(brightness_app->slider);
        lvgl_port_lock(0);
        display_brightness_set(brightness_percent);
        lvgl_port_unlock();
    }

    lv_obj_add_flag(options_screen->return_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(options_screen->save_button, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(options_screen->top_label, "设 置");
    lv_obj_load_anim(options_screen->top_label, options_screen->top_label, LV_SCR_LOAD_ANIM_FADE_IN, 200, 100);
    lv_obj_load_anim(options_screen->list, brightness_app->contanier, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 100);
    lv_style_reset(&brightness_app->style_main);
    lv_style_reset(&brightness_app->style_indicator);
    lv_style_reset(&brightness_app->style_pressed_color);
    lv_style_reset(&brightness_app->style_knob);
    lv_style_reset(&brightness_app->style_pressed_color);
    lv_obj_del(brightness_app->contanier);
    options_screen->lock = false;
    free(brightness_app);
    lv_obj_clear_flag(options_screen->list, LV_OBJ_FLAG_HIDDEN);
}