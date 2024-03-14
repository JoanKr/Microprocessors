#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

//program służący do zagrania dźwięków o odpowiednich częstotliwościach

// Definicje przycisków
#define BTN_FREQUENCY_INC GPIO_PIN_0  // Przycisk do zwiększania częstotliwości
#define BTN_VOLUME_INC GPIO_PIN_1      // Przycisk do zwiększania głośności
#define BTN_FREQUENCY_DEC GPIO_PIN_2  // Przycisk do zmniejszania częstotliwości
#define BTN_VOLUME_DEC GPIO_PIN_3      // Przycisk do zmniejszania głośności

// Początkowe ustawienia
uint32_t frequency = 1000;  // Początkowa częstotliwość
float volume = 0.5;        // Początkowa głośność

// Funkcja odtwarzająca dźwięk
void playSound(uint32_t frequency, uint32_t duration) {
    uint32_t PWMclock = ROM_SysCtlClockGet() / 4;
    uint32_t period = (PWMclock / frequency) - 1;
    float numOfCyc = ROM_SysCtlClockGet() / 1000.0;
    uint32_t counter = duration * numOfCyc / 3;

    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_1, period);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2, volume * period);
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_2_BIT, true);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_1);
    ROM_SysCtlDelay(counter);
    ROM_PWMGenDisable(PWM1_BASE, PWM_GEN_1);
}

// Funkcje zmieniające parametry dźwięku
void increaseFrequency() {
    frequency += 100;
    if (frequency > 5000) {
        frequency = 5000;
    }
}

void decreaseFrequency() {
    frequency -= 100;
    if (frequency < 100) {
        frequency = 100;
    }
}

void increaseVolume() {
    volume += 0.1;
    if (volume > 1.0) {
        volume = 1.0;
    }
}

void decreaseVolume() {
    volume -= 0.1;
    if (volume < 0.0) {
        volume = 0.0;
    }
}

int main(void) {
    // Konfiguracja zegara
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_4);
    ROM_SysCtlDelay(ROM_SysCtlClockGet() / 2);

    // Włączenie portów GPIO i PWM
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);

    // Konfiguracja generatora PWM
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN);

    // Konfiguracja pinu PA6 jako wyjścia PWM
    ROM_GPIOPinConfigure(GPIO_PA6_M1PWM2);
    ROM_GPIOPinTypePWM(GPIO_PORTA_BASE, GPIO_PIN_6);

    // Konfiguracja przycisków
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, BTN_FREQUENCY_INC | BTN_VOLUME_INC | BTN_FREQUENCY_DEC | BTN_VOLUME_DEC);
    ROM_GPIOPadConfigSet(GPIO_PORTH_BASE, BTN_FREQUENCY_INC | BTN_VOLUME_INC | BTN_FREQUENCY_DEC | BTN_VOLUME_DEC, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    for (;;) {
        // Sprawdzenie stanu przycisków i zmiana parametrów dźwięku
        if (ROM_GPIOPinRead(GPIO_PORTH_BASE, BTN_FREQUENCY_INC) == 0) {
            increaseFrequency();
            ROM_SysCtlDelay(ROM_SysCtlClockGet() / 10);
        }
        if (ROM_GPIOPinRead(GPIO_PORTH_BASE, BTN_FREQUENCY_DEC) == 0) {
            decreaseFrequency();
            ROM_SysCtlDelay(ROM_SysCtlClockGet() / 10);
        }
        if (ROM_GPIOPinRead(GPIO_PORTH_BASE, BTN_VOLUME_INC) == 0) {
            increaseVolume();
            ROM_SysCtlDelay(ROM_SysCtlClockGet() / 10);
        }
        if (ROM_GPIOPinRead(GPIO_PORTH_BASE, BTN_VOLUME_DEC) == 0) {
            decreaseVolume();
            ROM_SysCtlDelay(ROM_SysCtlClockGet() / 10);
        }

        // Odtwarzanie dźwięku i opóźnienie
        playSound(frequency, 300);
        ROM_SysCtlDelay(ROM_SysCtlClockGet() / 2);
    }

    return 0;
}
