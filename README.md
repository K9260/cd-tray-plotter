# cd-tray-plotter

This machine is built from 2 old PC CD trays. It can draw vectors and custom symbols.

Arduino Uno is used to control the device.
Motors are driven with 2 H-bridges.

Originally these trays use a stepper motor to move the laser that reads and writes the CD.
Stepper motors can be driven a certain amount of degrees per pulse. These motors move 16 degrees per pulse.
That makes it easy to keep track of the coordinates where the pen is drawing.

Maximum movement on both axis is around 40mm.

Complete part list:
* Arduino Uno
* 2x CD trays
* 2x H-bridge
* 5V 3A power supply (2 amps would work also)
* Servo to move the pen up and down
* Plywood frame  

### Click the image for video
[![plotter](http://img.youtube.com/vi/kKaVy1PQwqk/0.jpg)](http://www.youtube.com/watch?v=kKaVy1PQwqk "Plotter")

![](https://raw.githubusercontent.com/K9260/cd-tray-plotter/master/images/IMG_20190822_183742.jpg)
![](https://raw.githubusercontent.com/K9260/cd-tray-plotter/master/images/IMG_20190822_183753.jpg)
