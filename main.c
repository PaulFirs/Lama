#include "init.h"
#include "dwt_delay.h"
#include "usartESP.h"


const char *AT_OK			= "OK";
const char *AT_AT			= "AT\r\n";
const char *AT_CWJAP_CUR	= "AT+CWJAP_CUR=\"Internet\",\"23141841\"\r\n";
const char *AT_CIPMUX		= "AT+CIPMUX=1\r\n";
const char *AT_CIPSERVER	= "AT+CIPSERVER=1,48778\r\n";
const char *AT_CONNECT		= "0,CONNECT";
const char *AT_CLOSED		= "0,CLOSED";
const char *AT_IPD			= "+IPD,";




int main(void)
{
	init = 1;
	way_cmd = INIT_ESP;

	SetSysClockTo72();
	DWT_Init();
	ports_init();
	usartESP_init();

	DWT_Delay_sec(5);


    while(1)
    {
    	switch(way_cmd){
			case INIT_ESP:
				init_ESP();
				way_cmd = WAIT;
				init = 0;
				break;

			case DECODE:
				TX_BUF[0] = RX_BUF[0];//копирование ответной команды
				itoa(BUF_SIZE, count, 10);
				way_cmd = INIT_SENDMES;
				break;


			case INIT_SENDMES:
				USARTSendSTR("AT+CIPSEND=");
				USARTSendCHAR(id);
				USARTSendCHAR(',');
				USARTSendSTR(count);
				USARTSendSTR("\r\n");
				way_cmd = INVITATION;
				break;

			case SENDMES:
				USARTSendCMD(TX_BUF, BUF_SIZE);
				way_cmd = END;
				break;



			case END:
				clear_Buffer(RX_BUF, BUF_SIZE);
				clear_Buffer(TX_BUF, BUF_SIZE);
				way_cmd = WAIT;
				break;
		}
    }
}
