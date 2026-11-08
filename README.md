# Solar2MQTT ESP32 EaSun SMS 6.5K [![GitHub release](https://img.shields.io/github/v/release/AndyMuc72/SolarMQTT-ESP32-EaSun-SMS-6.5K?include_prereleases&sort=semver&color=blue)](https://github.com/AndyMuc72/SolarMQTT-ESP32-EaSun-SMS-6.5K/releases/latest) [![EASUN build](https://github.com/AndyMuc72/SolarMQTT-ESP32-EaSun-SMS-6.5K/actions/workflows/easun-build.yml/badge.svg)](https://github.com/AndyMuc72/SolarMQTT-ESP32-EaSun-SMS-6.5K/actions/workflows/easun-build.yml)

This ESP32 fork targets the EASUN iSolar-SMS-6.5KP-WiFi / SMS-6.5KP family. See [the read-only CVTE register map and target validation notes](Protocol/EASUN_SMS_CVTE_READ_ONLY.md) before relying on individual readings. The WiFi-labelled firmware 8 unit has target-specific corrections for output watts, load percentage, battery current, DC bus voltage, and inverter-reported SOC.

## Public release 0.1.0-easun.7

Normal releases contain **no personal Wi-Fi defaults or saved configuration**. This is also the policy for future releases. Private recovery files must never be published. See [release notes, test status and USB/OTA instructions](RELEASE_NOTES.md). Versions `.5` and `.6` are known problematic user-reported releases; `.7` is based on `.4` with the tested UI and update-check repairs.

## EASUN SMS-6.5KP fork

Read-only telemetry for the single-MPPT EASUN iSolar-SMS-6.5KP-Wifi is tracked in [Protocol/EASUN_SMS_CVTE_READ_ONLY.md](Protocol/EASUN_SMS_CVTE_READ_ONLY.md). Build status: [![EASUN ESP32 build](https://github.com/AndyMuc72/SolarMQTT-ESP32-EaSun-SMS-6.5K/actions/workflows/easun-build.yml/badge.svg?branch=codex/easun-sms-support)](https://github.com/AndyMuc72/SolarMQTT-ESP32-EaSun-SMS-6.5K/actions/workflows/easun-build.yml). A successful build is compile status, not physical inverter validation.

# Looking for the ESP8266 Variant? go [HERE](https://github.com/softwarecrash/Solar2MQTT-ESP8266)

# Features:
- Support WiFi or LAN Modules
- captive portal for wifi and MQTT config
- config in webinterface
- Full Controll with [Custom commands](https://github.com/softwarecrash/Solar2MQTT/wiki/Set-parameters)
- get essential data over webinterface, get [all data](https://github.com/softwarecrash/Solar2MQTT/wiki/Datapoints-and-units) over MQTT
- classic MQTT datapoints or Json string over MQTT
- get status JSON over web at `/api/status`
- firmware update over webinterface
- debug log over USB or Webserial
- [blink codes](https://github.com/softwarecrash/Solar2MQTT/wiki/Blink-Codes) for the current state of the ESP
- [Reset functions](https://github.com/softwarecrash/Solar2MQTT/wiki/Reset)
- [Support Home Assistant](https://github.com/softwarecrash/Solar2MQTT/wiki/HomeAssistant-integration)





**works with**
- Most devices that use the watchpower PC Software
-  Most devices that use the Solarpower PC Software
- PIP devices
- i solar 
- IGrid
- Many devices from EASUN
- and many many others based on the chinese solar inverter with a rj45 jack and usb port, primary identified by the display
- Take a look at the [supported devices](https://github.com/softwarecrash/Solar2MQTT/wiki/Supported-Devices) and [reported working devices](https://github.com/softwarecrash/Solar2MQTT/wiki/Reported-Working-Devices) in the wiki
- If your device works, see [how to report a working device](https://github.com/softwarecrash/Solar2MQTT/wiki/Report-a-Working-Device)


**Main screen:**

![alt text](Docs/README/status.png)

**Menu:**

![alt text](Docs/README/menu.png)

**Config:**

![alt text](Docs/README/mqtt.png)
![alt text](Docs/README/device.png)

![alt text](Docs/README/wifi.png)
![alt text](Docs/README/wifi-extendet.png)

![alt text](Docs/README/firmware.png)
![alt text](Docs/README/debug.png)

# How to use:
- flash your ESP32 (recommended Wemos D1 Mini ESP32) with our [Flash2MQTT-Tool](https://flash.2mqtt.de/?get=Solar2MQTT) or our desktop [2MQTT-Flasher](https://github.com/all-solutions/2MQTT-Flasher)
- connect the ESP like the [wiring diagram](https://github.com/softwarecrash/Solar2MQTT/wiki/Wiring-Diagram)
- search for the wifi ap "Solar2MQTT-AP" and connect to it
- surf to 192.168.4.1 and set up your wifi and optional MQTT
- that's it :)

### How-To video by Jarnsen

<a href="http://www.youtube.com/watch?feature=player_embedded&v=7u8hPLdXeso" target="_blank">
 <img src="http://img.youtube.com/vi/7u8hPLdXeso/0.jpg" alt="Watch the video" />
</a>



**POWER:** Using a 3.3V DC Buck Converter that can handle up to 20V or a DC/DC or USB power currently.

# Parts required to build

Most of the parts can be bought as modules, it's usually cheaper that way.

- ESP32
- MAX3232 module Like this https://amzn.eu/d/8t3gk5t or https://bit.ly/3BFPqrw or with orginal cable https://www.amazon.de/dp/B09XWPTDYP
- DC-DC buck module - 12-80v down to 5v

# Completely assembled and tested PCB's

You are welcome to get fully stocked and tested PCB's. These are then already loaded with the lastest firmware. The earnings from the PCBs are used for the further development of existing and new projects.

![Solar-MQTT-PCB](https://user-images.githubusercontent.com/17761850/233859179-cc9c9075-b88a-4f38-b804-bc0f409cf8ce.png)

If interested see [here](https://all-solutions.store)

#
Questions?
[Join the Discord Channel (German / English)](https://discord.gg/pAArqVsVS4)

#
[<img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"/>](https://donate.softwarecrash.de)

[![LICENSE](https://licensebuttons.net/l/by-nc-nd/4.0/88x31.png)](https://creativecommons.org/licenses/by-nc-nd/4.0/)

