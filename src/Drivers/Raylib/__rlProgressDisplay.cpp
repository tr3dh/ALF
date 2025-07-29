#include "__rlProgressDisplay.h"

// Funktion funktioniert nur wenn Fenster initialisiert worden ist
void displayProgress(float percentProgress, const char *message)
{
    // Fenstergröße dynamisch holen
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    BeginDrawing();
    ClearBackground(g_progressDisplayBackgroundColor);

    // Textanzeige
    DrawText(message, screenWidth/2 - MeasureText(message, 20)/2, screenHeight/2 - 60, 20, g_progressDisplayTextColor);

    // Fortschrittsbalken
    int barWidth = 400;
    int barHeight = 15;
    int barX = screenWidth/2 - barWidth/2;
    int barY = screenHeight/2;

    //
    DrawRectangle(barX, barY, barWidth, barHeight, LIGHTGRAY);

    //
    int filledWidth = (int)(barWidth * percentProgress);
    DrawRectangle(barX, barY, filledWidth, barHeight, GREEN);

    // so wird umrandung über balken gezeichnet, sodass Fortschrittsbalken geschlossen erscheint
    DrawRectangleLines(barX, barY, barWidth, barHeight, DARKGRAY);

    EndDrawing();

    // Pause um Raylib Zeit zum Rendern zu geben, wenn updates sehr schnell aufeinander folgen kann es sonst zu
    // komplikationen kommen
    WaitTime(0.01);
}