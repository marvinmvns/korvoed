#include "speech_commands_action.h"
#include <stdio.h>

void wake_up_action(void)
{
    printf("Wake word detected\n");
}

void speech_commands_action(int command_id)
{
    printf("Command ID %d detected\n", command_id);
}
