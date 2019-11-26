#include "main.h"



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

uint8_t sizeof_cmd(struct t_cmd cmd){
	uint8_t size = sizeof(cmd.cmd_type);
	size += sizeof(cmd.check_sum);
	switch (cmd.cmd_type) {            //читаем первый принятый байт-команду

		case SWITCH_LIGHT:
			return size;

		case SET_TIME:
			return size;

		case GET_TIME:
			return size += sizeof(struct t_time);

		case SET_ALARM:
			return size;

		case GET_ALARM:
			return size += sizeof(struct t_alarm);

		case GET_SENSORS:
			return size += sizeof(struct t_sensors);
		default:
			return size;
	}
}
void EXTI1_IRQHandler(void)//будильник
{
	EXTI->PR|=EXTI_PR_PR1; //Очищаем флаг
	ds3231_del_alarm();
	chan();
	//get_alarm();
}

//void get_alarm(void)//будильник
//{
//	if(stat_alarm & ALARM_CHAN){
//		chan();
//	}
//	if(stat_alarm & ALARM_LIGHT){
//	}
//	if(stat_alarm & ALARM_SING){
//	}
//	if(stat_alarm & ALARM_WINDOW){
//	}
//}


int main(void)
{
	//инициализация переменных
	way_get_mes = WAIT;
	way_prep_mes = INIT_ESP;
	command = GET_TIME;


	//инициализация периферии
	SetSysClockTo72();
	DWT_Init();
	ports_init();
	usartESP_init();
	timer_init();
	I2C1_init();

	//инициализация модулей
	DS3231_init();
	DWT_Delay_ms(1000); // задержка для инициализации ESP
	usartCN_init();
	//GPIOC->ODR ^= GPIO_Pin_13;
    while(1)
    {
    	switch(way_prep_mes){
			case INIT_ESP:
				init_ESP();
				way_prep_mes = UPDATA;
				break;

			case UPDATA:
				GPIOC->ODR ^= GPIO_Pin_13;
				way_prep_mes = WAIT_EQV;
				char *mes;
				if(mes = strstr((const char *)RX_BUF, "+IPD")){
//					id = mes[4];
//					id_rx = atoi (&mes[6]);
//					way_prep_mes = DECODE;
//					TIM4->CNT = 1000;
				}
				else if(strstr((const char *)RX_BUF, "CLOSED")){
					way_get_mes = WAIT;
				}
				else if((strstr((const char *)RX_BUF, "ERROR"))
						||(strstr((const char *)RX_BUF, "FAIL"))){
					clear_Buffer(RX_BUF, RX_BUF_SIZE);
					way_get_mes = WAIT;
					way_prep_mes = INIT_ESP;
				}
				break;


			case INIT_SENDMES:
				USARTSendSTR("AT+CIPSEND=");
				USARTSendCHAR(id);
				USARTSendCHAR(',');
				USARTSendSTR(count);
				USARTSendSTR("\r\n");
				//way_get_mes = GET_LESS;
				way_prep_mes = WAIT_EQV;
				break;

			case SENDMES:
				USARTSendCMD((uint8_t *)&cmd, size_cmd);
				clear_Buffer(RX_BUF, RX_BUF_SIZE);
				way_get_mes = WAIT;
				way_prep_mes = WAIT_EQV;
				break;
		}
    }
}
