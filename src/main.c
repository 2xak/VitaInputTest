#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/display.h>
#include <vita2d.h>

static int running = 1;
static unsigned int prev_buttons = 0;

static void handle_input() {
    SceCtrlData pad;
    sceCtrlPeekBufferPositive(0, &pad, 1);

    unsigned int buttons = pad.buttons;

    if (buttons & SCE_CTRL_CROSS) {
        running = 0; // Exit the application when CROSS button is pressed
    }

    prev_buttons = buttons;
}