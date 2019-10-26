#include "init.h"





void clear_Buffer(char *pucBuffer, uint8_t size);
void USARTSendCHAR(uint8_t pucBuffer);
void USARTSendSTR(const char *pucBuffer);
void USARTSendCMD(uint8_t *pucBuffer, uint8_t size);
void uart_wite_for(char *str);
void init_ESP(void);

