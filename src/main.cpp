#include <Arduino.h>
#include <bsec.h>
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

const uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_3s_4d/bsec_iaq.txt"
};

SSD1306Wire display(0x3c, SDA, SCL);  
Bsec iaqSensor;

String output1;
String output2;

void setup() {
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);

    iaqSensor.setConfig(bsec_config_iaq);
    Wire.begin();

    iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire);

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
}

void loop() {
    if (iaqSensor.run()) 
    { 
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_LEFT);

        output1 = String(millis());
        output1 += ", " + String(iaqSensor.pressure);
        output1 += "hPa";
        output1 += ", " + String(iaqSensor.gasResistance);
        output1 += "Ohm";
        output1 += ", " + String(iaqSensor.temperature);
        output1 += "°C";
        output1 += ", " + String(iaqSensor.humidity);
        output1 += "%";

        output2 = String(iaqSensor.iaq);
        output2 += "iaq";
        output2 += ", " + String(iaqSensor.iaqAccuracy);
        output2 += ", " + String(iaqSensor.staticIaq);
        output2 += "siaq";
        output2 += ", " + String(iaqSensor.co2Equivalent);
        output2 += "ppm";
        output2 += ", " + String(iaqSensor.breathVocEquivalent);
        output2 += "ppm";

        display.drawStringMaxWidth(0, 0, 128, output1);
        display.drawStringMaxWidth(0, 39, 128, output2);
        display.display();
    } 
    else {}
}
