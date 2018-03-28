/*
 * EEPROM.c
 *
 *  Created on: Mar 26, 2018
 *      Author: eric_
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "fsl_port.h"
#include "fsl_i2c.h"
#include "FREErtos.h"
#include "event_groups.h"
#include "EEPROM.h"

#define EEPROMADDRESS (0x50)

volatile bool g_MasterCompletionFlagprom = false;

static void i2c_master_callback(I2C_Type *base, i2c_master_handle_t *handle,
        status_t status, void * userData)
{

	if (status == kStatus_Success)
	{
		g_MasterCompletionFlagprom = true;
		PRINTF("\rSUCCESS\n");
	}
}

void writeMemory(uint16_t add,uint8_t data)
{
	i2c_master_handle_t g_m_handle;
	i2c_master_config_t masterConfig;
	i2c_master_transfer_t masterXfer;
	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = 100000;

	I2C_MasterInit(I2C0, &masterConfig, CLOCK_GetFreq(kCLOCK_BusClk));
	I2C_MasterTransferCreateHandle(I2C0, &g_m_handle,i2c_master_callback, NULL);

	uint8_t datah = data;

	masterXfer.slaveAddress = EEPROMADDRESS;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = add;
	masterXfer.subaddressSize = 2;
	masterXfer.data = &datah;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;


	I2C_MasterTransferNonBlocking(I2C0, &g_m_handle,&masterXfer);

	while (!g_MasterCompletionFlagprom)
	{
	}
	g_MasterCompletionFlagprom = false;
}


uint8_t readMemory(uint16_t add)
{
	i2c_master_handle_t g_m_handle;
	i2c_master_config_t masterConfig;
	i2c_master_transfer_t masterXfer;
	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = 100000;

	I2C_MasterInit(I2C0, &masterConfig, CLOCK_GetFreq(kCLOCK_BusClk));
	I2C_MasterTransferCreateHandle(I2C0, &g_m_handle,i2c_master_callback, NULL);

	uint16_t data_buffer = add;

	masterXfer.slaveAddress = EEPROMADDRESS;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data = &data_buffer;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferNoStopFlag;

	I2C_MasterTransferNonBlocking(I2C0,  &g_m_handle,&masterXfer);


	uint8_t read_data;

	masterXfer.slaveAddress = EEPROMADDRESS;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = 0;
	masterXfer.subaddressSize = 0;
	masterXfer.data = &read_data;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferRepeatedStartFlag;

	I2C_MasterTransferNonBlocking(I2C0, &g_m_handle,&masterXfer);

	while (!g_MasterCompletionFlagprom){}
	g_MasterCompletionFlagprom = false;

	for(int i=100000000;i==0;i--){}

	return read_data;
}
