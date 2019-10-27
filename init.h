#ifndef INIT_H
#define INIT_H


#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_i2c.h"
#include "misc.h"

#include <string.h>
#include <stdlib.h>


//-------------------------------------------//
//			��������� ������ ESP			 //
//-------------------------------------------//

//��������� � ���������
#define RX_BUF_SIZE		128// ������ ������������� ������ ������� ����� �������
#define BUF_SIZE 		9//������ ������ � ���������


//������ �� TCP �� ESP
uint8_t RXi; // ������� ������� RX_BUF
uint8_t RXc; // ���������� ��� �������� ���������� �����
uint8_t RX_BUF[RX_BUF_SIZE]; //�������� �����
uint8_t TX_BUF[BUF_SIZE];//�������� ������� � �����������

uint8_t init; 	// ��������� ����������� ������� � ������
				// 1 - ��� ������������ (�������������������� ������ � ������� �������)
				// 0 - ������ ��������� (���������� �������������� � ���)
uint8_t id;		// ����� ������ � �������� ����������� ������ (�������� ����� �������)
uint8_t id_rx;	// ���������� �������� ����
uint8_t FLAG_REPLY;	// ���� ��� ������ �������� ������ AT



//-------------------------------------------//
//			�������� ��������������			 //
//-------------------------------------------//

//Comands
#define SWITCH_LIGHT		0x00
#define GET_TIME 			0x01
#define SET_TIME 			0x02
#define GET_SET_ALARM 		0x03
#define GET_SENSORS			0x04


/*
 * SubComands for Sensors
 */
#define DELAY_SENSORS		40
#define GET_TEMP			0x01
#define GET_CARB			0x02







typedef struct{
	uint8_t null;
}t_swtch_light;

typedef struct{
	uint8_t h_m_s[3];
}t_time;


typedef struct{
	uint8_t time[3];
	uint8_t status;
	uint8_t settings;
}t_alarm;

typedef struct{
	uint8_t temp;
	uint16_t co2;

}t_sensors;

typedef struct{
	uint8_t cmd_type;

	union{
		t_swtch_light swtch_light;
		t_time time;
		t_alarm alarm;
		t_sensors sensors;
	}arg;

}t_cmd;







//-------------------------------------------//
//		��������� ��� ������� � ���������	 //
//-------------------------------------------//


//��������� MH-Z19
uint8_t RXi_MH; // ������� ��������� ������� �� MH-Z19




//��������� ������
#define DELAY_POUSE    242					//������������ �������� ����, ���
#define DELAY_BIT      3*DELAY_POUSE		//������������ ���� ��������� ����, ���

#define DELAY_MESSAGE  28*DELAY_POUSE		//�������� ����� �����������, ��

#define MES   0b0000000111110110011100000	//����������� ����� ��� ������

//���������������� ��� ���������� ������� A3 (������ �� �����������)
#define A3_ENABLE         GPIOA->BSRR = GPIO_BSRR_BS3;
#define A3_DISABLE    	  GPIOA->BSRR = GPIO_BSRR_BR3;






//��������� DS3231
#define DS_ADDRESS            		0xD0
#define DS_ADDRESS_WRITE            (DS_ADDRESS|0)
#define DS_ADDRESS_READ             (DS_ADDRESS|1)

#define DS3231_CONTROL				0x0E	// ����� �������� ����������
#define DS3231_A1IE					0x00
#define DS3231_A2IE					0x01
#define DS3231_INTCN				0x02
#define DS3231_CONV					0x05
#define DS3231_BBSQW				0x06

#define DS3231_STATUS				0x0F	// ����� �������� ���������
#define DS3231_A1F					0x00
#define DS3231_A2F					0x01
#define DS3231_BSY					0x02


#define DS3231_DY					0x0A	// ����� �������� �������� ������������
#define DS3231_DY_PERSEC			0b1111
#define DS3231_DY_SEC				0b1110
#define DS3231_DY_MINSEC			0b1100
#define DS3231_DY_HMINSEC			0b1000


#define DS3231_T_MSB				0x11	// ����� �������� ��������� ���������� ����������� �������� ����
#define DS3231_T_LSB				0x12	// ����� �������� ��������� ���������� ����������� �������� ����





//------------------------------------------//
//				��������� ������		 	//
//------------------------------------------//

//i2c
uint8_t error_i2c;





//------------------------------------------//
//				������ �������������	 	//
//------------------------------------------//
char count[3]; // ���������� ������������ ��������

uint8_t way_cmd;
enum do_for_esp{

WAIT,
ID,
RX_MODE,
DECODE,
INVITATION,
INIT_SENDMES,
SENDMES,
INIT_ESP,
END
};


uint8_t way_closed;
enum do_closed{
C,
L,
O,
S,
E,
D
};



//������ � �������
void SetSysClockTo72(void);

void ports_init(void);
void usartESP_init(void);
void timer_init(void);
void I2C1_init(void);
void usartCN_init(void);

#endif
