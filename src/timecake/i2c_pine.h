void i2c_setup(void);

//extern void i2c_write(int device,int length,uint8_t *data);

//int i2c_read(int device,int address,int length,uint8_t *data);
int i2c_read(uint8_t deviceAddress, uint8_t registerAddress, size_t size, uint8_t *data);
