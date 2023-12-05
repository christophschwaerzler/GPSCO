# GPSCO
GPS Calibrated Oscillator: A simple GPS based manual calibration method for a 10 MHz oscillator using the 1 pps signal.

This is not a full blown GPSDO, but rather a rock bottom method for manually calibrating an OCXO such
as one of these bargain kits incorporating a high quality CTI OSC5A2B02 which are available from various online stores:

![OCXO](https://github.com/christophschwaerzler/GPSCO/assets/151140591/9bcd4fed-32d4-4c5d-ad9c-726201224459)

These kits have a trimmer (blue part in the above picture) which sets the voltage for the VCXO and thus the exact frequency.
They come preadjusted, but one can still improve the precision by calibrating the OCXO to a known good.
Afterwards the OCXO is used on a stand-alone basis again, until it might be recalibrated some time later.
This is the concept behind the project here.


The idea to use a microcontroller to latch one of its internal timers is not new [1,2,3], but typically used at the
core of a feedback loop for controlling the OCXO in order to create a GPS disciplined oscillator (GPSDO). My goal
was to use this approach, but only for manually adjusting the OCXO and thus requiring just a minimum of additional
hardware.
I built the hardware around an Arduino UNO R3 board, since this is a cheap and popular uC system which offers most of
the hardware needed. Many builders will already have such a board lying around. There is only one problem:
The internal counter on timer1 allows for a maximal counting rate of f/2.5, where f is the clock frequency i.e. 16 MHz.
This is 6.4 MHz and is not sufficient for a 10 MHz OCXO, such as mine. Accordingly one more piece of hardware is needed
to divide bring the counts into the allowed range: A 74HC390 (or 74HCT390), which is a divider and with some change in
wiring allows for divisions by 2, 5 or 10.

This is the full schematics, note that only one of the four possible divider values will be connected to the microcontroller:

![Schematics_full](https://github.com/christophschwaerzler/GPSCO/assets/151140591/2730d952-068c-4b6d-8458-8d298501ea78)

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

Early on, it became obvious that the range of the trimmer in my OCXO kit was not sufficient to bring the frequency down
to the target frequency. The lowest I could get to was 10,000,002.4 Hz. A quick search on the WWW revealed that this is
a known issue [4] with kits from this vendor. This problem can be solved easily by changeing a resistor. I decided to replace
R2 (resistor marking: 68B) with a 10k resistor, which braught 10,000,000.0 right in the middle of the trimmer range.

# Code inherent delay

Ever since I came across such a timer/software implementation for latching the OCXO for the first time, I had a feeling
that there is a (very small) delay in stopping the timer. The respective sequence of the code

![Code](https://github.com/christophschwaerzler/GPSCO/assets/151140591/058534f5-c5ef-49ca-9110-84dd6a31d0d9)

typically checks whether the timer is started first and afterwards whether the timer is stopped. This means, that stopping 
the timer is slightly delayed versus starting (plus an integer number of seconds). With the above setup I could
not only produce proove, but also quantify this effect. This allows for numerically correcting the results, leading to a significantly
improved precision for shorter measurement times.

The following graph gives measured frequencies for a variety of measurement times. The blue dots represent the raw data without adjustment
and the red dots give the calculated values after applying the adjustments. Whereas there is not a big difference between the two values
for measuremt times of about 20 seconds and longer, it is very obvious that for shorter measurement times the adjustment significantly
improves the precision.

[1] http://www.arrl.org/files/file/QEX_Next_Issue/2015/Jul-Aug_2015/Marcus.pdf

[2] http://roland.cordesses.free.fr/GPS_Si2cor.html

[3] https://github.com/erikkaashoek/Arduino_SI5351_GPSDO

[4] https://www.eevblog.com/forum/projects/cheap-chinese-ocxo-frequency-reference-modules/