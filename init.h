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





uint8_t error_i2c;
/*
 * DS3231
 */

#define DS_ADDRESS            		0xD0
#define DS_ADDRESS_WRITE            (DS_ADDRESS|0)
#define DS_ADDRESS_READ             (DS_ADDRESS|1)

#define DS3231_CONTROL				0x0E	// Адрес регистра управления
#define DS3231_A1IE					0x00
#define DS3231_A2IE					0x01
#define DS3231_INTCN				0x02
#define DS3231_CONV					0x05
#define DS3231_BBSQW				0x06

#define DS3231_STATUS				0x0F	// Адрес регистра состояния
#define DS3231_A1F					0x00
#define DS3231_A2F					0x01
#define DS3231_BSY					0x02


#define DS3231_DY					0x0A	// Адрес регистра критерия срабатывания
#define DS3231_DY_PERSEC			0b1111
#define DS3231_DY_SEC				0b1110
#define DS3231_DY_MINSEC			0b1100
#define DS3231_DY_HMINSEC			0b1000


#define DS3231_T_MSB				0x11	// Адрес регистра последней измеренной температуры старшиий байт
#define DS3231_T_LSB				0x12	// Адрес регистра последней измеренной температуры младшиий байт


















//Настройки и контанты
#define RX_BUF_SIZE		128// Размер максимального пакета который можно принять
#define BUF_SIZE 		9//Размер пакета с командами


 /*
  * Comands for I2C
  */
#define SWITCH_LIGHT		0x00
#define GET_TIME 			0x01
#define SET_TIME 			0x02
#define GET_SET_ALARM 		0x03
#define GET_SENSORS			0x04

//-------------------------------------------//
//		Настройки для управления люстрой	 //
//-------------------------------------------//


#define DELAY_POUSE    242					//Длительность четверти бита, мкс
#define DELAY_BIT      3*DELAY_POUSE		//Длительность трех четвертей бита, мкс

#define DELAY_MESSAGE  28*DELAY_POUSE		//Задержка между сообщениями, мс

#define MES   0b0000000111110110011100000	//Управляющее слово для люстры

//макроопределения для управления выводом A3 (сигнал на радиомодуль)
#define A3_ENABLE         GPIOA->BSRR = GPIO_BSRR_BS3;
#define A3_DISABLE    	  GPIOA->BSRR = GPIO_BSRR_BR3;




//Данные по TCP
uint8_t RXi; // Счетчик массива RX_BUF
uint8_t RXc; // Переменная для хранения пришедшего байта
char RX_BUF[RX_BUF_SIZE]; //Принятый масив
uint8_t TX_BUF[BUF_SIZE];//Принятые команды с параметрами

uint8_t init; 	// Состояние подключения клиента к модулю
				// 1 - Нет подключенных (Переинициализируется модуль и ожидает клиента)
				// 0 - Клиент подключен (Начинается взаимодействие с ним)
uint8_t id;		// Номер сокета к которому подключился клиент (сквозной номер клиента)
uint8_t id_rx;	// Количество принятых байт
uint8_t FLAG_REPLY;	// Флаг для ожидания нужного сообщения от модуля
uint8_t way_cmd;

char count[3]; // количество отправляемых символов

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



//Методы и функции
void SetSysClockTo72(void);

void ports_init(void);
void usartESP_init(void);
void timer_init(void);
void I2C1_init(void);

#endif
