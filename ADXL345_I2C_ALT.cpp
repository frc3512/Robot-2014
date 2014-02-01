/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "ADXL345_I2C_ALT.h"
#include "DigitalModule.h"
#include "NetworkCommunication/UsageReporting.h"
#include "I2C.h"

const uint8_t ADXL345_I2C_ALT::kAddress;
const uint8_t ADXL345_I2C_ALT::kPowerCtlRegister;
const uint8_t ADXL345_I2C_ALT::kDataFormatRegister;
const uint8_t ADXL345_I2C_ALT::kDataRegister;
constexpr double ADXL345_I2C_ALT::kGsPerLSB;

/**
 * Constructor.
 *
 * @param moduleNumber The digital module that the sensor is plugged into (1 or 2).
 * @param range The range (+ or -) that the accelerometer will measure.
 */
ADXL345_I2C_ALT::ADXL345_I2C_ALT(uint8_t moduleNumber, ADXL345_I2C_ALT::DataFormat_Range range)
	: m_i2c (NULL)
{
	DigitalModule *module = DigitalModule::GetInstance(moduleNumber);
	if (module)
	{
		m_i2c = module->GetI2C(kAddress);

		// Turn on the measurements
		m_i2c->Write(kPowerCtlRegister, kPowerCtl_Measure);
		// Specify the data format to read
		m_i2c->Write(kDataFormatRegister, kDataFormat_FullRes | (uint8_t)range);

		nUsageReporting::report(nUsageReporting::kResourceType_ADXL345, nUsageReporting::kADXL345_I2C, moduleNumber - 1);
	}
}

/**
 * Destructor.
 */
ADXL345_I2C_ALT::~ADXL345_I2C_ALT()
{
	delete m_i2c;
	m_i2c = NULL;
}

/**
 * Get the acceleration of one axis in Gs.
 *
 * @param axis The axis to read from.
 * @return Acceleration of the ADXL345 in Gs.
 */
double ADXL345_I2C_ALT::GetAcceleration(ADXL345_I2C_ALT::Axes axis)
{
	int16_t rawAccel = 0;
	if(m_i2c)
	{
		m_i2c->Read(kDataRegister + (uint8_t)axis, sizeof(rawAccel), (uint8_t *)&rawAccel);

		// Sensor is little endian... swap bytes
		rawAccel = ((rawAccel >> 8) & 0xFF) | (rawAccel << 8);
	}
	return rawAccel * kGsPerLSB;
}

/**
 * Get the acceleration of all axes in Gs.
 *
 * @return Acceleration measured on all axes of the ADXL345 in Gs.
 */
ADXL345_I2C_ALT::AllAxes ADXL345_I2C_ALT::GetAccelerations()
{
	AllAxes data = {0.0};
	int16_t rawData[3];
	if (m_i2c)
	{
		m_i2c->Read(kDataRegister, sizeof(rawData), (uint8_t*)rawData);

		// Sensor is little endian... swap bytes
		for (int32_t i=0; i<3; i++)
		{
			rawData[i] = ((rawData[i] >> 8) & 0xFF) | (rawData[i] << 8);
		}

		data.XAxis = rawData[0] * kGsPerLSB;
		data.YAxis = rawData[1] * kGsPerLSB;
		data.ZAxis = rawData[2] * kGsPerLSB;
	}
	return data;
}
