// TPMS - tire pressure monitoring system, using cheap tire pressure sensors from AliExpress and the Seeed XIAO ESP32S3 board with OLED display

// created by upir, 2025
// youtube channel: https://www.youtube.com/upir_upir

// YouTube Video: https://youtu.be/P85tkCbQGo8
// Source Files: https://github.com/upiir/arduino_tpms_tire_pressure
// WOKWI sketch: https://wokwi.com/projects/437973476984561665

// Links from the video:
// Do you like this video? You can buy me a coffee ☕: https://www.buymeacoffee.com/upir
// Tire Pressure Sensors: https://s.click.aliexpress.com/e/_opKdOsF
// Starting wokwi project (Dasai Mochi): https://wokwi.com/projects/426385808890236929
// Seeed Studio XIAO ESP32S3 // https://s.click.aliexpress.com/e/_om3a6cb
// Seeed Xiao Expansion Board // https://s.click.aliexpress.com/e/_omUcACF
// Seeed Xiao ESP32S3 documentation: https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/
// 128x64 SSD1306 OLED Display 1.54": https://s.click.aliexpress.com/e/_DCYdWXb
// 128x64 SSD1306 OLED Display 0.96": https://s.click.aliexpress.com/e/_DCKdvnh
// 128x64 SSD1306 OLED Display 2.42": https://s.click.aliexpress.com/e/_DFdMoTh
// Transparent OLED Display: https://s.click.aliexpress.com/e/_opEzcuL
// U8g2 library documentation: https://github.com/olikraus/u8g2/wiki/u8g2reference
// Large Syringe: https://s.click.aliexpress.com/e/_omaqiwB
// BLE TPMS info: https://github.com/ra6070/BLE-TPMS
// BLE TPMS another info: https://github.com/andi38/TPMS
// Get BLE name: https://stackoverflow.com/questions/55735377/esp32-missing-names-during-ble-scan
// Dasai Mochi video: https://www.youtube.com/watch?v=QOoszpg0BsM&ab_channel=upir
// Lopaka Project: https://lopaka.app/editor/11628/24513

// Related videos with displays and cars:
// Dasai Mochi: https://youtu.be/QOoszpg0BsM
// Car VFD Gauges: https://youtu.be/Frq_JP1lalE
// Mustang Gear Indicator: https://youtu.be/qqmy5ZxKyq4
// Arduino Dashboard: https://youtu.be/L3ufJTFX2v0
// DIGIFIZ: https://youtu.be/XtSo1BaJ6Bg
// Arduino Temperature Gauge: https://youtu.be/A00CvNi1rzQ
// Ford RS gauges with Arduino UNO: https://youtu.be/ugS3rKBZVVc
// Inclinometer: https://youtu.be/GosqWcScwC0
// BOOST GAUGE: https://youtu.be/cZTx7T9uwA4
// Shifter display: https://youtu.be/pbqgrv5YSf0
// Get gear: https://youtu.be/QixtxaAda18
// SimHub gear indicator: https://youtu.be/JbVqXZngXAY
// Arduino Parking Sensor: https://youtu.be/gg08H-6Z1Lo
// OLED Gauge: https://youtu.be/xI6dXTA02UQ
// GPS Speedometer: https://youtu.be/dSD02o3M2sw
// Turbo pressure gauge: https://youtu.be/JXmw1xOlBdk

// addresses of individual sensors
// F.L >> 0A -  4a:20:00:00:22:2b
// F.R >> 1B -  4a:6c:00:00:9c:13
// R.L >> 2C -  4a:23:00:00:5f:00
// R.R >> 3D -  4a:24:00:00:8e:5b


#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <U8g2lib.h> // library for drawing images to the OLED display

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // initialization for the used OLED display

int scanTime = 5;  //In seconds
BLEScan *pBLEScan;

int front_left_updated; // was this sensor updated
float front_left_voltage; // sensor battery voltage in V
int front_left_temperature; // sensor temperature in °C
uint16_t front_left_pressure_psi_x10; // sensor pressure psi * 10
float front_left_pressure_psi; // sensor pressure PSI

int front_right_updated; // was this sensor updated
float front_right_voltage; // sensor battery voltage in V
int front_right_temperature; // sensor temperature in °C
uint16_t front_right_pressure_psi_x10; // sensor pressure psi * 10
float front_right_pressure_psi; // sensor pressure PSI

int rear_left_updated; // was this sensor updated
float rear_left_voltage; // sensor battery voltage in V
int rear_left_temperature; // sensor temperature in °C
uint16_t rear_left_pressure_psi_x10; // sensor pressure psi * 10
float rear_left_pressure_psi; // sensor pressure PSI

int rear_right_updated; // was this sensor updated
float rear_right_voltage; // sensor battery voltage in V
int rear_right_temperature; // sensor temperature in °C
uint16_t rear_right_pressure_psi_x10; // sensor pressure psi * 10
float rear_right_pressure_psi; // sensor pressure PSI



class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {

    front_left_updated = 0;
    front_right_updated = 0;
    rear_left_updated = 0;
    rear_right_updated = 0;    

    if (advertisedDevice.haveName()) { // does the advertised device have a name?
      if (advertisedDevice.getName() == "BR") { // if the name is "BR", it´s out pressure sensor
        //Serial.printf("Found BR Device: %s \n", advertisedDevice.toString().c_str()); // print all the received data to serial

        // get the address, and if it´s a known address, set which sensor should be updated
        if (advertisedDevice.getAddress().toString() == "4a:20:00:00:22:2b") { // F.L >> 0A 
          front_left_updated = 1;
        }
        if (advertisedDevice.getAddress().toString() == "4a:6c:00:00:9c:13") { // F.R >> 1B
          front_right_updated = 1;
        }
        if (advertisedDevice.getAddress().toString() == "4a:23:00:00:5f:00") { // R.L >> 2C
          rear_left_updated = 1;
        }
        if (advertisedDevice.getAddress().toString() == "4a:24:00:00:8e:5b") { // R.R >> 3D
          rear_right_updated = 1;
        }                        

        if (advertisedDevice.haveManufacturerData() == true) { // does the advertised device send manufacturer data? that data has the pressure and temperature values
          String strManufacturerData = advertisedDevice.getManufacturerData(); // get the advertised data as String

          byte cManufacturerData[100]; // helper variable to convert String (with capital S) to C-style string
          memcpy(cManufacturerData, strManufacturerData.c_str(), strManufacturerData.length()); // convert String to c-style string

          //for (int i = 0; i < strManufacturerData.length(); i++) { // print individual bytes of the manufacturer data
          //  Serial.printf("[%02X]", cManufacturerData[i]); // %02 means pad the value with zeros to have at least two digits, X is the byte
          //}
          //Serial.printf("\n"); // print new line after the manufacturers data

          // found a device that has the BR name

          // Received Manufacturers Data Example
          // [80] [1f] [1a] [00] [92] [14] [74]
          //
          // meaning:
          // 80 - status - ignore for now
          // 1f - battery voltage - 0x1f hex = 31 = 3.1V
          // 1a - temperature 0x1a hex = 26°C
          // 0092 - pressure 0x0092 hex = 146 = 14.6 psi
          // 14 74 - checksum - ignore for now  

          if (front_left_updated == 1) {
            front_left_voltage = (float)cManufacturerData[1] / 10.0;  // get battery voltage
            front_left_temperature = cManufacturerData[2];  // get temperature in °C
            front_left_pressure_psi_x10 = (uint16_t)cManufacturerData[3] << 8 | cManufacturerData[4]; // use two bytes for out 16bit value, psi x10
            front_left_pressure_psi = (float)front_left_pressure_psi_x10 / 10.0; // pressure in psi
          }
          
          if (front_right_updated == 1) {
            front_right_voltage = (float)cManufacturerData[1] / 10.0;  // get battery voltage
            front_right_temperature = cManufacturerData[2];  // get temperature in °C
            front_right_pressure_psi_x10 = (uint16_t)cManufacturerData[3] << 8 | cManufacturerData[4]; // use two bytes for out 16bit value, psi x10
            front_right_pressure_psi = (float)front_right_pressure_psi_x10 / 10.0; // pressure in psi
          }

          if (rear_left_updated == 1) {
            rear_left_voltage = (float)cManufacturerData[1] / 10.0;  // get battery voltage
            rear_left_temperature = cManufacturerData[2];  // get temperature in °C
            rear_left_pressure_psi_x10 = (uint16_t)cManufacturerData[3] << 8 | cManufacturerData[4]; // use two bytes for out 16bit value, psi x10
            rear_left_pressure_psi = (float)rear_left_pressure_psi_x10 / 10.0; // pressure in psi
          }
          
          if (rear_right_updated == 1) {
            rear_right_voltage = (float)cManufacturerData[1] / 10.0;  // get battery voltage
            rear_right_temperature = cManufacturerData[2];  // get temperature in °C
            rear_right_pressure_psi_x10 = (uint16_t)cManufacturerData[3] << 8 | cManufacturerData[4]; // use two bytes for out 16bit value, psi x10
            rear_right_pressure_psi = (float)rear_right_pressure_psi_x10 / 10.0; // pressure in psi
          }

        } 

        //Serial.println(advertisedDevice.getAddress().toString()); // get the device address and print it to serial
      }  
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();  //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  u8g2.begin(); // start the u8g2 library
}

static const unsigned char image_arrow_FL_bits[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x03};
static const unsigned char image_arrow_FR_bits[] = {0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char image_arrow_RL_bits[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00};
static const unsigned char image_arrow_RR_bits[] = {0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03};
static const unsigned char image_car_bits[] = {0x80, 0xff, 0x7f, 0x00, 0xe0, 0x03, 0xf0, 0x01, 0x70, 0x01, 0xa0, 0x03, 0x10, 0xff, 0x3f, 0x02, 0x18, 0xff, 0x3f, 0x06, 0x98, 0xff, 0x7f, 0x06, 0xf8, 0xff, 0xff, 0x07, 0xf8, 0xff, 0xff, 0x07, 0xf8, 0xff, 0xff, 0x07, 0xf8, 0xff, 0xff, 0x07, 0xf8, 0xff, 0xff, 0x07, 0xf8, 0x00, 0xc0, 0x07, 0x38, 0x00, 0x00, 0x07, 0x18, 0x00, 0x00, 0x06, 0x1e, 0x00, 0x00, 0x1e, 0x3f, 0x00, 0x00, 0x3f, 0x3f, 0x00, 0x00, 0x3f, 0x78, 0x00, 0x80, 0x07, 0x78, 0x00, 0x80, 0x07, 0xf8, 0x00, 0xc0, 0x07, 0xd8, 0xff, 0xff, 0x06, 0xd8, 0xff, 0xff, 0x06, 0x98, 0xff, 0x7f, 0x06, 0x98, 0xff, 0x7f, 0x06, 0x98, 0xff, 0x7f, 0x06, 0x98, 0xff, 0x7f, 0x06, 0x98, 0x8a, 0x44, 0x06, 0x98, 0xaa, 0x57, 0x06, 0x98, 0xca, 0x64, 0x06, 0x98, 0xec, 0x54, 0x06, 0x98, 0xff, 0x7f, 0x06, 0x98, 0xff, 0x7f, 0x06, 0x98, 0xff, 0x7f, 0x06, 0x98, 0xff, 0x7f, 0x06, 0x98, 0xff, 0x7f, 0x06, 0xd8, 0xf9, 0xe7, 0x06, 0xf8, 0x00, 0xc0, 0x07, 0xf8, 0x00, 0xc0, 0x07, 0x78, 0x00, 0x80, 0x07, 0x78, 0x00, 0x80, 0x07, 0x78, 0x00, 0x80, 0x07, 0x78, 0x00, 0x80, 0x07, 0xf8, 0x01, 0xe0, 0x07, 0xf8, 0xff, 0xff, 0x07, 0xf8, 0xff, 0xff, 0x07, 0xf8, 0xff, 0xff, 0x07, 0xf0, 0xff, 0xff, 0x03, 0xf0, 0xff, 0xff, 0x03, 0xc0, 0xff, 0xff, 0x00};



void loop() {
  // put your main code here, to run repeatedly:
  BLEScanResults *foundDevices = pBLEScan->start(scanTime, false);
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices->getCount());
  //Serial.println("Scan done!");
  pBLEScan->clearResults();  // delete results fromBLEScan buffer to release memory


  if (front_left_updated == 1) {
    Serial.println("F.L updated");
    Serial.print("Voltage (V): ");
    Serial.println(front_left_voltage);
    Serial.print("Temperature (°C): ");    
    Serial.println(front_left_temperature);  
    Serial.print("Pressure (psi): ");
    Serial.println(front_left_pressure_psi); 
  }

  if (front_right_updated == 1) {
    Serial.println("F.R updated");
    Serial.print("Voltage (V): ");
    Serial.println(front_right_voltage);
    Serial.print("Temperature (°C): ");    
    Serial.println(front_right_temperature);  
    Serial.print("Pressure (psi): ");
    Serial.println(front_right_pressure_psi); 
  }  

    if (rear_left_updated == 1) {
    Serial.println("R.L updated");
    Serial.print("Voltage (V): ");
    Serial.println(rear_left_voltage);
    Serial.print("Temperature (°C): ");    
    Serial.println(rear_left_temperature);  
    Serial.print("Pressure (psi): ");
    Serial.println(rear_left_pressure_psi); 
  }

  if (rear_right_updated == 1) {
    Serial.println("R.R updated");
    Serial.print("Voltage (V): ");
    Serial.println(rear_right_voltage);
    Serial.print("Temperature (°C): ");    
    Serial.println(rear_right_temperature);  
    Serial.print("Pressure (psi): ");
    Serial.println(rear_right_pressure_psi); 
  }  


  // OLED drawing 
  // helper variable is used to convert PSI float / temperature int value
  // into C-style string to be printed with u8g2 library
  char helper_c_string[10];
	int helper_str_width; // helper variable to store the width of the string to allow for right-align of the text

  u8g2.clearBuffer(); // clear the display buffer
  u8g2.setFontMode(1); // activate transparent font mode
  u8g2.setBitmapMode(1); // activate transparent bitmap mode
  
	u8g2.setFont(u8g2_font_helvB08_tf); // set font
  sprintf(helper_c_string, "%d%cC", front_left_temperature, 176); // convert temperature to c-style string
  u8g2.drawStr(0, 24, helper_c_string); // print temperature to OLED display

  u8g2.setFont(u8g2_font_profont17_tr); // set font
	dtostrf(front_left_pressure_psi, 0, 1, helper_c_string); // convert pressure float value to c-style string
  u8g2.drawStr(0, 12, helper_c_string); // draw the pressure value to OLED display

  u8g2.setFont(u8g2_font_helvB08_tf); // set font
  sprintf(helper_c_string, "%d%cC", front_right_temperature, 176); // convert temperature to c-style string
	helper_str_width = u8g2.getStrWidth(helper_c_string); // calculate the width of the string
  u8g2.drawStr(128 - helper_str_width, 24, helper_c_string); // print temperature to OLED display

  u8g2.setFont(u8g2_font_profont17_tr); // set font
	dtostrf(front_right_pressure_psi, 0, 1, helper_c_string); // convert pressure float value to c-style string
	helper_str_width = u8g2.getStrWidth(helper_c_string); // calculate the width of the string
  u8g2.drawStr(128 - helper_str_width, 12, helper_c_string); // draw the pressure value to OLED display

  u8g2.setFont(u8g2_font_helvB08_tf); // set font
  sprintf(helper_c_string, "%d%cC", rear_left_temperature, 176); // convert temperature to c-style string
  u8g2.drawStr(0, 62, helper_c_string); // print temperature to OLED display

  u8g2.setFont(u8g2_font_profont17_tr); // set font
	dtostrf(rear_left_pressure_psi, 0, 1, helper_c_string);	// convert pressure float value to c-style string
  u8g2.drawStr(0, 50, helper_c_string); // draw the pressure value to OLED display

  u8g2.setFont(u8g2_font_helvB08_tf); // set font
  sprintf(helper_c_string, "%d%cC", rear_right_temperature, 176); // convert temperature to c-style string
	helper_str_width = u8g2.getStrWidth(helper_c_string);	// calculate the width of the string
  u8g2.drawStr(128 - helper_str_width, 62, helper_c_string); // print temperature to OLED display

  u8g2.setFont(u8g2_font_profont17_tr); // set font
	dtostrf(rear_right_pressure_psi, 0, 1, helper_c_string); // convert pressure float value to c-style string	
	helper_str_width = u8g2.getStrWidth(helper_c_string);	// calculate the width of the string
  u8g2.drawStr(128 - helper_str_width, 50, helper_c_string); // draw the pressure value to OLED display

  u8g2.drawXBM(0, 47, 50, 6, image_arrow_RL_bits); // draw rear left arrow
  u8g2.drawXBM(0, 13, 50, 6, image_arrow_FL_bits); // draw front left arrow
  u8g2.drawXBM(78, 47, 50, 6, image_arrow_RR_bits); // draw rear right arrow
  u8g2.drawXBM(78, 13, 50, 6, image_arrow_FR_bits); // draw front right arrow
  u8g2.drawXBM(49, 9, 30, 49, image_car_bits); // draw the car in the middle

  u8g2.setFont(u8g2_font_helvB08_tf); // set the font
  u8g2.drawStr(56, 8, "PSI"); // draw the PSI label above the CAR

  u8g2.sendBuffer(); // send the buffer to the OLED display



  delay(50);
}
