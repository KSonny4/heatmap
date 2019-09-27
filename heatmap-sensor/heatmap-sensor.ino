#include <Arduino.h>
#include <RF24.h>
#include <SPI.h>
#include "printf.h"
#include "DHT.h"

#define HM_PIN_CE 10
#define HM_PIN_CSN 9
#define HM_PIN_LED_RED 4
#define HM_PIN_LED_GREEN 6
#define HM_PIN_BUZZER 3
#define HM_DHTPIN 2

#define BUZZER_ENABLED true

RF24 radio(HM_PIN_CE, HM_PIN_CSN);
DHT dht(HM_DHTPIN, DHT21);

const byte address[6] = "1tran"; // TODO configurable

void beep(int length) {
    if (BUZZER_ENABLED) digitalWrite(HM_PIN_BUZZER, HIGH);
    delay(length);
    if (BUZZER_ENABLED) digitalWrite(HM_PIN_BUZZER, LOW);
}

void beepAndBlinkRed(int length) {
    if (BUZZER_ENABLED) digitalWrite(HM_PIN_BUZZER, HIGH);
    digitalWrite(HM_PIN_LED_RED, HIGH);
    delay(length);
    if (BUZZER_ENABLED) digitalWrite(HM_PIN_BUZZER, LOW);
    digitalWrite(HM_PIN_LED_RED, LOW);
}

void beepAndBlinkGreen(int length) {
    if (BUZZER_ENABLED) digitalWrite(HM_PIN_BUZZER, HIGH);
    digitalWrite(HM_PIN_LED_GREEN, HIGH);
    delay(length);
    if (BUZZER_ENABLED) digitalWrite(HM_PIN_BUZZER, LOW);
    digitalWrite(HM_PIN_LED_GREEN, LOW);
    delay(10);
}

void initRadio() {
    if (!radio.begin()) {
        Serial.println("Could not initialize radio!!!");
        for (int i = 0; i < 5; i++) {
            beep(1000);
            delay(200);
        }
    } else Serial.println("Radio initialized");
}

void setup() {
    Serial.begin(9600);
    Serial.println("\n---\nStarting...\n");
    dht.begin();
    printf_begin();
    delay(100);

    pinMode(HM_PIN_BUZZER, OUTPUT);
    pinMode(HM_PIN_LED_RED, OUTPUT);
    pinMode(HM_PIN_LED_GREEN, OUTPUT);

    initRadio();
    radio.setAutoAck(true);
    radio.enableAckPayload();
    radio.enableDynamicPayloads();
    radio.setDataRate(RF24_250KBPS);
    radio.setRetries(15, 15);
    radio.setPALevel(RF24_PA_MAX);
    radio.setChannel(109);
    radio.openWritingPipe(address);
    radio.stopListening();
    radio.printDetails();
}

void loop() {
    //load data from sensor
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT");
    } else {
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" *C");

        // temperature to char arr
        char temperature_string[5];
        dtostrf(temperature, 2, 1, temperature_string);

        // humidity to char arr
        char humidity_string[5];
        dtostrf(humidity, 2, 1, humidity_string);

        // sensor to be of co2
        int co2 = 9999; // TODO measure

        char concatenated_string[25];
        sprintf(concatenated_string, "%s %s %d", temperature_string, humidity_string, co2);

        Serial.print("Sending: ");
        Serial.println(concatenated_string);

        if (radio.write(concatenated_string, sizeof(concatenated_string))) {
            delay(1000);
        } else {
            Serial.println("Could not sent data");
            beepAndBlinkRed(1000);
        }
    }
}
