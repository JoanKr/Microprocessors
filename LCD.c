#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "grlib/grlib.h"
#include "drivers/ili9341_240x320x262K.h"
#include "utils/uartstdio.h"
#include "driverlib/gpio.h"


//generowanie prostokąta z gradientem

// Logo Texas Instruments
const unsigned char TI_logo[(16 * 2) + 5] = {
    IMAGE_FMT_1BPP_UNCOMP, 16, 0, 16, 0,
    0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xf3, 0x0f, 0xfa, 0x0f, 0x8f, 0x0f, 0x89, 0x81, 0x99, 0x81, 0x19,
    0x03, 0x09, 0x07, 0x89, 0x07, 0xdf, 0x3f, 0xee, 0x7f, 0xf0, 0xff, 0xf8, 0xff, 0xf9, 0xff, 0xfb
};

// Funkcja główna
int main(void)
{
    tContext sContext;
    tRectangle sRect;

    // Włączanie leniwego stosowania dla obsługi przerwań. Pozwala to na korzystanie z instrukcji zmiennoprzecinkowych wewnątrz obsługiwaczy przerwań,
    // ale kosztem dodatkowego użycia stosu.
    ROM_FPULazyStackingEnable();

    // Ustawienie zegara tak, aby działał bezpośrednio z kwarcu.
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    // Inicjalizacja sterownika wyświetlacza.
    ILI9341_240x320x262K_Init();

    // Inicjalizacja kontekstu graficznego.
    GrContextInit(&sContext, &g_sILI9341_240x320x262K);

    // Wczytanie loga Texas Instruments
    GrImageDraw(&sContext, TI_logo, 0, 0);

    // Wymuszanie natychmiastowego rysowania.
    GrFlush(&sContext);

    unsigned int i;

    uint32_t *color = 16777215;
    char *text = "Kwadrat";
    tFont *font = g_psFontCm20b;

    // Rysowanie linii o zmieniającym się kolorze
    for (i = 0; i < 160; i++) {

        //zmieniamy kolor
        GrContextForegroundSet(&sContext, 0x002500A1 + i*256);

        //Rysujemy nowe linie
        GrLineDraw(&sContext, (GrContextDpyWidthGet(&sContext) / 4) + i, (GrContextDpyHeightGet(&sContext) / 4), (GrContextDpyWidthGet(&sContext) / 4) + i, (GrContextDpyHeightGet(&sContext) / 4) + 120);
    }
    GrFlush(&sContext);

    unsigned int changeColor = 0;
    for (i = 0; i < 4; i++) {
        GrContextForegroundSet(&sContext, 0x002500A1 + changeColor);
        GrContextFontSet(&sContext, font);
        GrStringDrawCentered(&sContext, text, -1, (GrContextDpyWidthGet(&sContext) / 2) - i, (GrContextDpyHeightGet(&sContext) / 2), 0);
        changeColor += 256;
    }

    GrFlush(&sContext);

    while (1)
    {
        // Odczekanie przez pewien czas (dla celów demonstracyjnych)
        ROM_SysCtlDelay(ROM_SysCtlClockGet() / 2);
    }
}
