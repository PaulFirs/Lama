#include "init.h"





void clear_Buffer(uint8_t *pucBuffer, uint8_t size);
void USARTSendCHAR(uint8_t pucBuffer);
void USARTSendSTR(const char *pucBuffer);
void USARTSendCMD(uint8_t *pucBuffer, uint8_t size);
uint8_t uart_wite_for(char *str, const char *resp);
void init_ESP(void);
void changeState_ESP(void);

void USART3SendCMD(const uint8_t pucBuffer[], uint8_t size);
