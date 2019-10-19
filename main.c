#include "init.h"
#include "dwt_delay.h"

int main(void)
{
	SetSysClockTo72();

	DWT_Delay_sec(5);
    while(1)
    {
    }
}
