/*
 * LCDNokia5110.c
 *
 *  Created on: Jun 13, 2014
 *      Author: Luis
 */

#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "fsl_dspi.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "LCDNokia5110.h"

#define TEST	(0)
#define BIT_DATA	(1<<3)
#define BIT_RESET	(1<<0)

static const uint8_t ASCII[][5] =
{
 {0x00, 0x00, 0x00, 0x00, 0x00} // 20
,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c ¥
,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e
,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f
};

dspi_master_handle_t g_dspiHandle;
dspi_transfer_t g_receiveXDspi;
volatile bool masterFinished;

static void dspi_master_transfer_callback(SPI_Type *base,
		dspi_master_handle_t *handle, status_t status, void *userData)
{
	if (kStatus_DSPI_Idle == status)
	{
		masterFinished = true;
	}
}

status_t init_SPI0(void)
{
	CLOCK_EnableClock(kCLOCK_PortD);
	CLOCK_EnableClock(kCLOCK_Spi0);

	port_pin_config_t pinConfig =
	{
			kPORT_PullDisable, kPORT_SlowSlewRate, kPORT_PassiveFilterDisable,
			kPORT_OpenDrainDisable, kPORT_LowDriveStrength, kPORT_MuxAlt2,
			kPORT_UnlockRegister
	};

	gpio_pin_config_t pinControlRegister =
	{kGPIO_DigitalOutput, 1};

	GPIO_PinInit(GPIOD, 1, &pinControlRegister);
	GPIO_PinInit(GPIOD, 2, &pinControlRegister);

	PORT_SetPinConfig(PORTD, 1, &pinConfig);
	PORT_SetPinConfig(PORTD, 2, &pinConfig);

    dspi_master_config_t masterConfig;

    DSPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;

    DSPI_MasterInit(SPI0, &masterConfig, CLOCK_GetFreq(kCLOCK_BusClk));
    DSPI_MasterTransferCreateHandle(SPI0, &g_dspiHandle,
    		dspi_master_transfer_callback, NULL);

    LCDNokia_init();
    LCDNokia_clear();

	return (kStatus_Success);
}


void LCDNokia_init(void) {

	//CLOCK_EnableClock(kCLOCK_PortD);
	//CLOCK_EnableClock(kCLOCK_Spi0);

	port_pin_config_t pinConfig =
	{
			kPORT_PullDisable, kPORT_SlowSlewRate, kPORT_PassiveFilterDisable,
			kPORT_OpenDrainDisable, kPORT_LowDriveStrength, kPORT_MuxAsGpio,
			kPORT_UnlockRegister
	};

	gpio_pin_config_t pinControlRegister =
	{kGPIO_DigitalOutput, 1};

	GPIO_PinInit(GPIOD, DATA_OR_CMD_PIN, &pinControlRegister);
	GPIO_PinInit(GPIOD, RESET_PIN, &pinControlRegister);

	PORT_SetPinConfig(PORTD, DATA_OR_CMD_PIN, &pinConfig);
	PORT_SetPinConfig(PORTD, RESET_PIN, &pinConfig);

	GPIO_PortClear(GPIOD, BIT_RESET);
	LCD_delay();
	GPIO_SetPinsOutput(GPIOD, BIT_RESET);
	LCDNokia_writeByte(LCD_CMD, 0x21); //Tell LCD that extended commands follow
	LCDNokia_writeByte(LCD_CMD, 0xBF); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
	LCDNokia_writeByte(LCD_CMD, 0x04); //Set Temp coefficent
	LCDNokia_writeByte(LCD_CMD, 0x14); //LCD bias mode 1:48: Try 0x13 or 0x14

	LCDNokia_writeByte(LCD_CMD, 0x20); //We must send 0x20 before modifying the display control mode
	LCDNokia_writeByte(LCD_CMD, 0x0C); //Set display control, normal mode. 0x0D for inverse
}

void LCDNokia_bitmap(const uint8_t* my_array){

	uint16_t index = 0;

	for (index = 0 ; index < (LCD_X * LCD_Y / 8) ; index++)
	{
		LCDNokia_writeByte(LCD_DATA, *(my_array + index));

	}
}

void LCDNokia_writeByte(uint8_t DataOrCmd, uint8_t data)
{
	g_receiveXDspi.txData = &data;
	g_receiveXDspi.dataSize = sizeof(data);
	g_receiveXDspi.configFlags = kDSPI_MasterCtar0|kDSPI_MasterPcs0;

	if(DataOrCmd)
	{
		GPIO_SetPinsOutput(GPIOD, BIT_DATA);
	}
	else
	{
		GPIO_PortClear(GPIOD, BIT_DATA);
	}

	DSPI_MasterTransferNonBlocking(SPI0, &g_dspiHandle,
			&g_receiveXDspi);
	while(!masterFinished)
	{
	}
}

void LCDNokia_sendChar(uint8_t character)
{
  uint16_t index = 0;

  LCDNokia_writeByte(LCD_DATA, 0x00); //Blank vertical line padding

  for (index = 0 ; index < 5 ; index++)
	  LCDNokia_writeByte(LCD_DATA, ASCII[character - 0x20][index]);
    //0x20 is the ASCII character for Space (' '). The font table starts with this character

  LCDNokia_writeByte(LCD_DATA, 0x00); //Blank vertical line padding
}

void LCDNokia_sendString(uint8_t *characters)
{
  while (*characters)
  {
	  LCDNokia_sendChar(*characters++);
  }
}

void LCDNokia_clear(void)
{
	uint16_t index = 0;

	for (index = 0 ; index < (LCD_X * LCD_Y / 8) ; index++)
	{
		LCDNokia_writeByte(LCD_DATA, 0x00);
	}
	LCDNokia_gotoXY(0, 0); //After we clear the display, return to the home position
}

void LCDNokia_gotoXY(uint8_t x, uint8_t y)
{
	LCDNokia_writeByte(LCD_CMD, 0x80 | x);  // Column.
	LCDNokia_writeByte(LCD_CMD, 0x40 | y);  // Row.  ?
}

void LCD_delay(void)
{
	int counter;

	for(counter=0; counter<1500; counter++)
	{

	}
}

void LCDNokia_printValue(uint32_t value){

	/**Data Type that saves the real value**/
	formatASCII real_Value;
	/**Temporal digit that is an acumulator**/
	uint8_t temp_digit;
	/**Counter of digits**/
	uint8_t counter;
	/**Value of Zero in ASCII**/
	const uint8_t zero = 48;
	/**Init the number of digits at 0**/
	real_Value.numberDigits = 0;

	/**While the value != 0**/
	if(value != 0){
		while(value > 0){
			/**Save the modulo**/
			temp_digit = value % 10;
			/**The modulo is converted to ASCII**/
			temp_digit = '0' + temp_digit;
			/**This value is save in the real value**/
			real_Value.realDigit[real_Value.numberDigits] = temp_digit;
			/**Continue with the next value**/
			value /= 10;
			/**Real value go on with the next position**/
			real_Value.numberDigits++;
		}

		/**Print the value saved in real value**/
		for(counter = real_Value.numberDigits; counter != 0 ; counter--){
			LCDNokia_sendChar(real_Value.realDigit[counter-1]);
		}
	}else{
		LCDNokia_sendChar(zero);
	}

}

void LCDNokia_printFloatValue(float value){

	/**Values in ASCII**/
	const uint8_t wordPoint = 46;
	const uint8_t numZero = 48;
	/**Save the temporal Float**/
	float tmp_Float;
	/**Saves the two parts of a real number**/
	uint8_t part_Float;
	uint32_t part_Int;

	/**Save the float part**/
	tmp_Float = value - (uint8_t)value;
	/**Converts the float part into integer**/
	tmp_Float *= 100;
	/**The float part is saved **/
	part_Float = (uint8_t)tmp_Float;
	/**The int part is saved**/
	part_Int = (uint8_t)value;

	/**Print the value smaller than 10**/
	if(part_Float < 10){

		LCDNokia_printValue(part_Int);
		LCDNokia_sendChar(wordPoint);

		LCDNokia_sendChar(numZero);
		LCDNokia_printValue(part_Float);
	}
	/**Print the value greater than 10**/
	if(part_Float >= 10){

		LCDNokia_printValue(part_Int);
		LCDNokia_sendChar(wordPoint);
		LCDNokia_printValue(part_Float);
	}

}
