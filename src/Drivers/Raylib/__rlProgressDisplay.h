#pragma once

#include <raylib.h>

extern Color g_progressDisplayBackgroundColor;
extern Color g_progressDisplayTextColor;

void displayProgress(float percentProgress, const char *message);