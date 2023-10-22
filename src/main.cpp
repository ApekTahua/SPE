#include "header.h"

void ultrasonic()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
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
    else
    {
        servo.write(0);
    }
}

void Seven_Segment()
{
    ultrasonic();
    if (!isnan(weight) && !isnan(percentage))
    {
        w_MSD = int(kg) / 10;
        w_CSD = int(kg) % 10;
        w_XSD = (kg - int(kg)) * 10;
        w_LSD = int(kg * 100) % 10;

        disp.Number(8, w_MSD);
        disp.Numberdp(7, w_CSD);
        disp.Number(6, w_XSD);
        disp.Number(5, w_LSD);

        per_MSD = int(percentage) / 10;
        per_LSD = int(percentage) % 10;

        disp.Number(3, per_MSD);
        disp.Number(2, per_LSD); // Show the percentage with one decimal place
    }
    else
    {
        disp.Clear();
    }
}
// }

void GPS()
{
    if (ss.available() > 0)
    {
        gps.encode(ss.read());
        if (gps.location.isValid())
        {
            latitude = (gps.location.lat());
            longitude = (gps.location.lng());
        }
        else
        {
            latitude = 0.0;
            longitude = 0.0;
        }
    }
    else
    {
        Serial.println("GPS Not Available");
    }
}

void printStatus()
{
    delay(5000);
    weight = scale.get_units();
    kg = weight / 1000;
    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.println(" g");

    Serial.print("Weight: ");
    Serial.print(kg);
    Serial.println(" kg");

    Serial.print("Percentage: ");
    Serial.print(percentage);
    Serial.println(" %");

    Serial.print("Distance: ");
    Serial.print(jarak);
    Serial.println(" cm");

    Serial.print("Latitude: ");
    Serial.println(latitude, 6);
    Serial.print("Longitude: ");
    Serial.println(longitude, 6);

    Serial.println();
}

void taskGetSensorReading(void *parameter)
{
    while (1)
    {
        ultrasonic();
        GPS();
        printStatus();
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void taskShowSegment(void *parameter)
{
    while (1)
    {
        Seven_Segment();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void setup()
{
    Serial.begin(115200);

    // //Relay
    // pinMode(RELAY, OUTPUT);
    // digitalWrite(RELAY, HIGH);

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
    scale.set_scale(18.51);
    scale.tare();
    disp.Initialize(8);
    disp.Clear();
    Serial.println("Setup done");

    // // GPS
    // Serial2.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);

    xTaskCreate(taskGetSensorReading, "Serial", configMINIMAL_STACK_SIZE + 10240, NULL, 5, NULL);
    xTaskCreate(taskShowSegment, "Segment", configMINIMAL_STACK_SIZE + 10240, NULL, 5, NULL);
}

void loop()
{
    Open_Bin();
}
