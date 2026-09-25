#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/display.h>
#include <vita2d.h>

#include "input.h"

static int running = 1;
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
    {"RIGHT", SCE_CTRL_RIGHT, 205, 195, 245, 220},
    {"CROSS", SCE_CTRL_CROSS, 775, 245, 760, 305},
    {"CIRCLE", SCE_CTRL_CIRCLE, 825, 195, 870, 220},
    {"SQUARE", SCE_CTRL_SQUARE, 725, 195, 630, 220},
    {"TRIANGLE", SCE_CTRL_TRIANGLE, 775, 145, 745, 135},
    {"LTrigger", SCE_CTRL_LTRIGGER, 125, 65, 100, 55},
    {"RTrigger", SCE_CTRL_RTRIGGER, 825, 65, 800, 55},
    {"SELECT", SCE_CTRL_SELECT, 415, 435, 400, 495},
    {"START", SCE_CTRL_START, 515, 435, 505, 495},
    {NULL, 0} // Sentinel value to mark the end of the array
};

static void draw_stick(vita2d_pgf *font, int center_x, int center_y, unsigned char x_value, unsigned char y_value, const char *label)
{
    // Draw the stick position as a rectangle
    float stick_x = center_x + (x_value - 128) * 0.25f; // Scale down for visibility
    float stick_y = center_y + (y_value - 128) * 0.25f; // Scale down for visibility

    vita2d_draw_rectangle(stick_x - 5, stick_y - 5, 10, 10, RGBA8(255, 255, 255, 255));
    vita2d_pgf_draw_text(font, center_x-40, center_y + 100, RGBA8(255, 255, 255, 255), 1.0f, label);
}

int main(void)
{
    // Initialize Vita2D
    vita2d_init();

    vita2d_pgf *font = vita2d_load_default_pgf();

    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

    unsigned int tested_buttons = 0;
    // Main loop
    while (running)
    {
        unsigned int pressed_buttons = 0;
        SceCtrlData pad = handle_input(&pressed_buttons);

        // Start drawing
        vita2d_start_drawing();
        vita2d_clear_screen();

        draw_stick(font, 270, 365, pad.lx, pad.ly, "Left Stick");
        draw_stick(font, 690, 365, pad.rx, pad.ry, "Right Stick");

        for (size_t i = 0; i < sizeof(button_info) / sizeof(button_info[0]); i++)
        {
            int x = button_info[i].x;
            int y = button_info[i].y;

            if (button_info[i].label == NULL)
            {
                break; // Reached the sentinel value
            }

            if (was_pressed(pressed_buttons, button_info[i].mask))
            {
                tested_buttons |= button_info[i].mask;
            }

            unsigned int cross_color = RGBA8(100, 100, 100, 255);

            if (tested_buttons & button_info[i].mask)
            {
                cross_color = RGBA8(60, 255, 60, 255);
            }

            if (is_held(&pad, button_info[i].mask))
            {
                cross_color = RGBA8(255, 60, 60, 255);
            }

            vita2d_draw_rectangle(x, y, 36, 36, cross_color);
            vita2d_pgf_draw_text(font, button_info[i].label_x, button_info[i].label_y, RGBA8(255, 255, 255, 255), 1.0f, button_info[i].label);
        }

        // End drawing and swap buffers
        vita2d_end_drawing();
        vita2d_swap_buffers();
    }

    // Cleanup
    vita2d_free_pgf(font);
    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}