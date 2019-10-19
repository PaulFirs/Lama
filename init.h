#ifndef INIT_H
#define INIT_H


#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"

#include <string.h>
#include <stdlib.h>



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


//
//
//const char *AT_OK			= "OK";
//const char *AT_AT			= "AT\r\n";
//const char *AT_CWJAP_CUR	= "AT+CWJAP_CUR=\"Internet\",\"23141841\"\r\n";
//const char *AT_CIPMUX		= "AT+CIPMUX=1\r\n";
//const char *AT_CIPSERVER	= "AT+CIPSERVER=1,48778\r\n";
//const char *AT_CONNECT		= "0,CONNECT";
//const char *AT_CLOSED		= "0,CLOSED";
//const char *AT_IPD			= "+IPD,";












//Данные по TCP
uint8_t RXi; // Счетчик массива RX_BUF
uint8_t RXc; // Переменная для хранения пришедшего байта
uint8_t RX_BUF[RX_BUF_SIZE]; //Принятый масив
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







//Методы и функции
void SetSysClockTo72(void);

void ports_init(void);
void usartESP_init(void);

#endif
