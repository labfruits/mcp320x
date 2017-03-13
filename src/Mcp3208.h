/**
 * @file Mcp3208.h
 * @author Patrick Rogalla <patrick@labfruits.com>
 *
 * Interface for the Microchip MSP3208/3204 12 bit ADC.
 * The class is implemented for the 8 channel version. But the 4 channel
 * version is fully supported and only limmeted by the number of channles.
 */
#ifndef MCP3208_H_
#define MCP3208_H_

#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>

class MCP3208 {

public:

  /** ADC resolution in bits. */
  static const uint8_t kResBits = 12;
  /** ADC resolution. */
  static const uint16_t kRes = (1 << kResBits);

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

  /**
   * Initiates a MCP3208 object. The chip select pin must be already
   * configured as output.
   * @param [in] vref ADC reference voltage in mV.
   * @param [in] csPin pin number to use for chip select.
   * @param [in] spi reference to the SPI interface to use.
   */
  MCP3208(uint16_t vref, uint8_t csPin, SPIClass *spi);

  /**
   * Initiates a MCP3208 object. The chip select pin must be already
   * configured as output. The default SPI interface will be used for
   * communication.
   * @param [in] vref the ADC reference voltage in mV.
   * @param [in] csPin the pin number to use for chip select.
   */
  MCP3208(uint16_t vref, uint8_t csPin);

  /**
   * Reads the supplied channel. The SPI interface must be initialized and
   * put in a usable state before calling this function.
   * @param [in] ch defines the channel to read from.
   * @return the converted raw value.
   */
  uint16_t read(Channel ch);

  /**
   * Performs a sampling speed test. The SPI interface must be initialized
   * and put in a usable state before calling this function.
   * @param [in] ch the channel to use for the speed test.
   * @param [in] num the number of reads to perform.
   * @return the average sampling time needed for one read in us.
   */
  uint16_t testSplSpeed(Channel ch, uint16_t num);

  /**
   * Converts the supplied raw value to an analog value in mV based on
   * the defined reference voltage.
   * @param [in] raw the sampled ADC value.
   * @return the converted analog value in mV.
   */
  uint16_t toAnalog(uint16_t raw);

  /**
   * Converts the supplied analog value to the digital representation
   * based on the defined reference voltage.
   * @param [in] val the analog value in mV.
   * @return the digital representation off the supplied analog value.
   */
  uint16_t toDigital(uint16_t val);

  /**
   * Returns the reference voltage.
   * @return the configured reference voltage in mV.
   */
  uint16_t getVref();

  /**
   * Returns the analog resolution in µV based on the defined
   * reference voltage.
   * @return the analog resolution in µV.
   */
  uint16_t getAnalogRes();

private:

  uint16_t mVref;
  uint8_t mCsPin;
  SPIClass *mSpi;

  /**
   * Defines 16 bit data. The structure implements an easy
   * access to each byte.
   */
  union AdcData {
    uint16_t value;  /**< value */
    struct {
      uint8_t loByte;  /**< low byte */
      uint8_t hiByte;  /**< high byte */
    };
  };
};

#endif // MCP3208_H_
