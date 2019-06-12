/**
 * @file Mcp320x.cpp
 * @author  Patrick Rogalla <patrick@labfruits.com>
 */
#include "Mcp320x.h"

// divide n by d and round to next integer
#define div_round(n,d) (((n) + ((d) >> 2)) / (d))

// channel configurations
using MCP3201Ch = MCP320xTypes::MCP3201::Channel;
using MCP3202Ch = MCP320xTypes::MCP3202::Channel;
using MCP3204Ch = MCP320xTypes::MCP3204::Channel;
using MCP3208Ch = MCP320xTypes::MCP3208::Channel;

template <typename T>
MCP320x<T>::MCP320x(uint16_t vref, uint8_t csPin, SPIClass *spi)
  : mVref(vref)
  , mCsPin(csPin)
  , mSplSpeed(0)
  , mSpi(spi) {}

template <typename T>
MCP320x<T>::MCP320x(uint16_t vref, uint8_t csPin)
  : MCP320x(vref, csPin, &SPI) {}

template <typename T>
void MCP320x<T>::calibrate(Channel ch)
{
  mSplSpeed = testSplSpeed(ch, 256);
}

template <typename T>
uint16_t MCP320x<T>::read(Channel ch) const
{
  return transfer(createCmd(ch));
}

template <typename T>
uint32_t MCP320x<T>::testSplSpeed(Channel ch) const
{
  return testSplSpeed(ch, 64);
}

template <typename T>
uint32_t MCP320x<T>::testSplSpeed(Channel ch, uint16_t num) const
{
  // start time
  uint32_t t1 = micros();
  // perform sampling
  for (uint16_t i = 0; i < num; i++) read(ch);
  // stop time
  uint32_t t2 = micros();

  // return average sampling speed
  return div_round((t2 - t1) * 1000, num);
}

template <typename T>
uint32_t MCP320x<T>::testSplSpeed(Channel ch, uint16_t num, uint32_t splFreq)
{
  // required delay
  uint16_t delay = getSplDelay(ch, splFreq);

  // start time
  uint32_t t1 = micros();
  // perform sampling
  for (uint16_t i = 0; i < num; i++) {
    read(ch);
    delayMicroseconds(delay);
  }
  // stop time
  uint32_t t2 = micros();

  // return average sampling speed
  return div_round((t2 - t1) * 1000, num);
}

template <typename T>
uint16_t MCP320x<T>::toAnalog(uint16_t raw) const
{
  return (static_cast<uint32_t>(raw) * mVref) / (kRes - 1);
}

template <typename T>
uint16_t MCP320x<T>::toDigital(uint16_t val) const
{
  return (static_cast<uint32_t>(val) * (kRes - 1)) / mVref;
}

template <typename T>
uint16_t MCP320x<T>::getVref() const
{
  return mVref;
}

template <typename T>
uint16_t MCP320x<T>::getAnalogRes() const
{
  return (static_cast<uint32_t>(mVref) * 1000) / (kRes - 1);
}

template <typename T>
uint16_t MCP320x<T>::getSplDelay(Channel ch, uint32_t splFreq)
{
  // requested sampling period (ns)
  uint32_t splTime = div_round(1000000000, splFreq);

  // measure speed if uncalibrated
  if (!mSplSpeed) calibrate(ch);

  // calculate delay in us
  int16_t delay =  (splTime - mSplSpeed) / 1000;
  return (delay < 0) ? 0 : static_cast<uint16_t>(delay);
}

template <>
MCP3201::Command<MCP3201Ch> MCP3201::createCmd(MCP3201Ch ch)
{
  // no command required
  return { .value = 0x0 };
}

template <>
MCP3202::Command<MCP3202Ch> MCP3202::createCmd(MCP3202Ch ch)
{
  // base command structure
  // 0b00000001cc100000
  // c: channel config
  return {
    // add channel to basic command structure
    .value = static_cast<uint16_t>((0x0120 | (ch << 6)))
  };
}

template <>
MCP3204::Command<MCP3204Ch> MCP3204::createCmd(MCP3204Ch ch)
{
  // base command structure
  // 0b000001cxcc000000
  // c: channel config
  return {
    // add channel to basic command structure
    .value = static_cast<uint16_t>((0x0400 | (ch << 6)))
  };
}

template <>
MCP3208::Command<MCP3208Ch> MCP3208::createCmd(MCP3208Ch ch)
{
  // base command structure
  // 0b000001cccc000000
  // c: channel config
  return {
    // add channel to basic command structure
    .value = static_cast<uint16_t>((0x0400 | (ch << 6)))
  };
}

template <>
uint16_t MCP3201::execute(Command<MCP3201Ch> cmd) const
{
  return transfer();
}

template <>
uint16_t MCP3202::execute(Command<MCP3202Ch> cmd) const
{
  return transfer(cmd);
}

template <>
uint16_t MCP3204::execute(Command<MCP3204Ch> cmd) const
{
  return transfer(cmd);
}

template <>
uint16_t MCP3208::execute(Command<MCP3208Ch> cmd) const
{
  return transfer(cmd);
}

template <typename T>
uint16_t MCP320x<T>::transfer() const
{
  SpiData adc;

  // activate ADC with chip select
  digitalWrite(mCsPin, LOW);

  // receive first(msb) 5 bits
  adc.hiByte = mSpi->transfer(0x00) & 0x1F;
  // receive last(lsb) 8 bits
  adc.loByte = mSpi->transfer(0x00);

  // deactivate ADC with slave select
  digitalWrite(mCsPin, HIGH);

  // correct bit offset
  // |x|x|x|11|10|9|8|7| |6|5|4|3|2|1|0|1
  return (adc.value >> 1);
}

template <typename T>
uint16_t MCP320x<T>::transfer(SpiData cmd) const
{
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
 * Explicit template instantiation for the channel types.
 */
template class MCP320x<MCP3201Ch>;
template class MCP320x<MCP3202Ch>;
template class MCP320x<MCP3204Ch>;
template class MCP320x<MCP3208Ch>;
