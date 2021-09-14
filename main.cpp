#include <avr/io.h>
#include <util/delay.h>
#include <uart.h>

#include <avr/sleep.h>

#include "controller.h"

GarageDoorController controller = GarageDoorController();

int main()
{
    controller.initialize();
    controller.print_identification();
    
    while (1)
    {
        controller.process();
        controller.poll_doors_status();
        
        // sleep for 10ms
        sleep_mode();
    }
}
