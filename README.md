# ESPHOME Argoclima IR

This project lets you use an ESP32 device with IR to interact with the Argoclima Ulisse 13 A/C unit with a WREM-3 remote. Tested with M5Stack AtomS3 Lite.

It exposes a Climate controller through the [ESPHome native API](https://esphome.io/components/api.html), and allows you to integrate the unit in Home Assistant.


## Software

1. Copy and rename `secrets.yaml.example` to `secrets.yaml` and update it with your WiFi credentials (`wifi_ssid` and `wifi_password`).

2. Build the image with [ESPHome](https://esphome.io/guides/getting_started_command_line.html)

```sh
make compile
```

3. Upload/flash the firmware to the board.

```sh
make upload
```

## Credits

- https://github.com/crankyoldgit/IRremoteESP8266/blob/master/src/ir_Argo.h
