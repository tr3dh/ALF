#pragma once

#include <raylib.h>

static Color g_progressDisplayBackgroundColor = GRAY;
static Color g_progressDisplayTextColor = RED;

void displayProgress(float percentProgress, const char *message);