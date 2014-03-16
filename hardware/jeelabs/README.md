# IDE-hardware

Extensions to the Arduino IDE 1.5.x series to support additional hardware.

This package adds a "JeeLabs AVR Boards" entry to the Tools -> Boards menu.

# Installation and use

* Make sure your IDE's "Sketchbook location" contains a `hardware` folder
* Download or "git clone" this project at <https://github.com/jcw/ide-hardware>
* Rename the download to `jeelabs` (!) and put it inside that `hardware` folder
* Restart the Arduino IDE if it was still running, so it'll pick up the change
* You should see a new "JeeLabs AVR boards" entry in the Tools -> Board menu
* Select Tools -> Programmer -> Arduino as ISP
* See [this post][1] on the JeeLabs weblog for ISP programming a JeeNode Micro
* To set the fuses and optional boot loader, use Tools -> Burn Bootloader
* To upload a sketch, you can then use the standard "Upload" button

  [1]: http://jeelabs.org/2013/03/21/programming-the-jnµ-at-last/

# License

GNU Lesser GPL, same as the original code (see below).

# Credits

This code was adapted from <http://code.google.com/p/arduino-tiny/>:

> Arduino-Tiny is an open source set of ATtiny "cores" for the Arduino platform.
> 
> The Arduino platform currently supports Atmel ATmega processors. There is a need for the Arduino platform to work with physically smaller DIP package processors. The intent of this project is fulfill that need. Specifically, our goal is to provide a core that enables Arduino users to work with the ATtiny84 (84/44/24), ATtiny85 (85/45/25), and ATtiny2313 processors.

More information is available in `avr/readme.txt`:

> Arduino-Tiny is based on work by David A. Mellis, René Bohne, R. Wiersma, 
Alessandro Saporetti, and Brian Cook.

The main change was to move things around to insert an extra `avr` folder and
to include the necessary boards.txt + platform.txt + programmers.txt files.
