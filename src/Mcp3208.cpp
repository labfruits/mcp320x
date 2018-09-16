/**
 * @file Mcp3208.cpp
 * @author  Patrick Rogalla <patrick@labfruits.com>
 */
#include "Mcp3208.h"

MCP3208::MCP3208(uint16_t vref, uint8_t csPin, SPIClass *spi) :
    mVref(vref),
    mCsPin(csPin),
    mSpi(spi) {}

MCP3208::MCP3208(uint16_t vref, uint8_t csPin) :
    MCP3208(vref, csPin, &SPI) {}

uint16_t MCP3208::read(Channel ch) {
  // transfer spi data
  return transfer(createCmd(ch));
}

template <typename T>
void MCP3208::read(Channel ch, T *data, uint16_t num) {

  SpiData cmd;

  // create command data
  cmd = createCmd(ch);
  // transfer spi data
  for (uint16_t i=0; i < num; i++)
    data[i] = static_cast<T>(transfer(cmd));
}

uint32_t MCP3208::testSplSpeed(Channel ch) {
  return testSplSpeed(ch, 64);
}

uint32_t MCP3208::testSplSpeed(Channel ch, uint16_t num) {

  // start time
  uint32_t t1 = micros();
  // perform sampling
  for (uint16_t i = 0; i < num; i++) read(ch);
  // stop time
  uint32_t t2 = micros();

  // return average sampling speed rounded to next integer
  return ((t2 - t1) * 1000 + (num / 2)) / num;
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

MCP3208::SpiData MCP3208::createCmd(Channel ch) {
  // base command structure
  // 0b000001cccc000000
  // c: channel config
  return {
    // add channel to basic command structure
    .value = static_cast<uint16_t>((0x0400 | (ch << 6)))
  };
}

uint16_t MCP3208::transfer(SpiData cmd) {

  SpiData adc;

  // activate ADC with chip select
  digitalWrite(mCsPin, LOW);

  // send first command byte
  mSpi->transfer(cmd.hiByte);
  // send second command byte and receive first(msb) 4 bits
  adc.hiByte = mSpi->transfer(cmd.loByte) & 0x0F;
  // receive last(lsb) 8 bits
  adc.loByte = mSpi->transfer(0x00);

  // deactivate ADC with slave select
  digitalWrite(mCsPin, HIGH);

  return adc.value;
}

/*
 * Explicit template instantiation for the supported data array types.
 */
template
void MCP3208::read<uint16_t>(Channel ch, uint16_t *data, uint16_t num);

template
void MCP3208::read<uint32_t>(Channel ch, uint32_t *data, uint16_t num);

template
void MCP3208::read<float>(Channel ch, float *data, uint16_t num);

template
void MCP3208::read<double>(Channel ch, double *data, uint16_t num);
