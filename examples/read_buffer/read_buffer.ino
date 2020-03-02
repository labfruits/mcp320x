/**
 * Read ADC data to buffer.
 * - connects to ADC
 * - reads multiple values from channel
 */

#include <SPI.h>
#include <Mcp320x.h>

#define SPI_CS    	2 		   // SPI slave select
#define ADC_VREF    3300     // 3.3V Vref
#define ADC_CLK     1600000  // SPI clock 1.6MHz
#define SPLS        128      // samples

uint32_t data[SPLS] = {0};

MCP3208 adc(ADC_VREF, SPI_CS);

void setup() {

  // configure PIN mode
  pinMode(SPI_CS, OUTPUT);

  // set initial PIN state
  digitalWrite(SPI_CS, HIGH);

  // initialize serial
  Serial.begin(115200);

  // initialize SPI interface for MCP3208
  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin();
  SPI.beginTransaction(settings);
}

void loop() {

  uint32_t t1;
  uint32_t t2;

  // start sampling
  Serial.println("Reading...");

  t1 = micros();
  adc.read(MCP3208::Channel::SINGLE_0, data);
  t2 = micros();

  // sampling time
  Serial.print("Samples: ");
  Serial.println(SPLS);
  Serial.print("Sampling time: ");
  Serial.print(static_cast<double>(t2 - t1) / 1000, 4);
  Serial.println("ms");

  delay(2000);
}
