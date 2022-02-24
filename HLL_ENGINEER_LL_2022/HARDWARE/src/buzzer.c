#include "buzzer.h"
//PH6
void Buzzer_Init(void)
{
		tim12_base_init();
    BUZZER_OFF();
}

