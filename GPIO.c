/*
main.c
*/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include <stdio.h>

#define GPIO_PINS_ALL GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7

int main(void)
{
    uint8_t a = 0;
    uint8_t b = 0;

    // Ustawienie zegara mikrokontrolera na bezpośrednią pracę z kryształu.
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_20 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

    // Włączenie obsługi GPIO dla portów A, B, E, F, G, H, J.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    // Ustawienie wszystkich pinów GPIO na portach A, B, E, F, G jako wyjścia.
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PINS_ALL);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PINS_ALL);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PINS_ALL);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PINS_ALL);
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PINS_ALL);

    // Ustawienie wszystkich pinów GPIO na porcie J jako wejścia.
    GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, GPIO_PIN_7);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_7);

    for (;;)
    {
        // Sprawdzenie naciśnięcia przycisku i zapisanie danych na porcie.
        if (GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_7) == GPIO_PIN_7)
            GPIOPinWrite(GPIO_PORTA_BASE, 0xFF, ++a);

        if (GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_7) == GPIO_PIN_7)
            GPIOPinWrite(GPIO_PORTE_BASE, 0xFF, ++b);

        // Wyświetlenie pewnych wzorców na diodach LED.
        GPIOPinWrite(GPIO_PORTG_BASE, 0xFF, a - b);

        uint8_t a_pom = a;
        uint8_t b_pom = b;
        uint8_t nwd;
        uint8_t nww;

        if (a == 0 || b == 0)
        {
            nwd = 1;
        }

        while (b_pom != 0)
        {
            uint8_t pom = b_pom;
            b_pom = a_pom % b_pom;
            a_pom = pom;
        }

        nwd = a_pom;
        nww = (a * b) / nwd;


	// Ustawienie wartości na porcie B na największy wspólny dzielnik 	//(NWD) i na porcie F na najmniejszą wspólną wielokrotność (NWW).

        GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, nwd);
        GPIOPinWrite(GPIO_PORTF_BASE, 0xFF, nww);

        // Opóźnienie, aby zmiany były widoczne.
        SysCtlDelay(SysCtlClockGet() / 2);
    }

    return 0;
}
