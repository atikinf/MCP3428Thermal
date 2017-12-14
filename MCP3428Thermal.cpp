#include "MCP3428Thermal.h"
#include <assert.h> 

/* Configuration Register Description:
	bit 7:   Ready Bit - [1] initializes new conversion (if in one shot mode)
	                     [0] no effect
	bit 6-5: Channel Selection Bits - Select [00] Channel 1 Default
	                                         [01] Channel 2
	                                         [10] Channel 3
	                                         [11] Channel 4  
	bit 4:   Conversion Mode Bit - [1] Continuous Conversion
	                               [0] One Shot Conversion
	bit 3-2: Sample Rate Selection Bit - [00] 240 SPS (12 bits) Default
	                                     [01] 60  SPS (14 bits)
	                                     [10] 15  SPS (16 bits) 
    bit 1-0: PGA Gain Selection Bits [00] x1  Default (NOT SUPER IMPORTANT)
*/

// Constructors:

// 3 Custom address bits are '000' by default
// NOTE: This assumes that the two address pins are left floating
MCP3428Thermal::MCP3428Thermal() {
	configReg = DEFAULT_CONFIG_REG;
	address = DEFAULT_ADDRESS;
}

// 8 possible deviceNums:
// CASE
// 0 Adr0: 0 Adr1: 0        OR Adr0: FLOATING Adr 1: FLOATING
// 1 Adr0: 0 Adr1: FLOATING
// 3 Adr0: 0 Adr1: 1
// 4 Adr0: 1Adr1:  0 
// 5 Adr0: 1 Adr1: FLOATING
// 6 Adr0: 1 Adr1: 1
// 7 Adr0: FLOATING Adr1: 0
// 8 Adr0: FLOATING Adr1: 1
MCP3428Thermal::MCP3428Thermal(int deviceNum, int cfg) {
	assert(0 <= deviceNum && deviceNum < 8);
	configReg = cfg;
	int deviceCodes[] = [DEVICE_ZERO, DEVICE_ONE, DEVICE_THREE, DEVICE_FOUR, 
	                     DEVICE_FIVE, DEVICE_SIX, DEVICE_SEVEN, DEVICE_EIGHT];
	int8_t mask = deviceCodes[deviceNum];
	address = 0b10110000 | mask;
}

// Returns true if connection is successful, false otherwise
bool MCP3428Thermal::testConnection(void) {
	Wire.beginTransmission(address);
	return Wire.endTransmission() == 0;
	/* endTransmission() returns a 0 if the transmission was successful 
		and some other value otherwise
	*/
}

// Updates the ADC's config byte to the given dataByte
void MCP3428Thermal::setConfigReg(int dataByte) {
	configReg = dataByte;
	updateADCConfigByte(dataByte); 
}

// HELPER func, updates the config byte (generally before proceeding to read)
bool MCP3428Thermal::updateADCConfigByte(int dataByte) {
	Wire.beginTransmission(address);
	Wire.write(dataByte);
	return Wire.endTransmission() == 0;
}

int MCP3428Thermal::getConfigReg(void) {
	return configReg;
}

int MCP3428Thermal::readRegister(int16_t *data, int reg) {
	int regNum;
	int regMap[] = [CFG_CHANNEL_ONE, CFG_CHANNEL_TWO,
		            CFG_CHANNEL_THREE, CFG_CHANNEL_FOUR];
	regNum = regMap[reg];

	// Updates config register and initiates conversion
	setConfigReg(configReg & CFG_BLANK_CHANNEL_MASK | regNum);

	// Checks that two bytes are returned
	int byteCount = Wire.requestFrom(address | ADDRESS_READING, 3); // Read mode
	if (byteCount == 3) {
		Serial.println("Two bytes received, we're good.");
	} else {
		Serial.println("Something's up.");
		return -1;
	}

	((char*)data)[0] = Wire.read();
	((char*)data)[1] = Wire.read();
	int adcStatus = Wire.read();
	return adcStatus;
}

// Returns temperature data from specified sensor
double MCP3428Thermal::readTemperature(int inputNum) {
	// Unimplemented
	return -1;
}
