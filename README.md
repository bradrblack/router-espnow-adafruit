# ESPNOW to Adafrut Router
Gets ESPNOW data from one or more sensor modules and relay/route to Adafruit IO Feeds
## Features:
- Supports multiple sensors and uses a "name" attribute to route sensor data to different Adafruit IO Feeds
- Connects to WiFi to send data at the same time as receiving ESPNOW data
- You must configure the device to use the same, static channel as your WiFi router - using dynamic channels would cause an issue where the WiFi station moves to another channel and cannot receive ESPNOW messages
