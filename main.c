#include "init.h"
#include "dwt_delay.h"
#include "usartESP.h"


void chan(void){
	for(uint8_t i = 3; i; i--){	//3 раз отправляет 0b0000000111110110011100000 для верности.
		uint32_t mes = MES;
		for(uint8_t i = 25; i; i--){
			if(mes & 1){              // Передача с нулевого бита
				A3_ENABLE;
				DWT_Delay_us(DELAY_BIT);
				A3_DISABLE;
				DWT_Delay_us(DELAY_POUSE);
			}
			else{
				A3_ENABLE;
				DWT_Delay_us(DELAY_POUSE);
				A3_DISABLE;
				DWT_Delay_us(DELAY_BIT);
			}
			mes >>= 1;
		}
		DWT_Delay_us(DELAY_MESSAGE);
	}
}

void TIM3_IRQHandler(void)
{
	static uint8_t timer_sensors = 0;
	if (TIM_GetITStatus(TIM3, ((uint16_t)0x0001)) != RESET)
	{
		if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)) {//нажата кнопка ручного включения люстры
			chan();
			//way_cmd = WAIT;
		}
	}
	TIM_ClearITPendingBit(TIM3, ((uint16_t)0x0001));// Обязательно сбрасываем флаг
}



int main(void)
{
	init = 1;
	way_cmd = INIT_ESP;
	way_closed = C;

	SetSysClockTo72();
	DWT_Init();
	ports_init();
	timer_init();
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
				switch (RX_BUF[0]) {            //читаем первый принятый байт-команду
					case SWITCH_LIGHT:
						chan();
						break;
				}
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
				clear_Buffer(RX_BUF, RX_BUF_SIZE);
				clear_Buffer(TX_BUF, BUF_SIZE);
				way_cmd = WAIT;

				//GPIOC->ODR^=GPIO_Pin_13;
				break;
		}
    }
}
