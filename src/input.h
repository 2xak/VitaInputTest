#ifdef INPUT_H
#define INPUT_H

#include <psp2/ctrl.h>

SceCtrlData handle_input(unsigned int *pressed_buttons);
int is_held(const SceCtrlData *pad, unsigned int button_mask);
int was_pressed(const SceCtrlData *pad, unsigned int button_mask);


#endif // INPUT_H