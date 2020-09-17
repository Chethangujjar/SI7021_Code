#include "board_config.h"
#include "Si7021.h"

unsigned long Si7021_read(unsigned long reg);
void Si7021_write(unsigned long reg, unsigned long data);

unsigned long Si7021_read_Temp(void)
{
 unsigned long MSB=0, LSB=0, data = 0;
 unsigned long timeout=0;

 //Initiate a write to I2C bus
 
	I2CMasterSlaveAddrSet(Si7021_I2C_MASTER_BASE, SLAVE_ADDRESS, false);

	I2CMasterDataPut(Si7021_I2C_MASTER_BASE, 0xE0);
	
	I2CMasterControl(Si7021_I2C_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);

	
 while(!(I2CMasterBusy(Si7021_I2C_MASTER_BASE)))
 {
	 
 }	

 while(I2CMasterBusy(Si7021_I2C_MASTER_BASE))
 {
  
 } 
 
 I2CMasterSlaveAddrSet(Si7021_I2C_MASTER_BASE, SLAVE_ADDRESS, true);
 
 I2CMasterControl(Si7021_I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
 
 while(!(I2CMasterBusy(Si7021_I2C_MASTER_BASE)))
 {

 } 
 
 while(I2CMasterBusy(Si7021_I2C_MASTER_BASE))
 {

 }

 MSB = I2CMasterDataGet(Si7021_I2C_MASTER_BASE);

 I2CMasterControl(Si7021_I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH); 
 
 while(!(I2CMasterBusy(Si7021_I2C_MASTER_BASE)))
 {

 } 

 while(I2CMasterBusy(Si7021_I2C_MASTER_BASE))
 {

 }

 LSB = I2CMasterDataGet(Si7021_I2C_MASTER_BASE);

 data = (MSB << 8) | LSB ;
 
 return data;
}

unsigned long Si7021_read_Humid(void)
{
 unsigned long MSB=0, LSB=0, data = 0;
 unsigned long timeout=0;

 //Initiate a write to I2C bus
 I2CMasterSlaveAddrSet(Si7021_I2C_MASTER_BASE, SLAVE_ADDRESS, false);	
	
 I2CMasterDataPut(Si7021_I2C_MASTER_BASE, 0xE5);
	
 I2CMasterControl(Si7021_I2C_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);	
	
 while(!(I2CMasterBusy(Si7021_I2C_MASTER_BASE)))
 {  
 }	

 while(I2CMasterBusy(Si7021_I2C_MASTER_BASE))
 {
 }
 
 I2CMasterSlaveAddrSet(Si7021_I2C_MASTER_BASE, SLAVE_ADDRESS, true);
 
 I2CMasterControl(Si7021_I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

 while(!(I2CMasterBusy(Si7021_I2C_MASTER_BASE)))
 {
  
 } 
 
 while(I2CMasterBusy(Si7021_I2C_MASTER_BASE))
 {

 }
 
 MSB = I2CMasterDataGet(Si7021_I2C_MASTER_BASE);
 
 I2CMasterControl(Si7021_I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
 
 while(!(I2CMasterBusy(Si7021_I2C_MASTER_BASE)))
 { 
 } 
 
 while(I2CMasterBusy(Si7021_I2C_MASTER_BASE))
 {
 }
 
 LSB = I2CMasterDataGet(Si7021_I2C_MASTER_BASE);

 data = (MSB << 8) | LSB ;
 
 return data;
}

/**
 *  @brief Returns temperature in degree Celsius read from Si7005 sensor
 *
 *  @param void.
 *  @return unsigned long temperature in degree Celsius
 */
unsigned long get_temperature(void)
{
	unsigned long temperature;

	temperature = ((175.72 * Si7021_read_Temp()) / 65536) - 46.85;
	
	UARTprintf("Temperature : %d\r\n", temperature);

	return temperature;
}

/**
 *  @brief Returns % relative humidity read from Si7005 sensor
 *
 *  @param void.
 *  @return unsigned long %RH
 */
unsigned long get_humidity(void)
{
	unsigned long humidity;

	humidity = (125 * Si7021_read_Humid() / 65536) - 6;
	
	UARTprintf("Humidity : %d\r\n", humidity);

	return humidity;
}

/**
 *  @brief  Configures I2C MasterBlock to communicate with Si7021
 *
 *	I2C MasterBlock is set up with
 *	peripheral clock equal to the processor clock
 *	data transfer speed 100 Kbps.
 *
 *  @param  void
 *  @return void
 */
void Si7021_init(void)
{
	//Enable GPIO Peripheral to which Si7021 is connected
	SysCtlPeripheralEnable(Si7021_PERIPH_GPIO);
	//Enable I2C Peripheral to which Si7005 is connected
	SysCtlPeripheralEnable(Si7021_PERIPH_I2C);

	//configure the pin mux that selects the I2CSCL function associated with the GPIO pin
	GPIOPinConfigure(Si7021_I2CSCL);
	//configure the pin mux that selects the I2CSDA function associated with the GPIO pin
	GPIOPinConfigure(Si7021_I2CSDA);

	//Configure pin for use as SCL by the I2C peripheral
	GPIOPinTypeI2CSCL(Si7021_GPIO_BASE, Si7021_GPIO_PIN_I2CSCL);
	//Configure SDA pin for use by the I2C peripheral.
	GPIOPinTypeI2C(Si7021_GPIO_BASE, Si7021_GPIO_PIN_I2CSDA);

	/*
	 * Set Up the I2C Master block to transfer data at 100 Kbps.
	 * Change third argument of I2CMasterInitExpClk to true to transfer data at 400 Kbps
	 */
	I2CMasterInitExpClk(Si7021_I2C_MASTER_BASE, SysCtlClockGet(), true);

	//Provide a small delay for initialisation of the I2C Peripheral
	App_delay(1000);
	
	UARTprintf("Si7021 Heater Enabled\r\n");
	Si7021_write(0xE6, 0x3E);
	UARTprintf("Si7021 Write\r\n");
}

/**
 *  @brief  Reads a register of Si7021
 *
 *  @param  reg  internal address of the register
 *  @return unsigned long  data read from the register
 *
 */
unsigned long Si7021_read(unsigned long reg)
{
	unsigned long data;
	unsigned long timeout;

	//Initiate a write to I2C bus
	I2CMasterSlaveAddrSet(Si7021_I2C_MASTER_BASE, SLAVE_ADDRESS, false);
	//Place the address of the register into I2C Master Data Register
	I2CMasterDataPut(Si7021_I2C_MASTER_BASE, reg);
	//Send the address of the register from which data has to be read
	I2CMasterControl(Si7021_I2C_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);

	//Wait till I2CMaster completes transmitting.
	while(!(I2CMasterBusy(Si7021_I2C_MASTER_BASE)))
	{

	}	

	//Wait till I2CMaster completes transmitting.
	while(I2CMasterBusy(Si7021_I2C_MASTER_BASE))
	{

	}

	//Initiate a read from the I2C bus
	I2CMasterSlaveAddrSet(Si7021_I2C_MASTER_BASE, SLAVE_ADDRESS, true);
	//Initiate receiving of data to the I2C Master Data Register from Si7021.
	I2CMasterControl(Si7021_I2C_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

	while(!(I2CMasterBusy(Si7021_I2C_MASTER_BASE)))
	{

	}	
	
	//Wait till I2CMaster completes transmitting.
	while(I2CMasterBusy(Si7021_I2C_MASTER_BASE))
	{
	}

	//Read a byte of data,cast as an unsigned long from the I2C Master Data Register.
	data = I2CMasterDataGet(Si7021_I2C_MASTER_BASE);

	//Return the data read from the register
	return data;

}


void Read_Configuration(void)
{
	UARTprintf( " Config : %x\r\n",Si7021_read(0xE7));
	
}

/**
 *  @brief  Writes data to a register of Si7021
 *
 *  @param  reg  internal address of the register
 *  @param  data value to be written to the register
 *  @return void
 *
 */
void Si7021_write(unsigned long reg, unsigned long data)
{

	unsigned long timeout;
	//Initiate a write to I2C bus
	I2CMasterSlaveAddrSet(Si7021_I2C_MASTER_BASE, SLAVE_ADDRESS, false);
	//Place the register value into I2C Master Data Register
	I2CMasterDataPut(Si7021_I2C_MASTER_BASE, reg);
	//Send the address of the register to which data has to be written
	I2CMasterControl(Si7021_I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	//Wait till I2CMaster completes transmitting.
	while(!(I2CMasterBusy(Si7021_I2C_MASTER_BASE)))
	{

	}	
	
	//Wait till I2CMaster completes transmitting.
	while(I2CMasterBusy(Si7021_I2C_MASTER_BASE))
	{
		
	}
	
	//Place the data to be written into I2C Master Data Register
	I2CMasterDataPut(Si7021_I2C_MASTER_BASE, data);
	//Send the data so that it is written to the register.
	I2CMasterControl(Si7021_I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

	//Wait till I2CMaster completes transmitting.
	while(!(I2CMasterBusy(Si7021_I2C_MASTER_BASE)))
	{
	}	
	
	//Wait till I2CMaster completes transmitting.
	while(I2CMasterBusy(Si7021_I2C_MASTER_BASE))
	{

	}
	
	I2CMasterControl(Si7021_I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_STOP);
	
}
