#include <unistd.h>		   //Needed for I2C port
#include <fcntl.h>		   //Needed for I2C port
#include <sys/ioctl.h>	   //Needed for I2C port
#include <linux/i2c-dev.h> //Needed for I2C port
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	int i2cF;
	int length;
	unsigned char buffer[60] = {0};

	//----- OPEN THE I2C BUS -----
	char *filename = (char *)"/dev/i2c-1";
	if ((i2cF = open(filename, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		printf("Failed to open the i2c bus");
		return;
	}

	int addr = 0x48; //<<<<<The I2C address of the slave
	if (ioctl(i2cF, I2C_SLAVE, addr) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return;
	}

	//----- READ BYTES -----
	length = 4;									  //<<< Number of bytes to read
	if (read(i2cF, buffer, length) != length) //read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		//ERROR HANDLING: i2c transaction failed
		printf("Failed to read from the i2c bus.\n");
	}
	else
	{
		
			
			printf("Data read: %x\n", buffer);
		
		
	}
}