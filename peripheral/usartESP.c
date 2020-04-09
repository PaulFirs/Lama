#include "usartESP.h"


void clear_Buffer(uint8_t *pucBuffer, uint8_t size) {

    memset(pucBuffer, 0xFF, size);
	RXi = 0;
}

void USARTSendSTR(const char *pucBuffer)
{
	while (*pucBuffer)
	{
		USART_SendData(USART1, *pucBuffer++);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
		{
		}
	}
}
void USARTSendCMD(uint8_t *pucBuffer, uint8_t size)
{
    for (uint8_t i=0;i<size;i++)
    {
        USART_SendData(USART1, pucBuffer[i]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        {
        }
    }
}
void USARTSendCHAR(uint8_t pucBuffer)
{
	USART_SendData(USART1, pucBuffer);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}
}

void USART1_IRQHandler(void)
{
	if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET)
	{
		RXc = USART_ReceiveData(USART1);
		if(RXi > RX_BUF_SIZE)
			RXi = 0;

		RX_BUF[RXi] = RXc;
		RXi++;
		switch(way_get_mes){

			case(WAIT):
			if((RXc == '+') && (way_prep_mes != INIT_ESP) && (way_state == S_SERVER)){
				way_get_mes = ID;
				clear_Buffer(RX_BUF, RX_BUF_SIZE);
			}
			break;

			case(ID):
				if(RXc == ':'){
					id = RX_BUF[4];
					cnt_rx = atoi (&RX_BUF[6]);//первый символ количества символов =D
					clear_Buffer(RX_BUF, RX_BUF_SIZE);
					way_get_mes = RX_MODE;
				}
				break;

			case(RX_MODE):
				if(RXi == cnt_rx){
					way_prep_mes = DECODE;
					way_get_mes = GET_LESS;
					TIM4->CNT = 1000;
				}
			break;

			case(GET_LESS):
				if(RXc == '>'){
					way_prep_mes = SENDMES;
				}
				break;
		}
	}
}

uint8_t uart_wite_for(char *str, const char *resp) {
	for(uint8_t i = 0; i < 3; i++){
		char result = 0;
		uint8_t error = 0;
		USARTSendSTR(str);
		for(uint8_t j = 0; j < 30; j++){
			for(uint32_t del = 0; del < 1000000; del++);//задержка
			result = strstr((const char *)RX_BUF, resp);
			error = (strstr((const char *)RX_BUF, "ERROR")?1:0)|(strstr((const char *)RX_BUF, "FAIL")?1:0);
			if(result){
				clear_Buffer(RX_BUF, RX_BUF_SIZE);
				return 0;
			}
			else if(error){
				clear_Buffer(RX_BUF, RX_BUF_SIZE);
				return 2;
			}
		}
	}
	clear_Buffer(RX_BUF, RX_BUF_SIZE);
	return 1;
}



uint8_t uart_wait_str(const char *resp) {

	char result = 0;
	uint8_t error = 0;
	for(uint8_t j = 0; j < 300; j++){
		for(uint32_t del = 0; del < 1000000; del++);//задержка
		result = strstr((const char *)RX_BUF, resp);
		error = (strstr((const char *)RX_BUF, "ERROR")?1:0)|(strstr((const char *)RX_BUF, "FAIL")?1:0);
		if(result){
			clear_Buffer(RX_BUF, RX_BUF_SIZE);
			return 0;
		}
		else if(error){
			clear_Buffer(RX_BUF, RX_BUF_SIZE);
			return 2;
		}
	}
	clear_Buffer(RX_BUF, RX_BUF_SIZE);
	return 1;
}


void init_ESP(void)
{
	clear_Buffer(RX_BUF, RX_BUF_SIZE);
	USARTSendCHAR(0xff);
	uint8_t response;
	do{
		response = 0;
		response |= uart_wite_for("AT\r\n", "OK");
		response |= uart_wite_for("AT+CWMODE=3\r\n", "OK");
		response |= uart_wite_for("AT+CWJAP_CUR=\"Internet\",\"23141841\"\r\n", "OK");
		response |= uart_wite_for("AT+CIPMUX=1\r\n", "OK");
		response |= uart_wite_for("AT+CIPSERVER=1,48778\r\n", "OK");
		//response |= uart_wite_for("AT+CIPSTART=\"TCP\",\"192.168.0.72\",80\r\n", "OK");
	}while(response);
}


uint8_t m_sizeof(uint16_t data)
{
	if(data < 10)
		return 1;
	else if (data < 100)
		return 2;
	else if (data < 1000)
		return 3;
	else if (data < 10000)
		return 4;
}


void changeState_ESP(void)
{
	way_state = S_CLIENT;
	clear_Buffer(RX_BUF, RX_BUF_SIZE);
	uint8_t response;
	for(uint8_t control = 0; control < 3; control++){
		response = 0;

		switch(way_state){
			case S_SERVER:
				response |= uart_wite_for("AT+CIPCLOSE\r\n", "OK");
				response |= uart_wite_for("AT+CIPMUX=1\r\n", "OK");
				response |= uart_wite_for("AT+CIPSERVER=1,48778\r\n", "OK");
				way_get_mes = WAIT;
				return;
			case S_CLIENT:
				response |= uart_wite_for("AT+CIPSERVER=0\r\n", "OK");
				response |= uart_wite_for("AT+CIPMUX=0\r\n", "OK");
				response |= uart_wite_for("AT+CIPSTART=\"TCP\",\"192.168.0.72\",80\r\n", "OK");


				char buffer_co[4];
				char buffer_temp[2];


				itoa((int)((uint16_t)(sensors.co[2]<<8) | sensors.co[3]), buffer_co, 10);
				itoa(sensors.temp, buffer_temp, 10);



				itoa(53 + m_sizeof((uint16_t)(sensors.co[2]<<8) | sensors.co[3]) + m_sizeof(sensors.temp), count, 10);
				USARTSendSTR("AT+CIPSEND=");
				USARTSendSTR(count);
				USARTSendSTR("\r\n");
				uart_wait_str("\r\n>");//ждем приглашения


				USARTSendSTR ("GET /robot/100001/");
				USARTSendSTR (buffer_co);
				USARTSendCHAR('/');
				USARTSendSTR (buffer_temp);
				USARTSendSTR ("/ HTTP/1.1\r\nHost: 192.168.0.72\r\n\r\n");




				if(uart_wait_str("Ok") == 0){
					way_state = S_SERVER;
					control = 0;
				}
				break;
		}
		if((control == 1)&&(!response)){
			way_prep_mes = INIT_ESP;
			break;
		}
	}
}

