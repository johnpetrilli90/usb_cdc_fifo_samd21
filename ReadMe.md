## USB CDC w/ Command FIFO Using Microchip Studio, ASF4, & SAMD21 XPRO
Microchip Studio version: 7.0.2594

Builds on the basic USB CDC example found here: https://github.com/johnpetrilli90/basic_usb_cdc_samd21

Adds in the command FIFO found here: https://github.com/johnpetrilli90/embedded_fifo

Adds a few commands to communicate between the PC and microcontroller over a serial terminal.

## Command Set
There are only 4 commands. The data is communicated via ASCII characters over USB CDC. ASCII characters will represent the hexadecimal data value. For example: decimal 10 = Hexidecimal 0xA = char ‘A’ = ASCII  0x41. So to communicate decimal 10, you would send char ‘A’ (ascii 0x41)

1. rr `<x>`
   - 'x' is the register number in hexadecimal
2. wr `<x>` `<y>`
   - 'x' is the register number in hexadecimal
   - 'y' is the argument or set value for the register
3. *IDN?
4. sts?

The terminating character is newline, `\n`

## Registers
There are 3 volatile dummy registers to hold values for the purpose of demonstration. These register values can be written to and read from via USB CDC.
