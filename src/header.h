#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <HX711.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <MAX7219_7Seg_Disp.h>
// #include <FirebaseESP32.h>
// #include <NTPClient.h>

// UltraSonic
#define TRIG_PIN 14
#define ECHO_PIN 27

// IR
#define IR_PIN 32

// Servo
Servo servo;
#define SERVO_PIN 33

// 7-Segment
MAX7219_7Seg_Disp disp(23, 5, 18);

// HX711
#define LOADCELL_DOUT_PIN 15
#define LOADCELL_SCK_PIN 22
// float calib = 18.51;

// GPS
#define GPS_RX 16
#define GPS_TX 17
#define GPS_BAUD 115200
TinyGPSPlus gps;
double latitude;
double longitude;
SoftwareSerial ss(GPS_RX, GPS_TX);

// Relay
#define RELAY 26

// UltraSonic settings
#define height 100
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

int jarak;
int percentage;
float weight;
float kg;
int w_LSD, w_CSD, w_MSD, w_XSD, per_LSD, per_MSD, per_fraction;
long duration;

HX711 scale;

// Semaphore
SemaphoreHandle_t ultrasonicSemaphore;
SemaphoreHandle_t openBinSemaphore;
SemaphoreHandle_t printStatusSemaphore;

volatile bool updateDisplayFlag;