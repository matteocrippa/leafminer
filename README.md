# LeafMiner ☘️⛏️

A FOSS ESP(\*) device Bitcoin miner.

![LeafMiner Logo](.github/images/leafminer.png)

**Disclaimer:** Mining a block with LeafMiner is highly unlikely given the current complexity. The primary goal is to provide a learning platform for a deeper understanding of how Bitcoin and SHA256 operate.

| ![LeafMiner on Lilygo-T-S3](.github/images/leafminer-lilygots3.png) | ![LeafMiner on GeekMagic Clock Small TV](.github/images/leafminer-geekmagicclock-smalltv.png) |
| :------------------------------------------------------------------ | :-------------------------------------------------------------------------------------------- |
| _Lilygo-T-S3_                                                       | _GeekMagic Clock - SmallTV_                                                                   |

### Features

- [x] SHA256 Optimisation for 64 + 16 structure (aka Midstate)
- [x] Double Hash Early Exit
- [x] Support for ESP32 and ESP8266

The SHA256 code uses a tweaked version of [NerdSHA256plus](https://github.com/BitMaker-hub/NerdMiner_v2) by NerdMiner.

### Supported Boards:

| Board             | Status | Expected Hashrate |
| :---------------- | :----: | :---------------- |
| ESP8266EX         |   🟧   | ±9 kH/s           |
| ESP32             |   🟩   | ±48 kH/s          |
| ESP32-S2          |   🟩   | ±19 kH/s          |
| ESP32-S3          |   🟩   | ±62 kH/s          |
| LILYGO-T-S3       |   🟩   | ±62 kH/s          |
| SMALLTV (ESP8266) |   🟧   | ±16 kH/s          |

_Legenda_
| Icon | Description |
| :- | :- |
| 🟩 | Fully functional |
| 🟧 | Partially functional |
| 🟥 | Not working |

### Mining method:

- [x] [Solo](docs/solo-mining.md)
- [ ] [Cascade](docs/cascade-mining.md) (aka 1 Main and N nodes)

# How to use

## Requirements

- A supported board
- Platform.io

## Build & Install

- Rename the file src/leafminer.template.h into leafminer.h.
- Edit the content according to your setup.
- Build and Upload!

Build & Upload.

## Donate

If you want to support this project:
Bitcoin - `bc1quqr7agnx6e0k9kn8h24zfj5dueeurhzflt3qjg`

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
