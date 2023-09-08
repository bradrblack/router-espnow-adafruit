//
//  ESPNOW to Adafruite IO "Router"  - Brad Black (C) 2023
//
//  Get ESPNOW data from one or more sensor modules and relay/route to Adafruit IO Feeds
//

#include "esp_now.h"
#include "esp_wifi.h"
#include "WiFi.h"
#include "AdafruitIO_WiFi.h"
#include "secrets.h"

/* secrets looks like...

const char* WIFI_SSID = "MY_SSID";
const char* WIFI_PASS = "MY_PASSWORD";
#define IO_USERNAME "MY_ADAFRUIT_IO_USERNAME"
#define IO_KEY "MY_ADAFRUIT_IO_KEY"

*/

// #define OPERATION_MODE // No Serial Debug
#define TEST_MODE // Serial Debug on for TESTING ONLY

uint32_t start;

typedef struct msg
{
  char name[32];
  float temperature;
  float voltage;
} msg;

// Create a struct_message called sensorData
msg sensorData;

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, "", "");
AdafruitIO_Feed *pooltemp = io.feed("pooltemp");
AdafruitIO_Feed *poolvolts = io.feed("voltage");
AdafruitIO_Feed *pooltemp2 = io.feed("pooltemp2");
AdafruitIO_Feed *poolvolts2 = io.feed("voltage2");

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&sensorData, incomingData, sizeof(sensorData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Name: ");
  Serial.println(sensorData.name);
  Serial.print("Temp: ");
  Serial.println(sensorData.temperature);
  Serial.print("Voltage: ");
  Serial.println(sensorData.voltage);
  Serial.println();
  if (String(sensorData.name) == "Pool")
  {
    Serial.println("Sending to IO");
    pooltemp->save(sensorData.temperature);
    poolvolts->save(sensorData.voltage);
  };
  if (String(sensorData.name) == "Dev")
  {
    Serial.println("Sending to IO");
    pooltemp2->save(sensorData.temperature);
    poolvolts2->save(sensorData.voltage);
  }

  if (millis() > (start + (1000 * 60 * 60 * 24))) {    // daily restart
    delay(2000);
    ESP.restart();
  }
}

void setup()
{

#if defined(TEST_MODE)
  Serial.begin(115200);
  while (!Serial)
  {
  }
  Serial.println("\nstarted");
#endif

start = millis();

  // Bring up the WiFi connection

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  delay(500);

  Serial.print("\nConnected to ");
  Serial.println(WIFI_SSID);
  Serial.print("Using IP address: ");
  Serial.println(WiFi.localIP());

  // Set the WiFi channel here to whatever your router is using - in this case it is "1" - this is required for WIFI_STA & ESPNOW at the same time

  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info

  Serial.println("ESP-NOW-INIT complete");

  esp_now_register_recv_cb(OnDataRecv);

  // connect to io.adafruit.com

  io.connect();
  delay(1000);
  // wait for a connection
  while (io.status() < AIO_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  // we are connected
  Serial.println();
  Serial.println(io.statusText());
}

void loop(void)
{

  io.run();
}
