void i2c_setup(void);

int i2c_write(uint8_t deviceAddress, const uint8_t *data, size_t size, bool stop);

int i2c_read(uint8_t deviceAddress, uint8_t registerAddress, size_t size, uint8_t *data);
