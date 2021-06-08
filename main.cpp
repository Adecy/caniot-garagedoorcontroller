#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <mcp_can.h>
#include <caniot/device.h>
#include <caniot/hw_init.h>
#include <uart.h>

#include <errno.h>

#define SPI_CS_PIN  10
#define CAN_INT_PIN 2

mcp2515_can can(SPI_CS_PIN);
can_device dev(&can, CAN_INT_PIN, CAN_500KBPS, MCP_8MHz);

uint32_t time_last = 0;
uint32_t time;

int main()
{
    hw_init();

    usart_init();

    dev.print_identification();

    dev.initialize();

    usart_u16(sizeof(struct can_device::attributes));
    
    while (1)
    {
        dev.process();

        // print updatime
        time = dev.uptime();
        if (time - time_last >= 10)
        {
            time_last = time;
            print_time_sec(time);

            usart_hex16(dev.abstime() >> 16);
            usart_hex16(dev.abstime());
        }
    }
}