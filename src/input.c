#include "input.h"

static unsigned int prev_buttons = 0;

SceCtrlData handle_input(unsigned int *pressed_buttons)
{
    SceCtrlData pad = {0};
    sceCtrlPeekBufferPositive(0, &pad, 1);

    if (pressed_buttons != NULL)
    {
        *pressed_buttons = pad.buttons & ~prev_buttons; // Get newly pressed buttons
    }
    
    /*
    if (is_held(&pad, SCE_CTRL_CROSS))
    {
        running = 0; // Exit the application when CROSS button is pressed
    }
    */

    prev_buttons = pad.buttons;
    return pad;
}

int is_held(const SceCtrlData *pad, unsigned int button_mask)
{
    return (pad->buttons & button_mask) != 0;
}

int was_pressed(unsigned int pressed_buttons, unsigned int button_mask)
{
    return (pressed_buttons & button_mask) != 0;
}