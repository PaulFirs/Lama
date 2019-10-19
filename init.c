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

