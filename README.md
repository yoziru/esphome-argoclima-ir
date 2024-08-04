# ESPHOME Argoclima IR

This project lets you use an ESP32 device with IR to interact with the Argoclima Ulisse 13 A/C unit with a WREM-3 remote. Tested with M5Stack AtomS3 Lite.

It exposes a Climate controller through the [ESPHome native API](https://esphome.io/components/api.html), and allows you to integrate the unit in Home Assistant.


## Software

Supported boards:
- `m5stack-atoms3`: M5Stack AtomS3 / Lite
- `m5stack-atoms3-ir`: M5Stack AtomS3 / Lite with M5Stack IR unit (U002)
- `m5stack-nanoc6`: M5Stack NanoC6
- `m5stack-nanoc6-ir`: M5Stack NanoC6 with M5Stack IR unit (U002)

1. Copy and rename `secrets.yaml.example` to `secrets.yaml` and update it with your WiFi credentials (`wifi_ssid` and `wifi_password`).

1. Set the `ha_room_temperature_sensor` substitution to your room temperature sensor in Home Assistant.

1. Build the image with [ESPHome](https://esphome.io/guides/getting_started_command_line.html)

    ```sh
    make compile BOARD=m5stack-atoms3
    ```


1. Upload/flash the firmware to the board.

    ```sh
    make upload BOARD=m5stack-atoms3
    ```

1. After flashing, you can use the log command to monitor the logs from the device. The host suffix is the last part of the device name in the ESPHome dashboard (e.g. 5b2ac7).

    ```sh
    make logs BOARD=m5stack-atoms3 HOST_SUFFIX=-5b2ac7
    ```

1. For updating your device, you can OTA update over local WiFi using the same host suffix:
    ```sh
    make upload BOARD=m5stack-atoms3 HOST_SUFFIX=-5b2ac7
    ```
## Credits

- https://github.com/crankyoldgit/IRremoteESP8266/blob/master/src/ir_Argo.h
