/*
 * i2c_test.h
 *
 *  Created on: Jul 23, 2024
 *      Author: michaeljacobson
 */

#ifndef INCLUDE_I2C_TEST_H_
#define INCLUDE_I2C_TEST_H_

#define ALL_I2C_BUSES	99


void init_i2c(char which_i2c);
void close_i2c(char which_i2c);
int read_i2c(char which_i2c, char device_addr, unsigned char reg_addr, unsigned char *p_data, int len);
int write_i2c(char which_i2c, char device_addr, unsigned char reg_addr, unsigned char *p_data, int len);
void Test_DigitPOT(void);
#endif /* INCLUDE_I2C_TEST_H_ */
