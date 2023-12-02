# GPSCO
GPS Calibrated Oscillator: A simple GPS based manual calibration method for a 10 MHz oscillator using the 1 pps signal.

This is not a full blown GPSDO, but rather a rock bottom method for manually calibrating an OCXO such
as one of these bargain kits incorporating a high quality CTI OSC5A2B02 which are available from various online stores:

![OCXO](https://github.com/christophschwaerzler/GPSCO/assets/151140591/9bcd4fed-32d4-4c5d-ad9c-726201224459)

These kits have a trimmer (blue part in the above picture) which sets the voltage for the VCXO and thus the exact frequency.
They come preadjusted, but one can still improve the precision by calibrating the OCXO to a known good.
Afterwards the OCXO is used on a stand-alone basis again, until it might be recalibrated some time later.
This is the concept behind the project here.

I built the hardware around an Arduino UNO R3 board, since this is a cheap and popular uC system which offers most of
the hardware needed. Many builders will already have such a board lying around. There is only one problem:
The internal counter on timer1 allows for a maximal counting rate of f/2.5, where f is the clock frequency i.e. 16 MHz.
This is 6.4 MHz and is not sufficient for a 10 MHz OCXO, such as mine. Accordingly one more piece of hardware is needed
to divide bring the counts into the allowed range: A 74HC390 (or 74HCT390), which is a divider and with some change in
wiring allows for divisions by 2, 5 or 10.

This is the full schematics, just note that only one of the four possible divider values will be connected to the microcontroller:
@@@
The divider should be chosen such that the input to the Arduino is the highest frequency below 6.4 MHz. For a
10 MHz OCXO this is the 2:1 divider setting, leading to an input to the Arduino timer of about 5 MHz.

The hardware setup can be put together on a breadboard in just a few minutes. Mine looks like this:
![GPSCO_setup](https://github.com/christophschwaerzler/GPSCO/assets/151140591/38020d42-44e5-461f-b481-1b859744947c)

Top left is the OCXO in a protective enclosure, which also increases thermal stability through wind protection.
To the left of the Arduino board is a breadboard with the 74HC390 and under the Arduino another breadboard just
for interfacing the connector to the GPS. You need a GPS with the 1 pps signal directly available, you do not
need the NMEA data from the GPS for this simple setup. Just ensure that the pps is solid, for example by checking
that the onboard LED of the Arduino goes on and off rythmically in 1 second intervals.

The system is quite sensible to HF-interference from the 10 MHz to the 1 pps, so take care to seperate the lines as
far as possible. Most probably you will also need some choking on the lines (especially the 1 pps line) with ferrites
like I had to:

![GPSCO_ferrites](https://github.com/christophschwaerzler/GPSCO/assets/151140591/d6658d51-1d59-4ee9-bc1d-f81a0125335e)

#Code inherent delay
Ever since I came across such a timer/software implementation for latching the OCXO for the first time, I had a feeling
that there is a (very small) delay in stopping the timer. The respective sequence of the code

@@@

typically checks whether the timer is started first and afterwards whether the timer is stopped. This means, that the stopping of
the timer is slightly delayed versus the starting (plus an integer number of seconds). With the above setup I could
not only proove my feeling, but also quantify the effect. This allows for correcting the results, leading to a significantly
improved precision for shorter measurement times.
