# GPSCO
GPS Calibrated Oscillator: A simple GPS based manual calibration method for a 10 MHz oscillator using the 1 pps signal.

This is NOT another GPSDO, but rather a rock bottom method for manually calibrating an OCXO such
as one of these bargain kits incorporating a high quality CTI OSC5A2B and which are available from various online stores:

![OCXO](https://github.com/christophschwaerzler/GPSCO/assets/151140591/9bcd4fed-32d4-4c5d-ad9c-726201224459)

These kits have a trimmer (blue part in the above picture) which sets the voltage for the VCXO and thus the exact frequency.
They come preadjusted, but one can still improve the precision by calibrating the OCXO to a known good.
Afterwards the OCXO is used on a stand-alone basis again, until it might be recalibrated some time later.
This is the concept behind the project here.

I built the hardware around an Arduino UNO R3 board, since this is a cheap and popular uC system which offers most of
the hardware which is needed. Many builders will already have such a board lying around. There is only one problem:
The internal counter on timer1 allows for a maximal counting rate of f/2.5, where f is the clock frequency i.e. 16 MHz.
This is 6.4 MHz and is not sufficient for a 10 MHz OCXO, such as mine. Accordingly one more piece of hardware is needed
to divide bring the counts into the allowed range: A 74HC390 (or 74HCT390), which is a divider and with some change in
wiring allows for divisions by 2, 5 or 10.

I put the OCXO in a 3D-printed protective enclosure, which also increases thermal stability through wind protection:

![OCXO_in_enclosure](https://github.com/christophschwaerzler/GPSCO/assets/151140591/3119a258-fadd-4b6d-b466-042e5a79baed)

