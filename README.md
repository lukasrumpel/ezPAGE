# ezPOC8
POCSAG transmitter by DO4LR

This is the repository of my POCSAG transmitter for multi-purpose use. You can use it with ezPAGE to connect it to the DAPNET (more information in its own folder), embedd it to a project or many other uses. The Software is open source. With a Poweramplifier it is a full-scale basestation transmitter. The Frequency coverage is from 142 to 1050 MHz.

The POCSAG generating library is based on ON1ARFs, but strongly modified (original won't work for ezPOC8). You can generate up to 76 characters per POCSAG message (depending on RIC). The library for the tansmitter module is a modified RadioHead by AirSpayce, please use the one in this repository, otherwise it won't work as a POCSAG Transmitter. 

ezPOC8 can be connected via USB or TTL UART. It is powered over USB or an external powersource. 

The best: it can be build for under 20€!
