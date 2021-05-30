#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <mcp_can.h>
#include <caniot/device.h>
#include <caniot/hw_init.h>
#include <uart.h>

#define SPI_CS_PIN  10
#define CAN_INT_PIN 2

mcp2515_can can(SPI_CS_PIN);
can_device dev(&can, CAN_INT_PIN, CAN_500KBPS, MCP_8MHz);

int main()
{
    hw_init();

    usart_init();

    dev.print_identification();

    dev.initialize();

    while (1)
    {
        if (dev.flag_can)
        {
            dev.process_message();
        }
    }
}