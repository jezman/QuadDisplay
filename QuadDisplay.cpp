#include "QuadDisplay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "bcm2835.h"

#define PIN RPI_V2_GPIO_P1_13

#define max(x, y) x>y ? x : y

#define MACRO_DIGITAL_WRITE(a,b) (bcm2835_gpio_write(a,b))

const static uint8_t numerals[] = {QD_0, QD_1, QD_2, QD_3, QD_4, QD_5, QD_6, QD_7, QD_8, QD_9};

typedef unsigned char byte;


int main(int argc, char **argv, char **envp) {

        if (!bcm2835_init()) exit(EXIT_FAILURE);

        bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);

        if(argc!=2)
        {
          displayClear(PIN);
          return (EXIT_SUCCESS);
        }
        displayTemperatureC(PIN, atoi(argv[1]));

        bcm2835_close();
        return (EXIT_SUCCESS);
}

static void sendByte(uint8_t pin, byte data)
{
    for (byte i = 8; i > 0; i--) {
        if (data & 1) {
            //noInterrupts();
            MACRO_DIGITAL_WRITE(pin, LOW);
            MACRO_DIGITAL_WRITE(pin, HIGH);
            //interrupts();
            delayMicroseconds(30);
        }
        else {
            MACRO_DIGITAL_WRITE(pin, LOW);
            delayMicroseconds(15);
            MACRO_DIGITAL_WRITE(pin, HIGH);
            delayMicroseconds(60);
        }
        data >>= 1;
    }
}

static void latch(uint8_t pin)
{
    MACRO_DIGITAL_WRITE(pin, LOW);
    delayMicroseconds(100);
    MACRO_DIGITAL_WRITE(pin, HIGH);
    delayMicroseconds(300);
}
void displayDigits(uint8_t pin, uint8_t digit1, uint8_t digit2, uint8_t digit3, uint8_t digit4)
{
    //pinMode(pin, OUTPUT);
    //turnOffPWM work here:
    MACRO_DIGITAL_WRITE(pin, HIGH);
    sendByte(pin, digit1);
    sendByte(pin, digit2);
    sendByte(pin, digit3);
    sendByte(pin, digit4);
    latch(pin);
}

void displayClear(uint8_t pin)
{
    displayDigits(pin, QD_NONE, QD_NONE, QD_NONE, QD_NONE);
}

void displayTemperatureC(uint8_t pin, int val)
{
    int padZeros=0;
    uint8_t digits[4] = {0xff, 0xff, QD_DEGREE, QD_C};

    if (!padZeros && !val)
        digits[1] = numerals[0];
    else {
        int negative = val < 0;
        val = abs(val);

        int8_t i;
        for (i = 2; i--; ) {
            uint8_t digit = val % 10;
            digits[i] = (val || padZeros) ? numerals[digit] : 0xff;

            val /= 10;
            if (!val && !padZeros)
                break;
        }

        if (negative)
            digits[max(0, i-1)] = QD_MINUS;
    }
    displayDigits(pin, digits[0], digits[1], digits[2], digits[3]);
}
