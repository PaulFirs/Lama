#include "init.h"

void SetSysClockTo72(void)
{
    ErrorStatus HSEStartUpStatus;
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
	/* Системный RESET RCC (делать не обязательно, но полезно на этапе отладки) */
	RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig( RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus == SUCCESS)
    {
    	 /* Следующие две команды касаются исключительно работы с FLASH.
    	    Если вы не собираетесь использовать в своей программе функций работы с Flash,
    	    FLASH_PrefetchBufferCmd( ) та FLASH_SetLatency( ) можно закомментировать */
    	/* Enable Prefetch Buffer */
        //FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);

    	/* FLASH Latency.
		Рекомендовано устанавливать:FLASH_Latency_0 - 0 < SYSCLKв‰¤ 24 MHz
		FLASH_Latency_1 - 24 MHz < SYSCLK в‰¤ 48 MHz
		FLASH_Latency_2 - 48 MHz < SYSCLK в‰¤ 72 MHz
		FLASH_SetLatency( FLASH_Latency_2);*/

        /* HCLK = SYSCLK */
        RCC_HCLKConfig( RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */
        RCC_PCLK2Config( RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */ /* Смотри на схеме APB1 Prescaler. Частота делится на 2 (RCC_HCLK_Div2)
		потому что на выходе APB1 должно быть не более 36МГц (смотри схему) */
	    RCC_PCLK1Config( RCC_HCLK_Div2);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        /* Указываем PLL от куда брать частоту (RCC_PLLSource_HSE_Div1) и на сколько ее умножать (RCC_PLLMul_9) */
        /* PLL может брать частоту с кварца как есть (RCC_PLLSource_HSE_Div1) или поделенную на 2 (RCC_PLLSource_HSE_Div2). Смотри схему */
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

	/* Configure USART3 Tx (PB.10) as alternate function push-pull */
	port.GPIO_Speed = GPIO_Speed_50MHz;
	port.GPIO_Mode = GPIO_Mode_AF_PP;
	port.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &port);

	/* Configure USART3 Rx (PB.11) as input floating */
	port.GPIO_Speed = GPIO_Speed_50MHz;
	port.GPIO_Mode = GPIO_Mode_IN_FLOATING;//болтающийся в воздухе пин, чтоб к нему подвести TX (она же имеет 3В!!!)
	port.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &port);

	/* Configure Pin (PB.6,7) as SCK and SDA for I2C */
	port.GPIO_Speed = GPIO_Speed_2MHz;
	port.GPIO_Mode = GPIO_Mode_AF_OD;
	port.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &port);

	/* Configure Pin (PB.0) as input(btn) for chan */
	port.GPIO_Speed = GPIO_Speed_2MHz;
	port.GPIO_Mode = GPIO_Mode_IPU;
	port.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &port);

	//Событие срабатывания будильника на ds3231
	//прерывание на PORTB_1
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	//Прерывания - это альтернативная функция порта
	//поэтому надо установить бит Alternate function I/O clock enable
	//в регистре RCC_APB2ENR
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN , ENABLE);
	//Наша задача получить в регистре EXTICR[0] такую комбинацию бит
	// 0000 0000 0001 0000
	// по умолчанию там ноли, поэтому установим только 1 бит
	AFIO->EXTICR[0]|=AFIO_EXTICR1_EXTI1_PB;
	//Прерывания от 1 ноги разрешены
	EXTI->IMR|=(EXTI_IMR_MR1);
	//Прерывания на ногах по нарастающему фронту
	EXTI->FTSR|=(EXTI_RTSR_TR1);
	//Разрешаем оба прерывания
	NVIC_EnableIRQ(EXTI1_IRQn);
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



    USART_InitTypeDef USART_InitStructure; 					//Структура для конфигурации USART

    //Сбор настроек в структуру
    USART_InitStructure.USART_BaudRate = 115200; 			// бод/сек
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //бит в слове
    USART_InitStructure.USART_StopBits = USART_StopBits_1; 		// кол-во стоп-Битов
    USART_InitStructure.USART_Parity = USART_Parity_No; 		// Контрольная сумма
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // аппаратное управление передачей данных
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 				//вкл/выкл прием/передача

    USART_Init(USART1, &USART_InitStructure); // инициализация собранных настрек

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//Включение прерываний
    USART_Cmd(USART1, ENABLE);				//Включение USART
}


void timer_init(void)
{

	TIM_TimeBaseInitTypeDef timer;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Prescaler = 7200;//раз в секунду считает таймер 72000000/7200 = 10000 раз в секунду
    timer.TIM_Period = 1000;//набрав столько сработает прерывание 1000/10000 = 0.1 секунд
	TIM_TimeBaseInit(TIM3, &timer);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);		//Enabling in main func

    /* NVIC Configuration */
    NVIC_InitTypeDef NVIC_InitStructure;
	    /* Enable the TIM3_IRQn Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void I2C1_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	I2C_InitTypeDef I2C_InitStructure;
	I2C_StructInit(&I2C_InitStructure);
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_InitStructure.I2C_OwnAddress1 = 1;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}
