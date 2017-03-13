# Arduino library for MCP3208/3204 ADC

This library provides an interface to communicate with the MCP3208/3204 12 bit ADC over SPI.

## Hardware

The library is implemented for the 8 channel (MCP3208) version but is fully
compatible with the 4 channel (MCP3204) version.

When used with the 4 channel version the available channels are limited to
the first 4 single channels (SINGLE_0-4) and the first 2 differential
channels (DIFF_0/1).

## License

This code is released under the MIT License.
