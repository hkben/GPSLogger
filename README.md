# GPSLogger
GPS Logging projects for Arduino


## Requirements
### Hardware
- Ardunio
- u-blox neo-6m GPS module
- 1602A LCD display
- Micro SD / SD Card module


### Library
- NMEAGPS (https://github.com/SlashDevin/NeoGPS)
- GPSport
- Streamers
- LiquidCrystal
- SD


### Log Format
* CSV
  - date : yy-mm-dd
  - time : hh:mm:ss (UTC)
  - latitude : seventh decimal place
  - longitude : seventh decimal place
  - altitude
  - speed(km/h)
  - heading


### Circuit

This Circuit is for [Ardunio Pro Mirco](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/hardware-overview-pro-micro)

* u-blox neo-6m GPS module
  - TXD - RXI
  - RXD - TXD

* 1602A LCD display
  - RS - pin 9
  - E  - pin 8
  - D4 - pin 5
  - D5 - pin 4
  - D6 - pin 3
  - D7 - pin 2

* Micro SD / SD Card module
  - MOSI - pin 16
  - MISO - pin 14
  - CLK - pin 15
  - CS - pin 10
