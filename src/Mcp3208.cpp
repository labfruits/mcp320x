/**
 * @file Mcp3208.cpp
 * @author  Patrick Rogalla <patrick@labfruits.com>
 */
#include "Mcp3208.h"


MCP3208::MCP3208(uint16_t vref, uint8_t csPin, SPIClass *spi) {

  // set member vars
  mVref = vref;
  mCsPin = csPin;
  mSpi = spi;
}

MCP3208::MCP3208(uint16_t vref, uint8_t csPin) {

  // set member vars with default SPI interface
  mVref = vref;
  mCsPin = csPin;
  mSpi = &SPI;
}

uint16_t MCP3208::read(Channel ch) {

  AdcData data;
  // base command structure
  // 0b000001cccc000000
  // c: channel config

  // add channel to basic command structure
  data.value = 0x0400 | (ch << 6);

  // activate ADC with chip select
  digitalWrite(mCsPin, LOW);

  // send first command byte
  mSpi->transfer(data.hiByte);
  // send second command byte and receive first(msb) 4 bits
  data.hiByte = mSpi->transfer(data.loByte) & 0x0F;
  // receive last(lsb) 8 bits
  data.loByte = mSpi->transfer(0x00);

  // deactivate ADC with slave select
  digitalWrite(mCsPin, HIGH);

  return data.value;
}

uint16_t MCP3208::testSplSpeed(Channel ch, uint16_t num) {

  // start time
  uint32_t t1 = micros();
  // perform sampling
  for (uint16_t i = 0; i < num; i++) read(ch);
  // stop time
  uint32_t t2 = micros();

  // return average sampling speed rounded to next integer
  return ((t2 - t1) + (num / 2)) / num;
}

uint16_t MCP3208::toAnalog(uint16_t raw) {
  return (static_cast<uint32_t>(raw) * mVref) / kRes;
}

uint16_t MCP3208::toDigital(uint16_t val) {
  return (static_cast<uint32_t>(val) * kRes) / mVref;
}

uint16_t MCP3208::getVref() {
  return mVref;
}

uint16_t MCP3208::getAnalogRes() {
  return (static_cast<uint32_t>(mVref) * 1000) / kRes;
}
