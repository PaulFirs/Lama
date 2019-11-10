#include "stm32f10x.h"
#include "stm32f10x_flash.h"

typedef struct
  {
    uint8_t alarm;     // 1 byte
    uint8_t n1;     // 1 byte
    uint8_t n2;     // 1 byte
    uint8_t n3;     // 1 byte

                            // 8 byte = 2  32-bits words.  It's - OK
                            // !!! Full size (bytes) must be a multiple of 4 !!!
  } tpSettings;

tpSettings settings;


void FLASH_Init(void);
void FLASH_ReadSettings(void);
void FLASH_WriteSettings(void);
