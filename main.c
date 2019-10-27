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

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, ((uint16_t)0x0001)) != RESET)
	{

		if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)) {//������ ������ ������� ��������� ������
			chan();
		}
	}
	TIM_ClearITPendingBit(TIM3, ((uint16_t)0x0001));// ����������� ���������� ����
}

void EXTI1_IRQHandler(void)//���������
{
	EXTI->PR|=EXTI_PR_PR1; //������� ����
	ds3231_del_alarm();
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
	way_cmd = INIT_ESP;
	way_closed = C;
	get_sensors = 0;

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
    	switch(way_cmd){
			case INIT_ESP:
				init_ESP();
				way_cmd = WAIT;
				init = 0;
				break;

			case DECODE:
				TX_BUF[0] = RX_BUF[0];//����������� �������� �������
				switch (RX_BUF[0]) {            //������ ������ �������� ����-�������
					case SWITCH_LIGHT:
						chan();
						break;
					case SET_TIME://������� ������� � GET_TIME. ��� ������� ������ ��� ������������� ������� � ���������
						for(uint8_t i = 3; i; i--)
							I2C_single_write(DS_ADDRESS, (i-1), RX_BUF[4-i]);//������ ������� �� �������� � ������

					case GET_TIME:
						for(uint8_t i = 3; i; i--)
							TX_BUF[4-i] = I2C_single_read(DS_ADDRESS, (i-1));//������ �������
						break;

					case GET_SET_ALARM:
						if(RX_BUF[5]){//���� �����������, ���� ������������ ������������ ���������
							//stat_alarm = RX_BUF[6];
							TX_BUF[6] = RX_BUF[6];
							if(RX_BUF[4]!=2)//��������� ������ ���� ���� ��������� ��������� ����������
								ds3231_on_alarm(RX_BUF[4]);

							if(RX_BUF[4]==2){//��������� ���� ���������� ����� ����������
								for(uint8_t i = 3; i; i--)
									I2C_single_write(DS_ADDRESS, (i+6), RX_BUF[4-i]);
							}
						}

						for(uint8_t i = 3; i; i--)
							TX_BUF[4-i] = I2C_single_read(DS_ADDRESS, (i+6));//������ ������� ����������

						TX_BUF[4] = I2C_single_read(DS_ADDRESS, DS3231_CONTROL) & (1 << DS3231_A1IE);//������ ��������� ����������
						break;

					case GET_SENSORS:
						switch(RX_BUF[1]) {

						case GET_TEMP:
							TX_BUF[0] = GET_SENSORS;
							TX_BUF[1] = GET_TEMP;
							TX_BUF[2] = DS3231_read_temp();//������ ���������� �� ������
							break;


						case GET_CARB:
							USART3SendCMD(getppm, BUF_SIZE);
							GetCO2();

							TX_BUF[0] = GET_SENSORS;
							TX_BUF[1] = GET_CARB;
							break;
						}
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
				break;
		}
    }
}
