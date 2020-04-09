#include "main.h"
#include "init.h"

const uint8_t getppm[9]			= {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
const uint8_t ppm2k[9]			= {0xff, 0x01, 0x99, 0x00, 0x00, 0x00, 0x07, 0xd0, 0x8f};
const uint8_t ppm5k[9]			= {0xff, 0x01, 0x99, 0x00, 0x00, 0x00, 0x13, 0x88, 0xcb};
const uint8_t abc[9]			= {0xff, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86}; //ABC logic on/off (ABC = automatic baseline correction)
const uint8_t clbd[9]			= {0xff, 0x01, 0x88, 0x00, 0x00, 0x00, 0x07, 0xD0, 0xa0};

uint8_t *cmd_mh;

extern uint8_t ticks_client;

uint8_t CRC8(uint8_t *pucBuffer, uint8_t size) {

    uint8_t crc = 0;
	uint8_t flag = 0;
	uint8_t data = 0;
	for(uint8_t i = 1; i<size; i++){
		data = pucBuffer[i];
		for (int j = 0; j < 8; j++) {
			flag = crc^data;
			flag = flag&0x01;
			crc = crc>>1;
			data = data >> 1;
			if (flag)
				crc = crc ^ 0x8C;
		}
	}
	return crc;
}



void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, ((uint16_t)0x0001)) != RESET)
	{
		if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)) {//нажата кнопка ручного включения люстры
			//get_alarm();
			chan();
		}
		if(way_prep_mes == WAIT_EQV){
			way_prep_mes = UPDATA;
		}

    	switch(way_prep_mes){
			case DECODE:
				cmd.cmd_type = RX_BUF[1];
				switch (RX_BUF[1]) {            //читаем первый принятый байт-команду

					case SWITCH_LIGHT:
						command = SWITCH_LIGHT;
						TIM3->CNT = TIM3_PERIOD-2000;
						break;

					case SET_TIME:

						for(uint8_t i = 0; i < 3; i++)
							time.h_m_s[i] = RX_BUF[2 + i];
						command = SET_TIME;
						TIM3->CNT = TIM3_PERIOD;
						break;

					case GET_TIME:
						cmd.arg.time = time;
						break;

					case SET_ALARM:
						for(uint8_t i = 0; i < 3; i++)
							alarm.h_m_s[i] = RX_BUF[2 + i];
						alarm.status = RX_BUF[5];
						alarm.settings = RX_BUF[6];
						command = SET_ALARM;
						TIM3->CNT = TIM3_PERIOD;
						break;

					case GET_ALARM:
						cmd.arg.alarm = alarm;
						cmd.arg.alarm.settings = settings.alarm;
						break;

					case SENSORS:

						switch(RX_BUF[2]){
						case GET_SENSORS:
							cmd_mh = getppm;
							cmd.arg.sensors = sensors;
							break;
						case PPM2K:
							cmd_mh = ppm2k;
							break;
						case PPM5K:
							cmd_mh = ppm5k;
							break;
						case ABC:
							cmd_mh = abc;
							break;
						}
						break;

					default:
						cmd.cmd_type = 0xFF;
				}

				size_cmd = sizeof_cmd(cmd);
				cmd.check_sum = CRC8((uint8_t *)&cmd, size_cmd);


				itoa(size_cmd, count, 10);
				way_prep_mes = INIT_SENDMES;
				clear_Buffer(RX_BUF, RX_BUF_SIZE);

			break;
    	}
	}
	TIM_ClearITPendingBit(TIM4, ((uint16_t)0x0001));// Обязательно сбрасываем флаг
}


void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, ((uint16_t)0x0001)) != RESET){

		if(!ticks_client){
			ticks_client = DELAY_WEB;
			way_prep_mes = CHANGE_STATE_ESP;
		}
		ticks_client--;

		switch(command){
			case SWITCH_LIGHT:
				chan();
				command = GET_TIME;
				break;

			case SENSORS:
				if((cmd_mh[2] == 0x99)||(cmd_mh[2] == 0x79)){
					USART3SendCMD(cmd_mh, BUF_SIZE);
					cmd_mh = getppm;
				}
				for(int i = 0; i < 100000; i++);
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
				command = SENSORS;
				break;
		}
	}
	TIM_ClearITPendingBit(TIM3, ((uint16_t)0x0001));// Обязательно сбрасываем флаг
}
