#include "init.h"

void SetSysClockTo72(void)
{
    ErrorStatus HSEStartUpStatus;
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
	/* ��������� RESET RCC (������ �� �����������, �� ������� �� ����� �������) */
	RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig( RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus == SUCCESS)
    {
    	 /* ��������� ��� ������� �������� ������������� ������ � FLASH.
    	    ���� �� �� ����������� ������������ � ����� ��������� ������� ������ � Flash,
    	    FLASH_PrefetchBufferCmd( ) �� FLASH_SetLatency( ) ����� ���������������� */
    	/* Enable Prefetch Buffer */
        //FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);

    	/* FLASH Latency.
		������������� �������������:FLASH_Latency_0 - 0 < SYSCLK≤ 24 MHz
		FLASH_Latency_1 - 24 MHz < SYSCLK ≤ 48 MHz
		FLASH_Latency_2 - 48 MHz < SYSCLK ≤ 72 MHz
		FLASH_SetLatency( FLASH_Latency_2);*/

        /* HCLK = SYSCLK */
        RCC_HCLKConfig( RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */
        RCC_PCLK2Config( RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */ /* ������ �� ����� APB1 Prescaler. ������� ������� �� 2 (RCC_HCLK_Div2)
		������ ��� �� ������ APB1 ������ ���� �� ����� 36��� (������ �����) */
	    RCC_PCLK1Config( RCC_HCLK_Div2);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        /* ��������� PLL �� ���� ����� ������� (RCC_PLLSource_HSE_Div1) � �� ������� �� �������� (RCC_PLLMul_9) */
        /* PLL ����� ����� ������� � ������ ��� ���� (RCC_PLLSource_HSE_Div1) ��� ���������� �� 2 (RCC_PLLSource_HSE_Div2). ������ ����� */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

        /* Enable PLL */
        RCC_PLLCmd( ENABLE);

        /* Wait till PLL is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock configuration.
     User can add here some code to deal with this error */

        /* Go to infinite loop */
        while (1)
        {
        }
    }
}

void ports_init(void)
{
	GPIO_InitTypeDef port;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);


	/* Configure (PC.13) as alternate function push-pull */
	port.GPIO_Speed = GPIO_Speed_50MHz;
	port.GPIO_Mode = GPIO_Mode_Out_PP;
	port.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &port);

	/* Configure Pins (PA.0,1,2,3,4) as output for indicate */
	/* Pins 0,1,2 - debug indicators*/
	/* Pin 3 - tx radio modle data*/
	port.GPIO_Speed = GPIO_Speed_2MHz;
	port.GPIO_Mode = GPIO_Mode_Out_PP;
	port.GPIO_Pin = 0b1111;
	GPIO_Init(GPIOA, &port);

	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	port.GPIO_Speed = GPIO_Speed_50MHz;
	port.GPIO_Mode = GPIO_Mode_AF_PP;
	port.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &port);

	/* Configure USART1 Rx (PA.10) as input floating */
	port.GPIO_Speed = GPIO_Speed_50MHz;
	port.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	port.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &port);
}

void usartESP_init(void)
{
    /* Enable USART1 and GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* NVIC Configuration */
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);



    USART_InitTypeDef USART_InitStructure; 					//��������� ��� ������������ USART

    //���� �������� � ���������
    USART_InitStructure.USART_BaudRate = 115200; 			// ���/���
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //��� � �����
    USART_InitStructure.USART_StopBits = USART_StopBits_1; 		// ���-�� ����-�����
    USART_InitStructure.USART_Parity = USART_Parity_No; 		// ����������� �����
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ���������� ���������� ��������� ������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 				//���/���� �����/��������

    USART_Init(USART1, &USART_InitStructure); // ������������� ��������� �������

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������� ����������
    USART_Cmd(USART1, ENABLE);				//��������� USART
}
