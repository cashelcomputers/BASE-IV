# BASE-IV
Buchanan and Son Electronics Model IV function generator

This repo will contain software and schematics for a project I did in year 2 of my BEngTech(Elec) degree. Currently, the source code is available, schematics will be uploaded soon. It is a benchtop DIY function generator, specifications are below. The design was sucessfully build and demonstrated for assessment. Some issues were found during testing, please consult the build, design and test report (available soon).

KiCad schematics and PCB design files will be made available early December. Some minor tweaks need to be performed, once exams are finished :)

The function generator uses the following hardware (summary only):

- AD9833 DDS VFO module
- Arduino Pro Mini (programmed as an AVR, not Arduino)
- Analog amplifier section build using op-amps
- MAX7221 7 segment display driver, driving 8 digits

A full design, build and test report will be made available soon.

## Specifications (as designed):
- Max frequency: 5 MHz (depends on selected op-amps, up to 12.5 MHz possible)
- Sine, square and triangle waves
- 10Vp-p max amplitude
- ± 5V DC offset
- Linear sweep

