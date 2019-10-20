#include "i2c.h"

uint8_t cicle(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT){//��������� ������� �� ���� I2C
	/*
	 * ������� ��������� ������� � �� ��������� ����������� ������� ������
	 * ���� ������� ���������, �� ��������� ������� �� ���� �������
	 */

	for(uint8_t i = 255; i; i--){
		if(I2C_CheckEvent(I2Cx, I2C_EVENT))
			return 0;
	}
	error_i2c = I2C_EVENT & 0xFF;
	I2C_GenerateSTOP(I2C1, ENABLE);
	return 1;
}


uint8_t I2C_single_write(uint8_t HW_address, uint8_t addr, uint8_t data)
{
	I2C_GenerateSTART(I2C1, ENABLE);
	if(cicle(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
		return 1;//������ ��� ������ �� �������
	I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Transmitter);
	if(cicle(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		return 1;//������ ��� ������ �� �������
	I2C_SendData(I2C1, addr);
	if(cicle(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		return 1;//������ ��� ������ �� �������
	I2C_SendData(I2C1, data);
	if(cicle(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		return 1;//������ ��� ������ �� �������
	I2C_GenerateSTOP(I2C1, ENABLE);
	return 1;//������ ��� ������ �� �������
}


/*******************************************************************/
uint8_t I2C_single_read(uint8_t HW_address, uint8_t addr)
{
	uint8_t data;

	I2C_GenerateSTART(I2C1, ENABLE);
	if(cicle(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
		return 1;//������ ��� ������ �� �������
	I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Transmitter);
	if(cicle(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		return 1;//������ ��� ������ �� �������
	I2C_SendData(I2C1, addr);
	if(cicle(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		return 1;//������ ��� ������ �� �������
	I2C_GenerateSTART(I2C1, ENABLE);
	if(cicle(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
		return 1;//������ ��� ������ �� �������
	I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Receiver);
	if(cicle(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED))
		return 1;//������ ��� ������ �� �������
	data = I2C_ReceiveData(I2C1);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	I2C_GenerateSTOP(I2C1, ENABLE);
	return data;
}
