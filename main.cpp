#include <avr/io.h>
#include <avr/interrupt.h>

#include <caniot/caniot.h>
#include <uart.h>

can_device can;

int main()
{
    usart_init();

    usart_u8((uint8_t) can.m_id);

    while (1)
    {

    }
}