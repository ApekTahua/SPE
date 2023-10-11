#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <HX711.h>
#include <SPI.h>
#include <TM1637Display.h>
#include <WiFi.h>
#include <time.h>
// #include <FirebaseESP32.h>
// #include <NTPClient.h>

// UltraSonic
#define TRIG_PIN 14
#define ECHO_PIN 27

// IR
#define IR_PIN 12

// Servo
Servo servo;
#define SERVO_PIN 13

// 7 Segment
#define CLK 22
#define DIO 23
#define TEST_DELAY 200

// HX711
#define LOADCELL_DOUT_PIN 15
#define LOADCELL_SCK_PIN 18

// GPS
#define GPS_RX 16
#define GPS_TX 17
#define GPS_BAUD 115200
TinyGPSPlus gps;

// UltraSonic settings
#define height 100
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

int jarak;
int percentage;
int weight;

long duration;

HX711 scale;
TM1637Display display(CLK, DIO);