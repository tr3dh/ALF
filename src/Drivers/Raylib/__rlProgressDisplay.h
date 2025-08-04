#pragma once

#include <raylib.h>

extern Color g_progressDisplayBackgroundColor;
extern Color g_progressDisplayTextColor;

extern bool windowInitialized;

void displayProgress(float percentProgress, const char *message);