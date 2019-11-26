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
			if((RXc == '+') && (way_prep_mes != INIT_ESP)){
				way_get_mes = ID;
				clear_Buffer(RX_BUF, RX_BUF_SIZE);
			}
			break;

			case(ID):
				if(RXc == ':'){
					id = RX_BUF[4];
					id_rx = atoi (&RX_BUF[6]);
					clear_Buffer(RX_BUF, RX_BUF_SIZE);
					way_get_mes = RX_MODE;
				}
				break;

			case(RX_MODE):
				if(RXi == id_rx){
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
		for(uint8_t i = 0; i < 30; i++){
			for(int i = 0; i < 1000000; i++);
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

void init_ESP(void)
{
	clear_Buffer(RX_BUF, RX_BUF_SIZE);
	USARTSendCHAR(0xff);
	uint8_t response;
	do{
		response = 0;
		response |= uart_wite_for("AT\r\n", "OK");
		response |= uart_wite_for("AT+CWJAP_CUR=\"Internet\",\"23141841\"\r\n", "OK");
		response |= uart_wite_for("AT+CIPMUX=1\r\n", "OK");
		response |= uart_wite_for("AT+CIPSERVER=1,48778\r\n", "OK");
	}while(response);
}
