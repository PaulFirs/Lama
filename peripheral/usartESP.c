#include "usartESP.h"

inline void clear_Buffer(char *pucBuffer, uint8_t size) {

    for (uint8_t i=0;i<size;i++){
    	pucBuffer[i] = '\0';
    }
	RXi = 0;
	FLAG_REPLY = 0;
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
    clear_Buffer(TX_BUF, BUF_SIZE);//очистка буфера TX
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

		if(init)
		{
			if ((RXc != 255) && (RXc != 0) && (RXc != 10))
			{
				if (RXc != 13)
				{
					RX_BUF[RXi] = RXc;
					RXi++;
					if (RXi > RX_BUF_SIZE-1)
					{
						clear_Buffer(RX_BUF, RX_BUF_SIZE);
					}

				}
				else
				{
					FLAG_REPLY = 1;
				}
			}
		}
		else {//'dslgoh
			RX_BUF[RXi] = RXc;
			RXi++;
			switch(way_cmd){
				case(ID):
					if(RXc == ':'){
						id = RX_BUF[4];
						id_rx = atoi (&RX_BUF[6]);
						clear_Buffer(RX_BUF, RX_BUF_SIZE);
						way_cmd = RX_MODE;
					}
					break;
				case(RX_MODE):
						if(RXi == id_rx){
							way_cmd = DECODE;
						}
					break;

				case(INVITATION):
					if(RXc == '>'){
						way_cmd = SENDMES;
					}
					break;
				case(WAIT):
					if(RXc == '+'){
						way_cmd = ID;
						clear_Buffer(RX_BUF, RX_BUF_SIZE);
					}
					/*if(strstr(RX_BUF, "+IPD,")){
						way_cmd = ID;
						clear_Buffer(RX_BUF, BUF_SIZE);

					}*/
					/*if(strstr(RX_BUF, "0,CLOSED")){
						clear_Buffer(RX_BUF, BUF_SIZE);
						init = 1;
						way_cmd = INIT_ESP;
					}*/
					switch(way_closed){
						case(C):
							if(RXc == 'C')
								way_closed = L;
							break;
						case(L):
							if(RXc == 'L')
								way_closed = O;
							break;
						case(O):
							if(RXc == 'O')
								way_closed = S;
							break;
						case(S):
							if(RXc == 'S')
								way_closed = E;
							break;
						case(E):
							if(RXc == 'E')
								way_closed = D;
							break;
						case(D):
							if(RXc == 'D'){
								clear_Buffer(RX_BUF, RX_BUF_SIZE);
								init = 1;
								get_sensors = 0;
								way_cmd = INIT_ESP;
								way_closed = C;
							}
							break;
					}
					break;
			}
		}
		if(strstr(RX_BUF, "ERROR")){
			init = 1;
			get_sensors = 0;
			way_cmd = INIT_ESP;
			clear_Buffer(RX_BUF, RX_BUF_SIZE);

		}
	}
}

void uart_wite_for(char *str) {
	char result = 0;

	while (result == 0){
		FLAG_REPLY = 0;
		clear_Buffer(RX_BUF, RX_BUF_SIZE);
		while (FLAG_REPLY == 0) {

		}
		result = strstr(RX_BUF, str);
	}
}

void init_ESP(void)
{
	USARTSendCHAR(0xff);
	USARTSendSTR("AT\r\n");
	uart_wite_for("OK");
	USARTSendSTR("AT+CWJAP_CUR=\"Internet\",\"23141841\"\r\n");
	uart_wite_for("OK");
	USARTSendSTR("AT+CIPMUX=1\r\n");
	uart_wite_for("OK");
	USARTSendSTR("AT+CIPSERVER=1,48778\r\n");
	uart_wite_for("OK");

	uart_wite_for("0,CONNECT");

	clear_Buffer(RX_BUF, RX_BUF_SIZE);
}
