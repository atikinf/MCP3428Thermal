#ifndef MCP3428_THERMAL_H
#define MCP3428_THERMAN_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else 
	#include "WProgram.h"
#endif

#include <Wire.h>

#define DEVICE_ZERO 0b0000
#define DEVICE_ONE 0b0010
#define DEVICE_TWO 0b0100
#define DEVICE_THREE 0b1000
#define DEVICE_FOUR 0b1010
#define DEVICE_FIVE 0b1100
#define DEVICE_SIX 0b0110
#define DEVICE_SEVEN 0b1110

// Config byte components: 
#define CFG_READY_BIT 0b10000000
#define CFG_UNREADY_BIT 0b00000000

#define CFG_BLANK_CHANNEL_MASK 0b10011111
#define CFG_CHANNEL_ONE 0b00000000
#define CFG_CHANNEL_TWO 0b00100000
#define CFG_CHANNEL_THREE 0b01000000
#define CFG_CHANNEL_FOUR 0b01100000
// DEFAULT vvv
#define CFG_CONT_MODE 0b00000000
#define CFG_SS_MODE 0b00010000
// DEFAULT vvv
//   12 bits (DEFAULT by specification)
#define CFG_12 0b00000000 
//   14 bits 
#define CFG_14 0b00000100
//   16 bits (Preferred for extra precision)
#define CFG_16 0b00001000
#define CFG_SIZE_MASK 0b00001100
// In 16 bit mode, D15 - 8 (1st data byte) (D15 is sign bit)
//                 D7  - 0 (2nd data byte)  

// Gain x1
#define CFG_GAIN_ONE 0b00000000
// Gain x2 
#define CFG_GAIN_TWO 0b00000001 
// Gain x4
#define CFG_GAIN_THREE 0b00000010 
// Gain x8
#define CFG_GAIN_FOUR 0b00000011 
#define CFG_GAIN_MASK 0b00000011

#define DEFAULT_CONFIG_REG 0b10001000

// Default MCP3248 I2C Address
/* IMPORTANT:
      Leave the Address Pins Floating (otherwise the Address Bits will
                                       will need to be adjusted)
      This is only important if you have  multiple devices on the same 
                                       I2C line
*/
#define DEFAULT_ADDRESS 0x1101000
//                        ^^^^      // MCP3428 Device Code
//                            ^^^   // Address Bits
//                               ^  // Read/Write Bit (Write by DEFAULT)
#define ADDRESS_READING 0b00000001 // Read
#define ADDRESS_WRITING 0b00000000 // Write

// Resolution values:
#define RES_12 1       // 12 bit resolution setting
#define RES_14 0.25    // 14 bit 
#define RES_16 0.0625  // 16 bit 

class MCP3428Thermal
{
	public:
		// Constructors
		MCP3428Thermal();
		MCP3428Thermal(int deviceNum, int cfg);
		
		// Tests
		bool testConnection(void);

		// Address Selection 
		// >>Not going to be used very often
		void setAddress(int addr);
		int getAddress(void);

		// Config Register Methods
		void setConfigReg(int data);
		int getConfigReg(void);

		// Read Methods
		// Gets two data bytes for the specified channel
		int readRegister(int16_t *data, int channel); 

		// Returns temperature of chosen thermometer (by input)
		double readTemperature(int inputNum);
	private:
		bool updateADCConfigByte(int dataByte);
		double dataToVoltage(int16_t data);
		double voltageToTemp(double voltage);
		int configReg; 
		int address;
};

#endif 