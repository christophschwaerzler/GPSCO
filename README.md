# GPSCO
GPS Calibrated Oscillator: A simple GPS based manual calibration method for a 10 MHz oscillator using the 1 pps signal.

This is NOT another GPSDO, but rather a rock bottom method for manually calibrating an OCXO such
as one of these bargain kits incorporating a high quality CTI OSC5A2B and which are available from various online stores:

![OCXO](https://github.com/christophschwaerzler/GPSCO/assets/151140591/9bcd4fed-32d4-4c5d-ad9c-726201224459)

These kits have a trimmer (blue part in the above picture) which sets the voltage for the VCXO and thus the exact frequency.
They come preadjusted, but one can still improve the precision by calibrating the OCXO to a known good.
Afterwards the OCXO is used on a stand-alone basis again, until it might be recalibrated some time later.
This is the concept behind the project here.
