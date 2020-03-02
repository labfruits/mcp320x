/**
 * @file Mcp320x.h
 * @author Patrick Rogalla <patrick@labfruits.com>
 *
 * Interface for the Microchip MCP3208/3204/3202/3201 12 bit ADC.
 * The class is implemented for all available channel versions of the chip.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include <SPI.h>

namespace MCP320xTypes {

namespace MCP3201 {
  /**
   * The ADC has 1 fixed non configurable input.
   */
  enum Channel {
    SINGLE_0 = 0x0  /**< single channel 0 */
  };
};

namespace MCP3202 {
  /**
   * The ADC has 2 inputs, usable as 2 individually single inputs with
   * reference to analog ground or 1 differential input. The polarity
   * of each differential input can be configured.
   *
   * bits|  2  | 1
   * :--:|:---:|:-:
   *  -  |type |ch
   *
   *  -  |      description
   * :--:|:--------------------------
   * type| 1 bit for single(1) or differential(0) input
   * ch  | 1 bit for channel number selection
   */
  enum Channel {
    SINGLE_0 = 0b10,  /**< single channel 0 */
    SINGLE_1 = 0b11,  /**< single channel 1 */
    DIFF_0PN = 0b00,  /**< differential channel 0 (input 0+,1-) */
    DIFF_0NP = 0b01,  /**< differential channel 0 (input 0-,1+) */
  };
};

namespace MCP3204 {
  /**
   * The ADC has 4 inputs, usable as 4 individually single inputs with
   * reference to analog ground or 2 differential inputs. The polarity
   * of each differential input can be configured.
   *
   * bits|  4  | 3 | 2 | 1
   * :--:|:---:|:-:|:-:|:-:
   *  -  |type | x |ch |ch
   *
   *  -  |      description
   * :--:|:--------------------------
   * type| 1 bit for single(1) or differential(0) input
   * x   | 1 bit unused
   * ch  | 2 bit for channel number selection
   */
  enum Channel {
    SINGLE_0 = 0b1000,  /**< single channel 0 */
    SINGLE_1 = 0b1001,  /**< single channel 1 */
    SINGLE_2 = 0b1010,  /**< single channel 2 */
    SINGLE_3 = 0b1011,  /**< single channel 3 */
    DIFF_0PN = 0b0000,  /**< differential channel 0 (input 0+,1-) */
    DIFF_0NP = 0b0001,  /**< differential channel 0 (input 0-,1+) */
    DIFF_1PN = 0b0010,  /**< differential channel 1 (input 2+,3-) */
    DIFF_1NP = 0b0011   /**< differential channel 1 (input 2-,3+) */
  };
};

namespace MCP3208 {
  /**
   * The ADC has 8 inputs, usable as 8 individually single inputs with
   * reference to analog ground or 4 differential inputs. The polarity
   * of each differential input can be configured.
   *
   * bits|  4  | 3 | 2 | 1
   * :--:|:---:|:-:|:-:|:-:
   *  -  |type |ch |ch |ch
   *
   *  -  |      description
   * :--:|:--------------------------
   * type| 1 bit for single(1) or differential(0) input
   * ch  | 3 bit for channel number selection
   */
  enum Channel {
    SINGLE_0 = 0b1000,  /**< single channel 0 */
    SINGLE_1 = 0b1001,  /**< single channel 1 */
    SINGLE_2 = 0b1010,  /**< single channel 2 */
    SINGLE_3 = 0b1011,  /**< single channel 3 */
    SINGLE_4 = 0b1100,  /**< single channel 4 */
    SINGLE_5 = 0b1101,  /**< single channel 5 */
    SINGLE_6 = 0b1110,  /**< single channel 6 */
    SINGLE_7 = 0b1111,  /**< single channel 7 */
    DIFF_0PN = 0b0000,  /**< differential channel 0 (input 0+,1-) */
    DIFF_0NP = 0b0001,  /**< differential channel 0 (input 0-,1+) */
    DIFF_1PN = 0b0010,  /**< differential channel 1 (input 2+,3-) */
    DIFF_1NP = 0b0011,  /**< differential channel 1 (input 2-,3+) */
    DIFF_2PN = 0b0100,  /**< differential channel 2 (input 4+,5-) */
    DIFF_2NP = 0b0101,  /**< differential channel 2 (input 5-,5+) */
    DIFF_3PN = 0b0110,  /**< differential channel 3 (input 6+,7-) */
    DIFF_3NP = 0b0111   /**< differential channel 3 (input 6-,7+) */
  };
};

}; // namespace MCP320xTypes

template <typename ChannelType>
class MCP320x {

public:

  /** ADC resolution in bits. */
  static const uint8_t kResBits = 12;
  /** ADC resolution. */
  static const uint16_t kRes = (1 << kResBits);

  /** ADC Channel configuration. */
  using Channel = ChannelType;

  /**
   * Initiates a MCP320x object. The chip select pin must be already
   * configured as output.
   * @param [in] vref ADC reference voltage in mV.
   * @param [in] csPin pin number to use for chip select.
   * @param [in] spi reference to the SPI interface to use.
   */
  MCP320x(uint16_t vref, uint8_t csPin, SPIClass *spi);

  /**
   * Initiates a MCP320x object. The chip select pin must be already
   * configured as output. The default SPI interface will be used for
   * communication.
   * @param [in] vref the ADC reference voltage in mV.
   * @param [in] csPin the pin number to use for chip select.
   */
  MCP320x(uint16_t vref, uint8_t csPin);

  /**
   * Calibrates read timing using the supplied channel. A calibration
   * should be performed after evey SPI frequency changes or other events
   * that could have an impact on the sampling speed.
   * The SPI interface must be initialized and put in a usable state
   * before calling this function.
   * @param [in] ch defines the channel to use for calibration.
   */
  void calibrate(Channel ch);

  /**
   * Reads the supplied channel. The SPI interface must be initialized and
   * put in a usable state before calling this function.
   * @param [in] ch defines the channel to read from.
   * @return the converted raw value.
   */
  uint16_t read(Channel ch) const;

  /**
   * Reads the supplied channel and stores the data in the supplied
   * data array. The SPI interface must be initialized and put in a
   * usable state before calling this function.
   * @param [in] ch defines the channel to read from.
   * @param [out] data array to store the values.
   */
  template <typename T, size_t N>
  void read(Channel ch, T (&data)[N]) const
  {
    readn(ch, data, N);
  }

  /**
   * Reads the supplied channel limited to the specified frequency and
   * stores the data in the supplied data array. The sample rate limit
   * is software controlled, and has a low precision.
   * The SPI interface must be initialized and put in a usable state
   * before calling this function.
   * @param [in] ch defines the channel to read from.
   * @param [out] data array to store the values.
   * @param [in] splFreq sample frequency limit in hz.
   */
  template <typename T, size_t N>
  void read(Channel ch, T (&data)[N], uint32_t splFreq)
  {
    readn(ch, data, N, splFreq);
  }

  /**
   * Reads the supplied channel and stores the data in the supplied
   * data array after the predicate is true. The SPI interface must be
   * initialized and put in a usable state before calling this function.
   * @param [in] ch defines the channel to read from.
   * @param [out] data array to store the values.
   * @param [in] p predicate funtion to control sampling start.
   */
  template <typename T, size_t N, typename Predicate>
  void read_if(Channel ch, T (&data)[N], Predicate p) const
  {
    readn_if(ch, data, N, p);
  }

  /**
   * Reads the supplied channel limited to the specified frequency and
   * stores the data in the supplied data array after the predicate is true.
   * The sample rate limit is software controlled, and has a low precision.
   * The SPI interface must be initialized and put in a usable state
   * before calling this function.
   * @param [in] ch defines the channel to read from.
   * @param [out] data array to store the values.
   * @param [in] splFreq sample frequency limit in hz.
   * @param [in] p predicate funtion to control sampling start.
   */
  template <typename T, size_t N, typename Predicate>
  void read_if(Channel ch, T (&data)[N], uint32_t splFreq, Predicate p)
  {
    readn_if(ch, data, N, splFreq, p);
  }

  /**
   * Reads the supplied channel and stores N values in the supplied
   * data array. The SPI interface must be initialized and put in a
   * usable state before calling this function.
   * @param [in] ch defines the channel to read from.
   * @param [out] data array to store the values.
   * @param [in] num number of reads. The data array needs to be
   * at least that size.
   */
  template <typename T>
  void readn(Channel ch, T *data, uint16_t num) const
  {
    execute(createCmd(ch), data, num);
  }

  /**
   * Reads the supplied channel limited to the specified frequency and
   * stores N values in the supplied data array. The sample rate limit
   * is software controlled, and has a low precision.
   * The SPI interface must be initialized and put in a usable state
   * before calling this function.
   * @param [in] ch defines the channel to read from.
   * @param [out] data array to store the values.
   * @param [in] num number of reads. The data array needs to be
   * at least that size.
   * @param [in] splFreq sample frequency limit in hz.
   */
  template <typename T>
  void readn(Channel ch, T *data, uint16_t num, uint32_t splFreq)
  {
    execute(createCmd(ch), data, num, getSplDelay(ch, splFreq));
  }

  /**
   * Reads the supplied channel and stores N values in the supplied
   * data array after the predicate is true. As long as the predicate
   * is false, the function keeps sampling without storing any data.
   * The SPI interface must be initialized and put in a usable state
   * before calling this function.
   * @param [in] ch defines the channel to read from.
   * @param [out] data array to store the values.
   * @param [in] num number of reads. The data array needs to be
   * at least that size.
   * @param [in] p predicate funtion to control sampling start.
   */
  template <typename T, typename Predicate>
  void readn_if(Channel ch, T *data, uint16_t num, Predicate p) const
  {
    auto cmd = createCmd(ch);
    while (!p(execute(cmd))) {}
    execute(cmd, data, num);
  }

  /**
   * Reads the supplied channel limited to the specified frequency and
   * stores N values in the supplied data array after the predicate
   * is true. As long as the predicate is false, the function keeps
   * sampling without storing any data.
   * The SPI interface must be initialized and put in a usable state
   * before calling this function.
   * @param [in] ch defines the channel to read from.
   * @param [out] data array to store the values.
   * @param [in] num number of reads. The data array needs to be
   * at least that size.
   * @param [in] splFreq sample frequency limit in hz.
   * @param [in] p predicate funtion to control sampling start.
   */
  template <typename T, typename Predicate>
  void readn_if(Channel ch, T *data, uint16_t num, uint32_t splFreq,
    Predicate p)
  {
    auto cmd = createCmd(ch);
    while (!p(execute(cmd))) {}
    execute(cmd, data, num, getSplDelay(ch, splFreq));
  }

  /**
   * Performs a sampling speed test over 64 reads. The SPI interface
   * must be initialized and put in a usable state before
   * calling this function.
   * @param [in] ch the channel to use for the speed test.
   * @return the average sampling time needed for one sample in ns.
   */
  uint32_t testSplSpeed(Channel ch) const;

  /**
   * Performs a sampling speed test. The SPI interface must be initialized
   * and put in a usable state before calling this function.
   * @param [in] ch the channel to use for the speed test.
   * @param [in] num the number of reads to perform.
   * @return the average sampling time needed for one sample in ns.
   */
  uint32_t testSplSpeed(Channel ch, uint16_t num) const;

  /**
   * Performs a sampling speed test limited to the specified frequency.
   * The SPI interface must be initialized and put in a usable state
   * before calling this function.
   * @param [in] ch the channel to use for the speed test.
   * @param [in] num the number of reads to perform.
   * @param [in] splFreq sample frequency limit in hz.
   * @return the average sampling time needed for one sample in ns.
   */
  uint32_t testSplSpeed(Channel ch, uint16_t num, uint32_t splFreq);

  /**
   * Converts the supplied raw value to an analog value in mV based on
   * the defined reference voltage.
   * @param [in] raw the sampled ADC value.
   * @return the converted analog value in mV.
   */
  uint16_t toAnalog(uint16_t raw) const;

  /**
   * Converts the supplied analog value to the digital representation
   * based on the defined reference voltage.
   * @param [in] val the analog value in mV.
   * @return the digital representation off the supplied analog value.
   */
  uint16_t toDigital(uint16_t val) const;

  /**
   * Returns the reference voltage.
   * @return the configured reference voltage in mV.
   */
  uint16_t getVref() const;

  /**
   * Returns the analog resolution in µV based on the defined
   * reference voltage.
   * @return the analog resolution in µV.
   */
  uint16_t getAnalogRes() const;

private:

  /**
   * Defines 16 bit SPI data. The structure implements an easy
   * access to each byte.
   */
  union SpiData {
    uint16_t value;  /**< value */
    struct {
      uint8_t loByte;  /**< low byte */
      uint8_t hiByte;  /**< high byte */
    };
  };

  /**
   * SPI command for a specific channel type.
   */
  template <typename>
  using Command = SpiData;

  /**
   * Returns the required delay for the requested sample rate.
   * @param [in] ch the channel to create the command for.
   * @return the delay in us.
   */
  uint16_t getSplDelay(Channel ch, uint32_t splFreq);

  /**
   * Creates a command from the supplied channel.
   * @param [in] ch the channel to create the command for.
   * @return the SPI data.
   */
  static Command<Channel> createCmd(Channel ch);

  /**
   * Executes the supplied command.
   * @param [in] cmd the command to execute.
   * @return the ADC value from the SPI response.
   */
  uint16_t execute(Command<Channel> cmd) const;

  /**
   * Executes the supplied command for the requested number
   * of samples.
   * @param [in] cmd the command to execute.
   * @param [out] data array to store the values.
   * @param [in] num number of reads. The data array needs to be
   * at least that size.
   */
  template <typename T>
  void execute(Command<Channel> cmd, T *data, uint16_t num) const
  {
    for (decltype(num) i=0; i < num; i++)
      data[i] = static_cast<T>(execute(cmd));
  }

  /**
   * Executes the supplied command for the requested
   * number of samples with delay between reads.
   * @param [in] cmd the command to execute.
   * @param [out] data array to store the values.
   * @param [in] num number of reads. The data array needs to be
   * at least that size.
   * @param [in] delay in us.
   */
  template <typename T>
  void execute(Command<Channel> cmd, T *data, uint16_t num,
    uint16_t delay) const
  {
    for (decltype(num) i=0; i < num; i++) {
      data[i] = static_cast<T>(execute(cmd));
      delayMicroseconds(delay);
    }
  }

  /**
   * Transfers without SPI command data.
   * @return the ADC value from the SPI response.
   */
  uint16_t transfer() const;

  /**
   * Transfers the supplied SPI command data.
   * @param [in] cmd the SPI command data to transfer.
   * @return the ADC value from the SPI response.
   */
  uint16_t transfer(SpiData cmd) const;

private:

  uint16_t mVref;
  uint8_t mCsPin;
  uint32_t mSplSpeed;
  SPIClass *mSpi;
};

using MCP3201 = MCP320x<MCP320xTypes::MCP3201::Channel>;
using MCP3202 = MCP320x<MCP320xTypes::MCP3202::Channel>;
using MCP3204 = MCP320x<MCP320xTypes::MCP3204::Channel>;
using MCP3208 = MCP320x<MCP320xTypes::MCP3208::Channel>;
