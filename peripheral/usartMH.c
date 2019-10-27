#include "usartMH.h"

void USART3SendCMD(const uint8_t *pucBuffer, uint8_t size)
{
    for (uint8_t i=0;i<size;i++)
    {
        USART_SendData(USART3, pucBuffer[i]);
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
        {
        }
    }
}


void GetCO2(void){
	RXi_MH = 0;
	while(RXi_MH < BUF_SIZE){
		for(uint16_t i = 10000; i; i--){
			if((USART3->SR & USART_FLAG_RXNE) != (u16)RESET){
				TX_BUF[RXi_MH] = USART_ReceiveData(USART3); //Присвоение элементу массива значения очередного байта

				break;
			}
		}
		RXi_MH++;//переход к следующему элементу массива.
	}
}
