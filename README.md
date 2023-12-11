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

![OCXO_schematics_full](https://github.com/christophschwaerzler/GPSCO/assets/151140591/23832137-ae0a-4a30-8c26-318da26e637a)

The divider should be chosen such that the input to the Arduino is the highest frequency below 6.4 MHz. For a
10 MHz OCXO this is the 2:1 divider setting, leading to an input to the Arduino timer of 5 MHz.

The hardware setup can be put together on a breadboard in just a few minutes. Mine looks like this:
![GPSCO_setup](https://github.com/christophschwaerzler/GPSCO/assets/151140591/38020d42-44e5-461f-b481-1b859744947c)

Top left is the OCXO in a protective enclosure, which also increases thermal stability through wind protection.
To the left of the Arduino board is a breadboard with the 74HC390 and under the Arduino another breadboard just
for interfacing the connector to the GPS. You need a GPS with the 1 pps signal directly available, you do not
need the NMEA data from the GPS for this simple setup. In my setup, I used a QLG1 GPS from QRP-Labs [4].
Just ensure that the pps is solid, for example by checking that the onboard LED of the Arduino goes on and off
rythmically in 1 second intervals.

The system is quite sensible to HF-interference from the input frequency to the 1 pps, so take care to seperate the lines as
far as possible. Most probably you will also need some choking on the lines (especially the 1 pps line) with ferrites
like I had to:

![GPSCO_ferrites](https://github.com/christophschwaerzler/GPSCO/assets/151140591/d6658d51-1d59-4ee9-bc1d-f81a0125335e)

Despite these precautions, I still encountered about 2% of the 1 second counts to be corrupt. These cases can easily be 
identified and sorted out. For longer measurement series I wrote some code to detect and treat these outliers 
automatically (see "Allan deviation" below). Most probably these errors can be reduced by additional shielding and
ultimately moving from a breadboard setup to a bespoke and carefully designed PCB.

Take into account, that the OCXO will need some time to warm up und stabilize. Give it a headstart of about 20 minutes or more.
After powering up the microcontroller, one has to wait for a stable 1 pps signal. This can be identified by the onboard
LED alternating with a 1 second rhythm. The system will print out data with 9600 baud on the serial bus. This can be
read and shown either by the Arduino IDE or a terminal program. I use PuTTY for this purpose and have the program
produce a logfile with the data for later processing. Each line consist of the running measurement number followed by
the exact number of counted OCXO (or divider output) pulses during the measurement time. This number has to be adjusted
for the divider factor and the measurement time (and probably the delay effect, see below), which I do afterwards using
a spreadsheet.

Since the code uses unsigned long integers, the maximum number of counts (= 'input_frequency * measurement time') must
not exceed 4,294,967,295. For a 10 MHz OCXO with a divider by 2 this implies a maximum measurement time of about
859 seconds (otherwise the overflow has to be taken care of by hand).

With this setup, the OCXO can easily be adjusted to 0.1 Hz (1E-8) and I am very confident that 0.01 Hz (1E-9) is
achievable as well. Not bad, for hardware costing less than 20 €! Just use measurement times in excess of 20 seconds
and/or adjust for shorter times according to the formula below ("Code inherent delay").
 
Early on, it became obvious that the range of the trimmer in my OCXO kit was not sufficient to bring the frequency down
to the target frequency. The lowest I could get to was 10,000,002.4 Hz. A quick search on the WWW revealed that this is
a known issue [4] with kits from this vendor. This problem can be solved easily by changing a resistor. I decided to replace
R2 (resistor marking: 68B) with a 10k resistor, which resulted in 10,000,000.0 being right in the middle of the trimmer range.

# Code inherent delay

Ever since I came across such a timer/software implementation for latching the OCXO for the first time, I had a feeling
that there is a (very small) delay in stopping the timer. The respective sequence of the code

![Code](https://github.com/christophschwaerzler/GPSCO/assets/151140591/87d551d4-96dc-450d-941f-ee95067e6c10)

typically checks whether the timer is started first and afterwards whether the timer is stopped. This means, that stopping 
the timer is slightly delayed versus starting (plus an integer number of seconds). With the above setup I could
not only produce proove, but also quantify this effect. This allows for numerically correcting the results, leading to a significantly
improved precision for shorter measurement times.

Empirically I found that the following adjustment to the measured frequency results in the best corrected frequency:

![Adjustment formula](https://github.com/christophschwaerzler/GPSCO/assets/151140591/1026108e-9dd3-4af0-98c4-6f16276ef480)

In these equations f is the corrected frequency. It is calculated by subtracting a value fadj from the measured
frequency fmeas. The adjustment itself is a constant (2.71E-7), devided by the gate time (Tmeas) and multiplied with the measured
frequency. It is obvious that the adjustment gets smaller with increasing measurement time, which is to be expected in this case.

The constant factor corresponds to slightly more than 4 ticks of the 16 MHz clock driving the Arduino. Maybe some machine code
savvy expert on the ATmega can compare that to the time delay associated with the code?

The following graph gives measured and adjusted frequencies for a variety of measurement times. The blue dots represent the raw data without 
adjustment and the green dots give the calculated values after applying the adjustments. Whereas there is not a big difference between the 
two values for measuremt times of about 50 seconds and longer, it is very obvious that for shorter measurement times the adjustment 
significantly improves the results.

![Adjusted frequency](https://github.com/christophschwaerzler/GPSCO/assets/151140591/25fbdfb6-aa32-4daf-bea6-775d403ad26d)

# Allan deviation

With this setup in place, I could not resist and challenge myself to some rudimentary Allan deviation measurements. The following
findings may not all be scientifically 100% correct, but it was my approach to get my feet wet with a new concept.

In a first step, I had to get rid of the small number of corrupt data (see above), while maintaining the timeline of the data.
To do this in an efficient way, I wrote some code for the open statistics package "R". This code, called "Datacleaning.R" is
available in the "Allan deviation" folder. There are two modes, to be selected within the code. One replaces outlier data by
randomly picked inlier data and the other one deletes the outlier data. For further treatment with the aim of calculating
Allan deviation, one has to choose "replace".

The second step is running the code "Allan deviation" (also in the "Allan deviation" folder). Inside the code, two parameters
have to be set: "tau0" is the time between measurements in seconds prevalent in the initial data, and "minsample" the minimum
number of data points for extracted data. For my shortest measurement times which were 1 second, followed by a 1 second pause,
tau0 is 2 seconds. If the measurement time is e.g. 20 seconds and the pause 1 second, tau0 is 21. The minsample is a
parameter due to the way the sigmas are calculated for longer taus. For example, the value for a tau of 4 seconds is
efficiently derived from the same initial 2 seconds data, by skipping every second data point. Unfortunately this leaves
one with only half the number of points and accordingly a less signifacant statistic measure for sigma. This problem can be
reduced to a certain extent as there are two time series of 4 second data: 1,3,5,7,.... and 2,4,6,8,... The best estimate
for sigma(tau=4) is then the avereage of those two sigmas. Following this scheme, a tau of 8 can be calculated by again
skipping every other data point of the tau 4 data series and so on. It is clear, that after so many iterations of this
process the resulting time series are comprised of only a limited number of data points und even if there are several of
these series, the calculated sigma looses statistic significance. The parameter "minsample" defines the minimum number of
data points of a series before the program stops the process. The results are shown graphically and written in a Excel file
called "Allan_results.xlsx".

These codes enabled me to do some experimenting with my measured data. Very soon I found that my short term measurements
did not provide very satisfying values for the Allan deviation (they were in the order of 1E-7) and only from about 20
seconds of measuerement time onwards the results got better. Maybe the measurement precision for shorter times - even with the
above adjustment - is still limited.

Only then a pattern emerged which I consider significant. As the following graph shows, there is a dip in sigma around a
measurement time of about 200 seconds.

![Allan_Plot](https://github.com/christophschwaerzler/GPSCO/assets/151140591/69e55622-4e56-4dcd-a69e-fd1c56f6615d)

The associated sigma value there seems to be slightly better than 6E-9. This would imply that a measurement time of about
200 seconds is preferrable in order to get best results. As this is a very practical time and also coincides with my
gut feeling when doing the experiments, I will go for 200 seconds of measurement time in future recalibrations of the OCXO.

[1] http://www.arrl.org/files/file/QEX_Next_Issue/2015/Jul-Aug_2015/Marcus.pdf

[2] http://roland.cordesses.free.fr/GPS_Si2cor.html

[3] https://github.com/erikkaashoek/Arduino_SI5351_GPSDO

[4] https://www.qrp-labs.com/qlg1.html

[5] https://www.eevblog.com/forum/projects/cheap-chinese-ocxo-frequency-reference-modules/
