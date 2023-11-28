#include "header.h"

void ultrasonic()
{
    if (xSemaphoreTake(ultrasonicSemaphore, portMAX_DELAY))
    {
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);

        duration = pulseIn(ECHO_PIN, HIGH);
        jarak = duration * SOUND_SPEED / 2;

        percentage = (height - jarak) * 100 / height;

        xSemaphoreGive(ultrasonicSemaphore);
    }
}

void Open_Bin()
{
    if (xSemaphoreTake(openBinSemaphore, portMAX_DELAY))
    {
        if (digitalRead(IR_PIN) == HIGH)
        {
            servo.write(180);
            updateDisplayFlag = true;
        }
        else
        {
            servo.write(0);
            updateDisplayFlag = true;
        }

        xSemaphoreGive(openBinSemaphore);
    }
}

void Seven_Segment()
{
    if (updateDisplayFlag)
    {
        if (!isnan(weight) && !isnan(percentage))
        {
            int w_MSD = int(kg) / 10;
            int w_CSD = int(kg) % 10;
            int w_XSD = (kg - int(kg)) * 10;
            int w_LSD = int(kg * 100) % 10;

            disp.Number(8, w_MSD);
            disp.Numberdp(7, w_CSD);
            disp.Number(6, w_XSD);
            disp.Number(5, w_LSD);

            int per_MSD = int(percentage) / 10;
            int per_LSD = int(percentage) % 10;

            disp.Number(3, per_MSD);
            disp.Number(2, per_LSD);

            updateDisplayFlag = true;
        }
        else
        {
            disp.Clear();
        }
    }
}

void GPS()
{
    if (ss.available() > 0)
    {
        gps.encode(ss.read());
        if (gps.location.isValid())
        {
            latitude = gps.location.lat();
            longitude = gps.location.lng();
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
    if (xSemaphoreTake(printStatusSemaphore, portMAX_DELAY))
    {
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

        xSemaphoreGive(printStatusSemaphore);
    }
}

void taskGetSensorReading(void *parameter)
{
    while (1)
    {
        ultrasonic();
        GPS();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void taskShowSegment(void *parameter)
{
    while (1)
    {
        Seven_Segment();
    }
}

void taskPrintStatus(void *parameter)
{
    while (1)
    {
        printStatus();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void taskOpenBin(void *parameter)
{
    while (1)
    {
        Open_Bin();
        vTaskDelay(pdMS_TO_TICKS(100)); // Adjust the delay as needed
    }
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
    scale.set_scale(18.51);
    scale.tare();
    disp.Initialize(8);
    disp.Clear();
    Serial.println("Setup done");

    ultrasonicSemaphore = xSemaphoreCreateMutex();
    openBinSemaphore = xSemaphoreCreateMutex();
    printStatusSemaphore = xSemaphoreCreateMutex();

    xTaskCreate(taskOpenBin, "OpenBin", configMINIMAL_STACK_SIZE + 10240, NULL, 3, NULL);
    xTaskCreate(taskGetSensorReading, "SensorReading", configMINIMAL_STACK_SIZE + 10240, NULL, 2, NULL);
    xTaskCreate(taskShowSegment, "ShowSegment", configMINIMAL_STACK_SIZE + 10240, NULL, 4, NULL);
    xTaskCreate(taskPrintStatus, "PrintStatus", configMINIMAL_STACK_SIZE + 10240, NULL, 1, NULL);
}

void loop()
{
    // This loop can be empty since tasks are handling the functionality
}
