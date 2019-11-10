#ifndef INIT_H
#define INIT_H


#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_i2c.h"
#include "misc.h"
#include "flash.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


//-------------------------------------------//
//			Настройки модуля ESP			 //
//-------------------------------------------//

//Настройки и константы
#define RX_BUF_SIZE		128// Размер максимального пакета который можно принять
#define BUF_SIZE 		9//Размер пакета с командами


//Данные по TCP от ESP
uint8_t RXi; // Счетчик массива RX_BUF
uint8_t RXc; // Переменная для хранения пришедшего байта
uint8_t RX_BUF[RX_BUF_SIZE]; //Принятый масив

uint8_t init; 	// Состояние подключения клиента к модулю
				// 1 - Нет подключенных (Переинициализируется модуль и ожидает клиента)
				// 0 - Клиент подключен (Начинается взаимодействие с ним)
uint8_t id;		// Номер сокета к которому подключился клиент (сквозной номер клиента)
uint8_t id_rx;	// Количество принятых байт
uint8_t FLAG_REPLY;	// Флаг для приема ответных команд AT



//-------------------------------------------//
//			Протокол взаимодействия			 //
//-------------------------------------------//

//Comands


enum list_cmd{

SWITCH_LIGHT,
GET_TIME,
SET_TIME,
GET_ALARM,
SET_ALARM,
GET_SENSORS,

ZERO = 0xFF
};



/* Compatibility */
#ifndef PACKSTRUCT
/*Default packed configuration*/
#ifdef __GNUC__
#ifdef _WIN32
#define PACKSTRUCT( decl ) decl __attribute__((__packed__,gcc_struct))
#else
#define PACKSTRUCT( decl ) decl __attribute__((__packed__))
#endif
#define ALIGNED __attribute__((aligned(0x4)))
#elif __IAR_SYSTEMS_ICC__

#define PACKSTRUCT( decl ) __packed decl

#define ALIGNED
#elif _MSC_VER  /*msvc*/

#define PACKSTRUCT( decl ) __pragma( pack(push, 1) ) decl __pragma( pack(pop) )
#define ALIGNED
#else
#define PACKSTRUCT(a) a PACKED
#endif
#endif


PACKSTRUCT(struct t_time{
	uint8_t h_m_s[3];
});

PACKSTRUCT(struct t_alarm{
	uint8_t h_m_s[3];
	uint8_t status;
	uint8_t settings;
});



PACKSTRUCT(struct t_sensors{
	uint8_t temp;
	uint8_t co[9];
});

PACKSTRUCT( struct t_cmd{
	uint8_t cmd_type;

	union{
		struct t_time time;
		struct t_alarm alarm;
		struct t_sensors sensors;
	}arg;

});


struct t_time time;
struct t_alarm alarm;
struct t_sensors sensors;

struct t_cmd cmd;
uint8_t size_cmd;
uint8_t command;




//-------------------------------------------//
//		Настройки для модулей и периферии	 //
//-------------------------------------------//


//настройки MH-Z19
uint8_t RXi_MH; // Счетчик принятого массива от MH-Z19
uint8_t co_h;
uint8_t co_l;
uint8_t temp;



//настройки люстры
#define DELAY_POUSE    242					//Длительность четверти бита, мкс
#define DELAY_BIT      3*DELAY_POUSE		//Длительность трех четвертей бита, мкс

#define DELAY_MESSAGE  28*DELAY_POUSE		//Задержка между сообщениями, мс

#define MES   0b0000000111110110011100000	//Управляющее слово для люстры

//макроопределения для управления выводом A3 (сигнал на радиомодуль)
#define A3_ENABLE         GPIOA->BSRR = GPIO_BSRR_BS3;
#define A3_DISABLE    	  GPIOA->BSRR = GPIO_BSRR_BR3;






//Настройки DS3231
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




//------------------------------------------//
//				Обработка ошибок		 	//
//------------------------------------------//

//i2c
uint8_t error_i2c;





//------------------------------------------//
//				Логика взамодействия	 	//
//------------------------------------------//
char count[3]; // количество отправляемых символов

uint8_t way_get_mes;
enum get_mes{

WAIT,
ID,
RX_MODE
};

uint8_t way_prep_mes;
enum prep_mes{

UPDATA,
DECODE,
INVITATION,
INIT_SENDMES,
SENDMES,
INIT_ESP,
GET_SENSOR,
WAIT_EQV
};


#define TIM3_PERIOD 50000 //50000/10000 = 5 секунд


//Методы и функции
void SetSysClockTo72(void);

void ports_init(void);
void usartESP_init(void);
void timer_init(void);
void I2C1_init(void);
void usartCN_init(void);

#endif
