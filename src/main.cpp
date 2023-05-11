/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-05-06 10:48:38
 * @LastEditTime: 2023-05-06 13:28:21
 */
#include <Arduino.h>

#include <RH_ASK.h>

#define RF_PIN 2
#define LED_READ_PIN 1
#define LED_WHITE_PIN 3

#define OPEN_CODE_WHITE 0xff
#define PWM_CODE 0xff
#define OPEN_CODE_READ 0xff

RH_ASK driver(2000, RF_PIN);

int pwmValue = 0;
int readOpen = 0;
int whiteOpen = 0;

void setup() {
    Serial.begin(9600);
    driver.init();

    pinMode(RF_PIN, INPUT);
    pinMode(LED_READ_PIN, OUTPUT);
    pinMode(LED_WHITE_PIN, OUTPUT);
}

void openOrCloseLed(int ledIndex, int openState) {
    if (openState) {
        // 打开
        if (pwmValue == 0) {
            pwmValue = 255;
        }
        digitalWrite(ledIndex, pwmValue);
    } else {
        // 关闭
        digitalWrite(ledIndex, 0);
    }
}

void loop() {
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);

    if (driver.recv(buf, &buflen))  // Non-blocking
    {
        // Message with a good checksum received, dump it.
        driver.printBuffer("Got:", buf, buflen);
        uint8_t code = buf[0];
        int setp = 85;  // PWM步长
        if (code == OPEN_CODE_WHITE) {
            whiteOpen = !whiteOpen;
            openOrCloseLed(LED_WHITE_PIN, whiteOpen);
        } else if (code == OPEN_CODE_READ) {
            readOpen = !readOpen;
            openOrCloseLed(LED_READ_PIN, readOpen);
        } else if (code == PWM_CODE) {
            pwmValue += setp;
            if (pwmValue > 255) {
                pwmValue = setp;
            }
            if (readOpen || whiteOpen) {
                openOrCloseLed(readOpen ? LED_READ_PIN : LED_WHITE_PIN, 1);
            }
        }
    }
}