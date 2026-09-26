#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/display.h>
#include <vita2d.h>

#include "input.h"

static int running = 1;

typedef enum
{
    VIEW_BUTTONS,
    VIEW_TOUCH
} AppView;

typedef struct
{
    const char *label;
    unsigned int mask;
    int x;
    int y;
    int label_x;
    int label_y;
} ButtonInfo;

static const ButtonInfo button_info[] = {
    {"UP", SCE_CTRL_UP, 155, 145, 155, 135},
    {"DOWN", SCE_CTRL_DOWN, 155, 245, 145, 305},
    {"LEFT", SCE_CTRL_LEFT, 105, 195, 45, 220},
    {"RIGHT", SCE_CTRL_RIGHT, 205, 195, 250, 220},
    {"CROSS", SCE_CTRL_CROSS, 775, 245, 760, 305},
    {"CIRCLE", SCE_CTRL_CIRCLE, 825, 195, 870, 220},
    {"SQUARE", SCE_CTRL_SQUARE, 725, 195, 630, 220},
    {"TRIANGLE", SCE_CTRL_TRIANGLE, 775, 145, 745, 135},
    {"LTrigger", SCE_CTRL_LTRIGGER, 125, 65, 100, 55},
    {"RTrigger", SCE_CTRL_RTRIGGER, 825, 65, 800, 55},
    {"SELECT", SCE_CTRL_SELECT, 415, 435, 400, 495},
    {"START", SCE_CTRL_START, 515, 435, 505, 495},
    {NULL, 0} // End of the array
};

// Draw functions
static void draw_stick(vita2d_pgf *font, int center_x, int center_y, unsigned char x_value, unsigned char y_value, const char *label)
{
    float stick_x = center_x + (x_value - 128) * 0.25f;
    float stick_y = center_y + (y_value - 128) * 0.25f;

    vita2d_draw_rectangle(stick_x - 5, stick_y - 5, 10, 10, RGBA8(255, 255, 255, 255));
    vita2d_pgf_draw_text(font, center_x-40, center_y + 100, RGBA8(255, 255, 255, 255), 1.0f, label);
}

static void draw_tabs(vita2d_pgf *font, AppView view)
{
    unsigned int active = RGBA8(60, 140, 220, 255);
    unsigned int inactive = RGBA8(70, 70, 70, 255);

    vita2d_draw_rectangle(295, 15, 180, 48, view == VIEW_BUTTONS ? active : inactive);
    vita2d_draw_rectangle(485, 15, 180, 48, view == VIEW_TOUCH ? active : inactive);
    vita2d_pgf_draw_text(font, 350, 47, RGBA8(255, 255, 255, 255), 1.0f, "Buttons");
    vita2d_pgf_draw_text(font, 545, 47, RGBA8(255, 255, 255, 255), 1.0f, "Touch");
}

static void draw_touch_panel(vita2d_pgf *font, const char *label, int x, int y,
                             const SceTouchData *touch)
{
    unsigned int white = RGBA8(255, 255, 255, 255);
    vita2d_pgf_draw_text(font, x, y, white, 1.0f, label);
    vita2d_draw_rectangle(x, y + 15, 400, 250, RGBA8(180, 180, 180, 255));
    vita2d_draw_rectangle(x + 2, y + 17, 396, 246, RGBA8(35, 35, 45, 255));

    for (int i = 0; i < touch->reportNum; i++)
    {
        float marker_x = x + touch->report[i].x * (400.0f / 1920.0f);
        float marker_y = y + 15 + touch->report[i].y * (250.0f / 1088.0f);
        vita2d_draw_rectangle(marker_x - 8, marker_y - 8, 16, 16,
                              RGBA8(255, 70, 70, 255));
    }
}

int main(void)
{
    vita2d_init();

    vita2d_pgf *font = vita2d_load_default_pgf();

    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);

    unsigned int tested_buttons = 0; // Track tested buttons
    AppView view = VIEW_BUTTONS;

    while (running)
    {
        unsigned int pressed_buttons = 0;
        SceCtrlData pad = handle_input(&pressed_buttons);
        SceTouchData front_touch = {0};
        SceTouchData rear_touch = {0};
        handle_touch(&front_touch, &rear_touch);

        // Switch view between buttons and touch
        for (int i = 0; i < front_touch.reportNum; i++)
        {
            int tx = front_touch.report[i].x * 960 / 1920;
            int ty = front_touch.report[i].y * 544 / 1088;
            if (ty < 65)
            {
                if (tx >= 295 && tx < 475)
                    view = VIEW_BUTTONS;
                else if (tx >= 485 && tx < 665)
                    view = VIEW_TOUCH;
            }
        }

        vita2d_start_drawing();
        vita2d_clear_screen();

        draw_tabs(font, view);

        if (view == VIEW_BUTTONS)
        {
            draw_stick(font, 270, 365, pad.lx, pad.ly, "Left Stick");
            draw_stick(font, 690, 365, pad.rx, pad.ry, "Right Stick");

            for (size_t i = 0; i < sizeof(button_info) / sizeof(button_info[0]); i++)
            {
                int x = button_info[i].x;
                int y = button_info[i].y;

                if (button_info[i].label == NULL)
                    break;

                if (was_pressed(pressed_buttons, button_info[i].mask))
                    tested_buttons |= button_info[i].mask;

                unsigned int cross_color = RGBA8(100, 100, 100, 255);

                if (tested_buttons & button_info[i].mask)
                    cross_color = RGBA8(60, 255, 60, 255);

                if (is_held(&pad, button_info[i].mask))
                    cross_color = RGBA8(255, 60, 60, 255);

                vita2d_draw_rectangle(x, y, 36, 36, cross_color);
                vita2d_pgf_draw_text(font, button_info[i].label_x, button_info[i].label_y,
                                     RGBA8(255, 255, 255, 255), 1.0f, button_info[i].label);
            }
        }
        else
        {
            draw_touch_panel(font, "Front touch panel", 55, 125, &front_touch);
            draw_touch_panel(font, "Rear touch panel", 505, 125, &rear_touch);
        }

        vita2d_end_drawing();
        vita2d_swap_buffers();
    }

    // Cleanup
    vita2d_free_pgf(font);
    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}
