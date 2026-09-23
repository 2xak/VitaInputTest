#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/display.h>
#include <vita2d.h>

static int running = 1;
static unsigned int prev_buttons = 0;
typedef struct
{
    const char *label;
    unsigned int mask;
} ButtonInfo;

static const ButtonInfo button_info[] = {
    {"UP", SCE_CTRL_UP},
    {"DOWN", SCE_CTRL_DOWN},
    {"LEFT", SCE_CTRL_LEFT},
    {"RIGHT", SCE_CTRL_RIGHT},
    {"CROSS", SCE_CTRL_CROSS},
    {"CIRCLE", SCE_CTRL_CIRCLE},
    {"SQUARE", SCE_CTRL_SQUARE},
    {"TRIANGLE", SCE_CTRL_TRIANGLE},
    {"L1", SCE_CTRL_L1},
    {"R1", SCE_CTRL_R1},
    {"L2", SCE_CTRL_L2},
    {"R2", SCE_CTRL_R2},
    {NULL, 0} // Sentinel value to mark the end of the array
};

static int is_held(const SceCtrlData *pad, unsigned int button_mask)
{
    return (pad->buttons & button_mask) != 0;
}

static int was_pressed(const SceCtrlData *pad, unsigned int button_mask)
{
    return (pad->buttons & button_mask) && !(prev_buttons & button_mask);
}

static SceCtrlData handle_input(unsigned int *pressed_buttons)
{
    SceCtrlData pad = {0};
    sceCtrlPeekBufferPositive(0, &pad, 1);

    *pressed_buttons = pad.buttons & ~prev_buttons; // Get newly pressed buttons

    /*
    if (is_held(&pad, SCE_CTRL_CROSS))
    {
        running = 0; // Exit the application when CROSS button is pressed
    }
    */

    prev_buttons = pad.buttons;
    return pad;
}

int main(void)
{
    // Initialize Vita2D
    vita2d_init();

    vita2d_pgf *font = vita2d_load_default_pgf();

    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

    // Main loop
    while (running)
    {
        unsigned int pressed_buttons = 0;
        SceCtrlData pad = handle_input(&pressed_buttons);

        // Start drawing
        vita2d_start_drawing();
        vita2d_clear_screen();

        for (size_t i = 0; i < sizeof(button_info) / sizeof(button_info[0]); i++)
        {
            int column = i / 6;
            int row = i % 6;
            float x = 120.0f + column * 300.0f;
            float y = 70.0f + row * 65.0f;

            if (button_info[i].label == NULL)
            {
                break; // Reached the sentinel value
            }

            unsigned int cross_color = (is_held(&pad, button_info[i].mask))
                                           ? RGBA8(255, 60, 60, 255)
                                           : RGBA8(100, 100, 100, 255);

            if (was_pressed(&pad, button_info[i].mask))
            {
                // Handle CROSS button press event
                // You can add any specific action you want to perform here
                cross_color = RGBA8(60, 255, 100, 255); // Change color to green when pressed
            }

            // Draw something (e.g., a simple rectangle)
            vita2d_draw_rectangle(x, y, 36, 36, cross_color);
            vita2d_pgf_draw_text(font, x + 50, y + 20, RGBA8(255, 255, 255, 255), 1.0f, button_info[i].label);
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