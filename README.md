# esphome-neopixel_bus

Enable Output Ports on NeoPixelBus strips --- to use with standard esphome compoments

use this module if you want to use the standard light component of esphome where you set the output pin
which are mapped to the neo pixel bus pixels.

If you have for example a P9813 strip, with mosfet drivers like this:

https://pmdway.com/products/p9813-12v-rgb-led-strip-mosfet-controller

you can use than this esphome module to drive via pwm a motor or led whatever you want instead of the led strip.

## Example

```yaml
substitutions:
  plug_name: schrank-l
  wifi_ssid: !secret wifi_ssid
  wifi_password: !secret wifi_password

esphome:
  name: ${plug_name}

wifi:
  ssid: ${wifi_ssid}
  password: ${wifi_password}
  reboot_timeout: 3600s

# Enable logging
logger:

# Enable Home Assistant API
api:

ota:

esp8266:
  board: esp01_1m

external_components:
  - source:
      type: git
      url: https://github.com/mabels/esphome-neopixel_bus
    components:
    - neopixel_bus

neopixel_bus:
   - id: fled_bus
     chip_channels: 4
     variant: SK6812
     pin: GPIO2
     method: ESP8266_UART1
     num_chips: 3

output:
  - platform: neopixel_bus
    id: output_f1_g
    channels:
    - bus: fled_bus
      chip_offset: 0
      num_chips: 1
      channel_offset: 0

  - platform: neopixel_bus
    id: output_f1_r
    channels:
    - bus: fled_bus
      chip_offset: 1
      num_chips: 1
      channel_offset: 1

  - platform: neopixel_bus
    id: output_f1_b
    channels:
    - bus: fled_bus
      chip_offset: 2
      num_chips: 1
      channel_offset: 2

light:
  - platform: rgb
    name: "f1"
    red: output_f1_r
    green: output_f1_g
    blue: output_f1_b

```
