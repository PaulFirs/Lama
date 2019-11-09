#include "init.h"
#include "dwt_delay.h"
#include "usartESP.h"
#include "usartMH.h"
#include "i2c.h"
#include "ds3231.h"

const uint8_t getppm[9]			= {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};


void chan(void){
	for(uint8_t i = 3; i; i--){	//3 ��� ���������� 0b0000000111110110011100000 ��� ��������.
		uint32_t mes = MES;
		for(uint8_t i = 25; i; i--){
			if(mes & 1){              // �������� � �������� ����
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
	switch (cmd.cmd_type) {            //������ ������ �������� ����-�������

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

void TIM3_IRQHandler(void)
{
	static timer = 0;
	if (TIM_GetITStatus(TIM3, ((uint16_t)0x0001)) != RESET)
	{

		if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)) {//������ ������ ������� ��������� ������
			chan();
		}
		if(way_prep_mes == WAIT_EQV){
			way_prep_mes = UPDATA;
			if(timer > 100){
				command = GET_SENSORS;
				timer = 0;
			}
			if(timer == 30)
				command = GET_TIME;
			if(timer == 60)
				command = GET_ALARM;

			timer++;
		}

    	switch(way_prep_mes){
			case DECODE:
				timer = 0;
				cmd.cmd_type = RX_BUF[0];
				switch (RX_BUF[0]) {            //������ ������ �������� ����-�������

					case SWITCH_LIGHT:
						command = SWITCH_LIGHT;
						break;

					case SET_TIME:

						for(uint8_t i = 0; i < 3; i++)
							time.h_m_s[i] = RX_BUF[1 + i];
						command = SET_TIME;
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
						break;

					case GET_ALARM:
						cmd.arg.alarm = alarm;
						cmd.arg.alarm.settings = setting_alarm;
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
	TIM_ClearITPendingBit(TIM3, ((uint16_t)0x0001));// ����������� ���������� ����
}

void EXTI1_IRQHandler(void)//���������
{
	EXTI->PR|=EXTI_PR_PR1; //������� ����
	ds3231_del_alarm();
	GPIOC->ODR ^= GPIO_Pin_13;
	chan();
	//get_alarm();
}

//void get_alarm(void)//���������
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
	//������������� ����������
	init = 1;
	way_get_mes = WAIT;
	way_prep_mes = INIT_ESP;
	command = ZERO;


	//������������� ���������
	SetSysClockTo72();
	DWT_Init();
	ports_init();
	usartESP_init();
	timer_init();
	I2C1_init();

	//������������� �������
	DS3231_init();
	DWT_Delay_ms(1000); // �������� ��� ������������� ESP
	usartCN_init();
    while(1)
    {
    	switch(way_prep_mes){
			case INIT_ESP:
				init_ESP();
				way_prep_mes = UPDATA;
				command = GET_TIME;
				break;

			case UPDATA:

				way_prep_mes = WAIT_EQV;
				switch(command){
					case SWITCH_LIGHT:
						chan();
						break;

					case GET_SENSORS:
						USART3SendCMD(getppm, BUF_SIZE);
						GetCO2();
						sensors.temp = DS3231_read_temp();//������ ���������� �� ������
						break;

					case SET_TIME:

						for(uint8_t i = 3; i; i--)
							I2C_single_write(DS_ADDRESS, (i-1), time.h_m_s[3 - i]);//������ ������� �� �������� � ������

					case GET_TIME:
						for(uint8_t i = 3; i; i--)
							time.h_m_s[3 - i] = I2C_single_read(DS_ADDRESS, (i-1));//������ �������
						break;

					case SET_ALARM:
						setting_alarm = alarm.settings;
						if(alarm.status!=2)//��������� ������ ���� ���� ��������� ��������� ����������
							ds3231_on_alarm(alarm.status);

						if(alarm.status==2){//��������� ���� ���������� ����� ����������
							for(uint8_t i = 3; i; i--)
								I2C_single_write(DS_ADDRESS, (i+6), alarm.h_m_s[3 - i]);
						}

					case GET_ALARM:
						for(uint8_t i = 3; i; i--)
							alarm.h_m_s[3 - i] = I2C_single_read(DS_ADDRESS, (i+6));//������ ������� ����������

						alarm.status = I2C_single_read(DS_ADDRESS, DS3231_CONTROL) & (1 << DS3231_A1IE);//������ ��������� ����������
						break;
				}


				if(strstr((const char *)RX_BUF, "CLOSED")){
				}
				if(strstr((const char *)RX_BUF, "ERROR")){
					clear_Buffer(RX_BUF, RX_BUF_SIZE);
					init = 1;
					way_prep_mes = INIT_ESP;
				}
				if(strstr((const char *)RX_BUF, "FAIL")){
					clear_Buffer(RX_BUF, RX_BUF_SIZE);
					init = 1;
					way_prep_mes = INIT_ESP;
				}

				command = ZERO;
				break;





			case INIT_SENDMES:
				USARTSendSTR("AT+CIPSEND=");
				USARTSendCHAR(id);
				USARTSendCHAR(',');
				USARTSendSTR(count);
				USARTSendSTR("\r\n");
				way_prep_mes = WAIT_EQV;
				break;

			case SENDMES:
				USARTSendCMD((uint8_t *)&cmd,size_cmd);
				clear_Buffer(RX_BUF, RX_BUF_SIZE);
				way_get_mes = WAIT;
				way_prep_mes = WAIT_EQV;
				break;


		}
    }
}
