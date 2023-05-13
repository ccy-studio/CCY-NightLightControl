/*
 * @Description:
 * @Author: chenzedeng
 * @Date: 2023-05-06 10:48:38
 * @LastEditTime: 2023-05-13 01:52:42
 */
#define EI_ARDUINO_INTERRUPTED_PIN
#include <Arduino.h>
#include "RCSwitch.h"

#define RF_PIN 2
#define LED_READ_PIN 1
#define LED_WHITE_PIN 3

#define OPEN_CODE_WHITE 2763140
#define PWM_CODE 2763144
#define OPEN_CODE_READ 2763138

#define REPEAT_CLICK_WAIT_TIME 300
#define PWM_SETP 85

RCSwitch mySwitch = RCSwitch();

int pwmValue = PWM_SETP;
bool readOpen = false;
bool whiteOpen = false;
unsigned long lastTime = 0;
unsigned long code;

void setup() {
    // digitalWrite(LED_READ_PIN,LOW);
    // digitalWrite(LED_WHITE_PIN,LOW);
    pinMode(RF_PIN, INPUT_PULLUP);
    mySwitch.enableReceive(RF_PIN);

    pinMode(LED_READ_PIN, FUNCTION_3);
    pinMode(LED_WHITE_PIN, FUNCTION_3);
}

void openOrCloseLed(int ledIndex, int openState) {
    if (openState) {
        // 打开
        if (pwmValue <= 0) {
            pwmValue = PWM_SETP;
        }
        analogWrite(ledIndex, pwmValue);
    } else {
        // 关闭
        analogWrite(ledIndex, 0);
    }
}

void loop() {
    // 防止抖动函数
    delay(10);
    if (mySwitch.available()) {
        if (((micros() - lastTime) / 1000) > REPEAT_CLICK_WAIT_TIME) {
            code = mySwitch.getReceivedValue();
            if (code == OPEN_CODE_WHITE) {
                whiteOpen = !whiteOpen;
                openOrCloseLed(LED_WHITE_PIN, whiteOpen);
            } else if (code == OPEN_CODE_READ) {
                readOpen = !readOpen;
                openOrCloseLed(LED_READ_PIN, readOpen);
            } else if (code == PWM_CODE) {
                pwmValue += PWM_SETP;
                if (pwmValue > 255) {
                    pwmValue = PWM_SETP;
                }
                if (readOpen) {
                    openOrCloseLed(LED_READ_PIN, true);
                }
                delay(1);
                if (whiteOpen) {
                    openOrCloseLed(LED_WHITE_PIN, true);
                }
            }
            lastTime = micros();
        }
        mySwitch.resetAvailable();
    }
}