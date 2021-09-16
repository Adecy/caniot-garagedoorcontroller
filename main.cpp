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
        controller.poll_doors_status();
        controller.process();
        
        // sleep for 10ms
        sleep_mode();
    }
}
