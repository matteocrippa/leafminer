# LeafMiner ☘️⛏️

LeafMiner is a Free and Open Source Software (FOSS) ESP(\*) device Bitcoin miner.

![LeafMiner Logo](.github/images/leafminer.png)

**Disclaimer:** While the chances of successfully mining a block with LeafMiner are highly unlikely due to the current complexity, the primary goal of this project is to serve as a learning platform for gaining a deeper understanding of how Bitcoin and SHA256 operate.

| ![LeafMiner on Lilygo-T-S3](.github/images/leafminer-lilygots3.png) | ![LeafMiner on GeekMagic Clock Small TV](.github/images/leafminer-geekmagicclock-smalltv.png) |
| :------------------------------------------------------------------ | :-------------------------------------------------------------------------------------------- |
| _Lilygo-T-S3_                                                       | _GeekMagic Clock - SmallTV_                                                                   |

## Features

- [x] SHA256 Optimization for 64 + 16 structure (aka Midstate)
- [x] Double Hash Early Exit
- [x] Support for ESP32 and ESP8266

The SHA256 code is based on a tweaked version of [NerdSHA256plus](https://github.com/BitMaker-hub/NerdMiner_v2) by NerdMiner.

## Supported Boards:

| Board             | Status | Expected Hashrate |
| :---------------- | :----: | :---------------- |
| ESP8266EX         |   🟧   | ±16 kH/s           |
| ESP32             |   🟩   | ±48 kH/s          |
| ESP32-S2          |   🟩   | ±19 kH/s          |
| ESP32-S3          |   🟩   | ±62 kH/s          |
| LILYGO-T-S3       |   🟩   | ±62 kH/s          |
| SMALLTV (ESP8266) |   🟧   | ±16 kH/s          |

_Legend_
| Icon | Description |
| :--- | :-------------- |
| 🟩 | Fully functional |
| 🟧 | Partially functional |
| 🟥 | Not working |

## Mining Method:

- [x] [Solo](docs/solo-mining.md)
- [ ] [Cascade](docs/cascade-mining.md) (aka 1 Main and N nodes)

# How to Use

## Requirements

- A supported board
- (optional) VSCode + Platformio.io

## Build & Install

### Flash Binary

- TBD

### Build from Scratch

- Clone the project
- Upload the project to your board

### Setup

1. Power up your ESP32/ESP8266.
2. Connect to the LeafMiner WiFi.
3. Open your browser to the page [http://192.168.4.1](http://192.168.4.1) if the captive portal is not appearing.
4. Provide the information in the form and hit save, then reboot.

We have already pre-set _public-pool.io_ as the suggested solo pool, but you can change it if you prefer.

If the setup was successful, you should be able to see your miner in the stats.

If you need to restart the setup flow:

- ESP32 - Unplug the USB cable, press and hold the physical button, and plug in the USB.
- ESP8266 - You need to erase the flash and reflash.

You should now discover the _LeafMiner_ WiFi again and go through the setup process.

## Donate

If you want to support this project, consider donating to the Bitcoin address: `bc1quqr7agnx6e0k9kn8h24zfj5dueeurhzflt3qjg`

## Credits

Inspired by:

- [HAN](https://github.com/valerio-vaccaro/HAN)
- [NerdMinerv2](https://github.com/BitMaker-hub/NerdMiner_v2)
- [Jade](https://github.com/Blockstream/Jade/tree/miner_all_0.1.47/components/miner)

Useful documentation:

- [Solominer](https://github.com/iceland2k14/solominer/blob/main/solo_miner.py)
- [pyminer.py](https://github.com/jgarzik/pyminer/blob/master/pyminer.py)
- [Stratum Protocol](https://reference.cash/mining/stratum-protocol)
- [Stratum Protocol Diagram](https://github.com/aeternity/protocol/blob/master/STRATUM.md)
- [NBits](https://learnmeabitcoin.com/technical/bits)
- [Bitcoin Mining](https://www.righto.com/2014/02/bitcoin-mining-hard-way-algorithms.html)
- [How To Mine](https://gist.github.com/Ending2015a/70373b2f6f665a765b4d0b0c427f052b)
- [Image Converter 565](http://www.rinkydinkelectronics.com/t_imageconverter565.php)
- [Lilygo-T-Display-S3](https://github.com/Xinyuan-LilyGO/T-Display-S3/tree/main)
