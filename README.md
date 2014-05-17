Captain
=======

An Arduino-powered remote control model cargo ship system licensed under the GNU-GPLv3.

Structure
---------
Captain is composed of two modules. A **Base module** and a **Ship module**. The Base module is the land-based controller while the Ship module will be on the cargo boat itself receiving and executing commands.

Hardware
--------
* 1x [Arduino Uno R3][ArduinoUno]
* 1x [Arduino Mega 2560 R3][ArduinoMega]
  * 1x [USB Host Shield 2.0][HostShield]
  * 1x [Bluetooth Dongle][BTD]
* 2x [XBee-PRO 900HP][XBee]
  * 2x [900MHz Duck Antenna RP-SMA][900MHz]
* 1x [Fastrax UP501 GPS Receiver][GPS]
* 1x [16x2 Character LCD - RGB Backlight][LCD]
* 1x [PS3 DualShock 3 Controller][DS3]
* 1x [Futaba S3003 Servo][S3003]
* 1x [Pololu High-Power Motor Driver 18v15][PololuDriver]
* 1x 12V Brushed DC Motor (We pulled one from a cordless drill)

Licensing
---------
Copyright &copy; 2013-2014 Kevin Mark

Captain is licensed under the GNU GPLv3. Please see `LICENSE.md` for licensing
details.


[ArduinoUno]: https://www.sparkfun.com/products/11021
[ArduinoMega]: https://www.sparkfun.com/products/11061
[HostShield]: http://www.circuitsathome.com/products-page/arduino-shields/usb-host-shield-2-0-for-arduino
[BTD]: http://www.amazon.com/dp/B0085BPPGA
[XBee]: http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/point-multipoint-rfmodules/xbee-pro-900hp
[900MHz]: https://www.sparkfun.com/products/9143
[GPS]: https://www.sparkfun.com/products/10702
[LCD]: https://www.sparkfun.com/products/10862
[DS3]: http://www.amazon.com/dp/B0015AARJI
[S3003]: http://www.gpdealera.com/cgi-bin/wgainf100p.pgm?I=FUTM0031
[PololuDriver]: http://www.pololu.com/catalog/product/755
