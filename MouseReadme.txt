MouseReadme.txt

This library is for the attiny85 running tiny core Arduino (e.g. the Digispark)

This implements a USB HID Touch device with 16bit 
The code was borrowed mostly from the Digispark Mouse library and from Rapha�l Ass�nat's code on using an atmega8 as a Nintendo Gamecube/N64 controller to USB bridge: http://www.raphnet.net/electronique/gc_n64_usb/index_en.php and from Yiyin Ma and Abby Lin of Cornell https://instruct1.cit.cornell.edu/courses/ee476/FinalProjects/s2007/ayl26_ym82/ayl26_ym82/index.htm
Rapha�l's work is truly marvelous, and Ma and Lin did a nice job with the mouse software. 

Because most of this code is coming from other projects with GNU GPL, I am letting my modifications inherit the same protection. A copy of this license is included in the source. 

As to the use of this code in Arduino, #include "DigiTouch.h" as you would any other library. See the included sample for use of the functions. 

