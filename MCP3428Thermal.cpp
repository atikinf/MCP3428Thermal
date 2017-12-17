#include "MCP3428Thermal.h"
#include <assert.h> 

/* 

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
	configReg = (int) DEFAULT_CONFIG_REG;
	address = (int) DEFAULT_ADDRESS;
}

// 8 possible deviceNums:
// CASE
// 0 Adr0: 0        Adr1: 0        OR Adr0: FLOATING Adr 1: FLOATING
// 1 Adr0: 0        Adr1: FLOATING
// 3 Adr0: 0        Adr1: 1
// 4 Adr0: 1        Adr1:  0 
// 5 Adr0: 1        Adr1: FLOATING
// 6 Adr0: 1        Adr1: 1
// 7 Adr0: FLOATING Adr1: 0
// 8 Adr0: FLOATING Adr1: 1
MCP3428Thermal::MCP3428Thermal(int deviceNum, int cfg) {
	assert(0 <= deviceNum && deviceNum < 8);
	configReg = cfg;
	int deviceCodes[] = {DEVICE_ZERO, DEVICE_ONE, DEVICE_TWO, DEVICE_THREE, 
		                 DEVICE_FOUR, DEVICE_FIVE, DEVICE_SIX, DEVICE_SEVEN};
	int8_t mask = deviceCodes[deviceNum];
	address = 0b11010000 | mask;
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

int MCP3428Thermal::readRegister(int16_t *data, int channel) {
	int regNum;
	int regMap[] = {CFG_CHANNEL_ONE, CFG_CHANNEL_TWO,
		            CFG_CHANNEL_THREE, CFG_CHANNEL_FOUR};
	regNum = regMap[channel];

	// This might need to be changed. Testing pending . . .
	// Updates config register and initiates conversion
	setConfigReg(configReg & CFG_BLANK_CHANNEL_MASK | regNum);

	// Checks that two bytes are returned
	Serial.println(address);
	int byteCount = Wire.requestFrom(address, 3); // Read mode
	if (byteCount == 3) {
		Serial.println("Two bytes received, we're good.");
	} else {
		Serial.println("Something's up. Returning default values.");
		*data = 0;
		return -1;
	}

	((char*)data)[0] = Wire.read();
	((char*)data)[1] = Wire.read();
	int adcStatus = Wire.read();
	return adcStatus;
}

// Returns temperature data from specified channel (temp sensor)
double MCP3428Thermal::readTemperature(int inputNum) {
	// Unimplemented
	assert(1 <= inputNum && inputNum <= 4);
	int16_t* dataPtr;
	readRegister(dataPtr, inputNum);
	double voltage = dataToVoltage(*dataPtr);
	return voltageToTemp(voltage);
}

// Returns voltage (mV) corresponding to given 16 bit data information
double MCP3428Thermal::dataToVoltage(int16_t data) {
	int PGA = 1 << (configReg & CFG_GAIN_MASK); // Gets gain coefficient
	int dataSize = (configReg & CFG_SIZE_MASK) >> 2;
	// Resolution coef: 12 bits - 1 mV 
	float resMap[] = {RES_12, RES_14, RES_16}; // Gets resolution coefficient
    int resCoef = resMap[dataSize];
    // The equation for voltage as per the ADC specification
    return (double) data * resCoef / PGA;
}

// Takes a value in mV and returns the corresponding temperature in Celsius
double MCP3428Thermal::voltageToTemp(double voltage) {
	// Based off of TMP 36 specification
	// Two points in the voltage-temp graph: (750mV, 25C), (1000mV, 50C).
	// So the equation is C = mV / 10 - 50
	return voltage / 10 - 50;
}
