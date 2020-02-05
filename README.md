Digital Stopwatch using:
1- TIMER1 in CTC Mode in AVR ATMEGA16
2- F_CPU = 1 MHz and N=64 (Pre-scalar)
3- Multiplexed 6 Common Anode 7-segments display with 2 7-segment displays for each of seconds, minutes, hours and BCD to 7-segment decoder (7447)
4- Switch on INT0 to reset the clock (Falling edge using internal pull-up resistor)
5- Switch on INT1 to pause the clock (Rising edge using external pull-down resistor)
6- Switch on INT2 to resume the clock (Falling edge using internal pull-up resistor)
7- PORTA from PA0 to PA5 to Base pins for each transistor to enable 7-segment displays
8- PORTC from PC0 to PC3 to 7447 decoder to output the time digits