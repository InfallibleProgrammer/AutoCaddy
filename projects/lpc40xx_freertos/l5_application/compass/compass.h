#pragma once

#include <stdbool.h>

void compass__init(void);
void compass__run_once(void);
float compass__run_once_get_heading(void);
