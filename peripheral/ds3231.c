#include "ds3231.h"

uint8_t DS3231_init(void){
	error_i2c = 0;
	uint8_t temp = 1;
	temp = I2C_single_read(DS_ADDRESS, DS3231_CONTROL);
	temp |= (1 << DS3231_INTCN);
	I2C_single_write(DS_ADDRESS, DS3231_CONTROL, temp);

	I2C_single_write(DS_ADDRESS, DS3231_DY, 0b10000000);
	return 1;
}

uint8_t DS3231_read_temp(void){
	uint8_t temp = 1;

	// �������� ������ BSY
	while(temp)
	{
		temp = I2C_single_read(DS_ADDRESS, DS3231_STATUS);
		temp &= (1 << DS3231_BSY);
	}

	// ������ �������� CONTROL � ��������� ���� CONV
	temp = I2C_single_read(DS_ADDRESS, DS3231_CONTROL);
	temp |= (1 << DS3231_CONV);
	I2C_single_write(DS_ADDRESS, DS3231_CONTROL, temp);

	// ������ �����������
	return I2C_single_read(DS_ADDRESS, DS3231_T_MSB);
}



void ds3231_del_alarm(void){
	uint8_t temp = 1;
	temp = I2C_single_read(DS_ADDRESS, DS3231_STATUS);
	temp &= ~(1 << DS3231_A1F);
	I2C_single_write(DS_ADDRESS, DS3231_STATUS, temp);
}

void ds3231_on_alarm(uint8_t stat){

	ds3231_del_alarm();
	uint8_t temp = 1;
	temp = I2C_single_read(DS_ADDRESS, DS3231_CONTROL);
	if(stat){
		temp |= (1 << DS3231_A1IE);// �������� ���������
	}
	else{
		temp &= ~(1 << DS3231_A1IE);
	}

	I2C_single_write(DS_ADDRESS, DS3231_CONTROL, temp);


}
