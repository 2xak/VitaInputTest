#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>
#include <psp2/ctrl.h>
#include <psp2/touch.h>

SceCtrlData handle_input(unsigned int *pressed_buttons);
void handle_touch(SceTouchData *front_touch, SceTouchData *rear_touch);
int is_held(const SceCtrlData *pad, unsigned int button_mask);
int was_pressed(unsigned int pressed_buttons, unsigned int button_mask);


#endif // INPUT_H
