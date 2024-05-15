# ESPHOME Argoclima IR

This project lets you use an ESP32 device with IR to interact with the Argoclima Ulisse 13 A/C unit with a WREM-3 remote. Tested with M5Stack AtomS3 Lite.

It exposes a Climate controller through the [ESPHome native API](https://esphome.io/components/api.html), and allows you to integrate the unit in Home Assistant as depicted below:
![Home Assistant screenshot](./docs/ha_screen.png)



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

> By default the project builds for the AtomS3 board. To change your board, you can specify the `BOARD` parameter. For example for the Olimex ESP32-EVB:
>```sh
>make compile BOARD=esp32-evb
>make upload BOARD=esp32-evb
>```

Now when you go to the Home Assistant “Integrations” screen (under “Configuration” panel), you should see the ESPHome device show up in the discovered section (although this can take up to 5 minutes). Alternatively, you can manually add the device by clicking “CONFIGURE” on the ESPHome integration and entering “<NODE_NAME>.local” as the host.

![Comfoair Q Home Assistant](docs/homeassistant.png?raw=true "Comfoair Q Home Assistant")

Optional: for the ventilation card with the arrows, see [`docs/home-assistant/example-picture-elements-card.yaml`](docs/home-assistant/example-picture-elements-card.yaml)


## Credits

Based on the original repo: https://github.com/felixstorm/esphome-custom-components

Inspired by

- https://github.com/vekexasia/comfoair-esp32
- https://github.com/michaelarnauts/aiocomfoconnect
- [https://github.com/mat3u/comfoair-esp32](https://github.com/mat3u/comfoair-esp32/tree/hacomfoairmqtt-compatibility)
- [https://github.com/hcouplet/comfoair-esp32](https://github.com/hcouplet/comfoair-esp32/tree/hacomfoairmqtt-compatibility)

A lot of this repo was inspired by the reverse engineering [here](https://github.com/marco-hoyer/zcan/issues/1).

- [ComfoControl Protocol](https://github.com/michaelarnauts/aiocomfoconnect/blob/master/docs/PROTOCOL.md)
- [RMI PROTOCOL](https://github.com/michaelarnauts/aiocomfoconnect/blob/master/docs/PROTOCOL-RMI.md)
- [PDO PROTOCOL](https://github.com/michaelarnauts/aiocomfoconnect/blob/master/docs/PROTOCOL-PDO.md)
