#include <Arduino.h>
#include <bsec.h>
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

#define ADC_U_BATT A0
#define NCHG_BATT D6

const uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_3s_4d/bsec_iaq.txt"
};

SSD1306Wire display(0x3c, SDA, SCL);  
Bsec iaqSensor;

String output1;
double Old_U_Batt = 0;
bool Charging = false;

void setup() {
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);

    iaqSensor.setConfig(bsec_config_iaq);
    Wire.begin();

    iaqSensor.begin(BME680_I2C_ADDR_PRIMARY, Wire);

    bsec_virtual_sensor_t sensorList[10] = {
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_STATIC_IAQ,
        BSEC_OUTPUT_CO2_EQUIVALENT,
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    };

    iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);

    pinMode(NCHG_BATT, INPUT);
}

void loop() {
    if (iaqSensor.run()) 
    { 
        display.clear();
        display.setFont(ArialMT_Plain_16);
        display.setTextAlignment(TEXT_ALIGN_LEFT);

        double U_Batt = (((double)analogRead(ADC_U_BATT)/1023.0)*6.87167);

        output1 = String(iaqSensor.temperature);
        output1 += "°C";
        output1 += ", " + String(iaqSensor.humidity);
        output1 += "%, ";
        output1 += String(iaqSensor.iaq);
        output1 += ", " + String(iaqSensor.iaqAccuracy);
        output1 += String(U_Batt);
        output1 += "V ";


        if (((U_Batt + 0.05) < Old_U_Batt) || (digitalRead(NCHG_BATT)))
        {
            Charging = false;
        }
        else if ((U_Batt > (Old_U_Batt + 0.1)) && (!digitalRead(NCHG_BATT)))
        {
            Charging = true;
        }
        Old_U_Batt = U_Batt;

        if (Charging)
        {
            output1 += "CHR";
        }
        else 
        {
            output1 += "NCHR";
        }


        display.drawStringMaxWidth(0, 0, 128, output1);
        display.display();
    } 
    else {}
}
