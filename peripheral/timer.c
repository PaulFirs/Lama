#include "main.h"
#include "init.h"

const uint8_t getppm[9]			= {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, ((uint16_t)0x0001)) != RESET)
	{
		if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)) {//нажата кнопка ручного включения люстры
			chan();
		}

    	switch(way_prep_mes){
			case DECODE:
				cmd.cmd_type = RX_BUF[0];
				switch (RX_BUF[0]) {            //читаем первый принятый байт-команду

					case SWITCH_LIGHT:
						command = SWITCH_LIGHT;
						TIM3->CNT = TIM3_PERIOD;
						break;

					case SET_TIME:

						for(uint8_t i = 0; i < 3; i++)
							time.h_m_s[i] = RX_BUF[1 + i];
						command = SET_TIME;
						TIM3->CNT = TIM3_PERIOD;
						break;

					case GET_TIME:
						cmd.arg.time = time;
						break;

					case SET_ALARM:
						for(uint8_t i = 0; i < 3; i++)
							alarm.h_m_s[i] = RX_BUF[1 + i];
						alarm.status = RX_BUF[4];
						alarm.settings = RX_BUF[5];
						command = SET_ALARM;
						TIM3->CNT = TIM3_PERIOD;
						break;

					case GET_ALARM:
						cmd.arg.alarm = alarm;
						cmd.arg.alarm.settings = settings.alarm;
						break;

					case GET_SENSORS:
						cmd.arg.sensors = sensors;
						break;

					default:
						cmd.cmd_type = 0xFF;
				}
				size_cmd = sizeof_cmd(cmd);
				itoa(size_cmd, count, 10);
				way_prep_mes = INIT_SENDMES;
			break;
    	}
	}
	TIM_ClearITPendingBit(TIM4, ((uint16_t)0x0001));// Обязательно сбрасываем флаг
}


void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, ((uint16_t)0x0001)) != RESET){
		if(way_prep_mes == WAIT_EQV){
			way_prep_mes = UPDATA;
		}
		switch(command){
			case SWITCH_LIGHT:
				chan();
				command = GET_TIME;
				break;

			case GET_SENSORS:
				USART3SendCMD(getppm, BUF_SIZE);
				GetCO2();
				sensors.temp = DS3231_read_temp();//Чтение темпеатуры из модуля
				command = GET_TIME;
				break;

			case SET_TIME:

				for(uint8_t i = 3; i; i--)
					I2C_single_write(DS_ADDRESS, (i-1), time.h_m_s[3 - i]);//Запись времени от телефона в модуль

			case GET_TIME:
				for(uint8_t i = 3; i; i--)
					time.h_m_s[3 - i] = I2C_single_read(DS_ADDRESS, (i-1));//Чтение времени
				command = GET_ALARM;
				break;

			case SET_ALARM:
				settings.alarm = alarm.settings;
			    FLASH_WriteSettings();
				if(alarm.status!=2)//Выполнять только если было изменение состояния будильника
					ds3231_on_alarm(alarm.status);

				else if(alarm.status==2){//Выполнять если изменилось время будильника
					for(uint8_t i = 3; i; i--)
						I2C_single_write(DS_ADDRESS, (i+6), alarm.h_m_s[3 - i]);
				}

			case GET_ALARM:
				for(uint8_t i = 3; i; i--)
					alarm.h_m_s[3 - i] = I2C_single_read(DS_ADDRESS, (i+6));//Чтение времени будильника

				alarm.status = I2C_single_read(DS_ADDRESS, DS3231_CONTROL) & (1 << DS3231_A1IE);//Чтение состояния будильника
				command = GET_SENSORS;
				break;
		}
	}
	TIM_ClearITPendingBit(TIM3, ((uint16_t)0x0001));// Обязательно сбрасываем флаг
}
