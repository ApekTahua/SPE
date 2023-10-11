#include "header.h"

void ultrasonic()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    jarak = duration * SOUND_SPEED / 2;

    percentage = (height - jarak) * 100 / height;
}

void Open_Bin()
{
    if (digitalRead(IR_PIN) == HIGH)
    {
        servo.write(180);
    }

    for (int i = 180; i >= 0; i--)
    {
        servo.write(i);
        delay(3);
    }
}

void Seven_Segment()
{
    int k;
    uint8_t data[] = {0xff, 0xff, 0xff, 0xff};
    uint8_t blank[] = {0x00, 0x00, 0x00, 0x00};
    display.setBrightness(0x0f);

    // All segments on
    display.setSegments(data);
    delay(TEST_DELAY);

    // Selectively set different digits
    data[0] = display.encodeDigit(0);
    data[1] = display.encodeDigit(1);
    data[2] = display.encodeDigit(2);
    data[3] = display.encodeDigit(3);
    display.setSegments(data);
    delay(TEST_DELAY);

    /*
    for(k = 3; k >= 0; k--) {
    display.setSegments(data, 1, k);
    delay(TEST_DELAY);
    }
    */

    display.clear();
    display.setSegments(data + 2, 2, 2);
    delay(TEST_DELAY);

    display.clear();
    display.setSegments(data + 2, 2, 1);
    delay(TEST_DELAY);

    display.clear();
    display.setSegments(data + 1, 3, 1);
    delay(TEST_DELAY);

    // Show decimal numbers with/without leading zeros
    display.showNumberDec(0, false); // Expect: ___0
    delay(TEST_DELAY);
    display.showNumberDec(0, true); // Expect: 0000
    delay(TEST_DELAY);
    display.showNumberDec(1, false); // Expect: ___1
    delay(TEST_DELAY);
    display.showNumberDec(1, true); // Expect: 0001
    delay(TEST_DELAY);
    display.showNumberDec(301, false); // Expect: _301
    delay(TEST_DELAY);
    display.showNumberDec(301, true); // Expect: 0301
    delay(TEST_DELAY);
    display.clear();
    display.showNumberDec(14, false, 2, 1); // Expect: _14_
    delay(TEST_DELAY);
    display.clear();
    display.showNumberDec(4, true, 2, 2); // Expect: __04
    delay(TEST_DELAY);
    display.showNumberDec(-1, false); // Expect: __-1
    delay(TEST_DELAY);
    display.showNumberDec(-12); // Expect: _-12
    delay(TEST_DELAY);
    display.showNumberDec(-999); // Expect: -999
    delay(TEST_DELAY);
    display.clear();

    // Brightness Test
    for (k = 0; k < 4; k++)
        data[k] = 0xff;
    for (k = 0; k < 7; k++)
    {
        display.setBrightness(k);
        display.setSegments(data);
        delay(TEST_DELAY);
    }
}

void GPS()
{
    if (Serial2.available() > 0)
    {
        if (gps.encode(Serial2.read()))
        {
            if (gps.location.isValid())
            {
                Serial.print("Latitude: ");
                Serial.println(gps.location.lat(), 6);
                Serial.print("Longitude: ");
                Serial.println(gps.location.lng(), 6);
            }
        }
    }
    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
        Serial.println(F("No GPS detected: check wiring."));
        while (true)
            ;
    }
}

void Print_Status()
{
    weight = scale.get_units();

    // float lastReading;

    if (scale.is_ready())
    {
        Serial.print("Weight: ");
        Serial.print(weight);
        Serial.println(" g");

        Serial.print("Distance Ultra: ");
        Serial.print(jarak);
        Serial.println(" cm");

        Serial.print("Percentage: ");
        Serial.print(percentage);
        Serial.println(" %");

        GPS();
    }
    else
    {
        Serial.println("HX711 Hilang WOEEEEE ILANGGG");
    }
    scale.power_down(); // put the ADC in sleep mode
    delay(100);
    scale.power_up();

    Serial.println();
}

void setup()
{
    Serial.begin(115200);

    // UltraSonic
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // IR
    pinMode(IR_PIN, INPUT);

    // Servo
    pinMode(SERVO_PIN, OUTPUT);
    servo.attach(SERVO_PIN);
    servo.write(0); // Close the bin

    // HX711
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale();
    scale.tare();

    // GPS
    Serial2.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
}

void loop()
{
    ultrasonic();
    Seven_Segment();
    Print_Status();
    delay(1000);
}
