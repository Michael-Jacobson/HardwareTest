#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BACKLIGHT_PATH  "/sys/class/backlight/backlight/"
#define BACKLIGHT_MAX_FILE  "max_brightness"
#define BACKLIGHT_CURRENT_BRIGHTNESS "brightness"

char SetBacklightValue(int percent);

char MaxBacklightBrightness = 0;

/****************************************************
 *
 */
char InitBacklight(void)
{
    int file = 0;
    char fullPath[100];
    char valueString[11];
    char retVal = 0;

    memset(fullPath, 0, sizeof(fullPath));
    memset(valueString, 0, sizeof(valueString));

    strcpy(fullPath, BACKLIGHT_PATH);
    strcat(fullPath, BACKLIGHT_MAX_FILE);

    file = open(fullPath, O_RDONLY);

    if(file != 0)
    {
        read(file, valueString, sizeof(valueString));
        MaxBacklightBrightness = atol(valueString);

        close(file);
        retVal++;

        if(MaxBacklightBrightness > 0)
        {
            retVal += SetBacklightValue(100);
        }
    }
    else
    {
    	printf(" Failed to open Backlight\n");
    }

    return retVal;
}

char SetBacklightValue(int percent)
{
    int file = 0;
    char fullPath[100];
    char valueString[11];
    char retVal = 0;

    memset(fullPath, 0, sizeof(fullPath));
    memset(valueString, 0, sizeof(valueString));

    strcpy(fullPath, BACKLIGHT_PATH);
    strcat(fullPath, BACKLIGHT_CURRENT_BRIGHTNESS);

    file = open(fullPath, O_WRONLY);

    if(file != 0)
    {
        sprintf(valueString, "%d", ((MaxBacklightBrightness*percent)/100));
        write(file, valueString, strlen(valueString));
        close(file);
        retVal++;
    }

    return retVal;
}
