#ifndef INIT_H
#define INIT_H


#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"

#include <string.h>
#include <stdlib.h>



//��������� � ��������
#define RX_BUF_SIZE		128// ������ ������������� ������ ������� ����� �������
#define BUF_SIZE 		9//������ ������ � ���������





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












//������ �� TCP
uint8_t RXi; // ������� ������� RX_BUF
uint8_t RXc; // ���������� ��� �������� ���������� �����
uint8_t RX_BUF[RX_BUF_SIZE]; //�������� �����
uint8_t TX_BUF[BUF_SIZE];//�������� ������� � �����������

uint8_t init; 	// ��������� ����������� ������� � ������
				// 1 - ��� ������������ (�������������������� ������ � ������� �������)
				// 0 - ������ ��������� (���������� �������������� � ���)
uint8_t id;		// ����� ������ � �������� ����������� ������ (�������� ����� �������)
uint8_t id_rx;	// ���������� �������� ����
uint8_t FLAG_REPLY;	// ���� ��� �������� ������� ��������� �� ������
uint8_t way_cmd;

char count[3]; // ���������� ������������ ��������

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







//������ � �������
void SetSysClockTo72(void);

void ports_init(void);
void usartESP_init(void);

#endif
