#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#include <mcp_can.h>
#include <caniot/device.h>
#include <caniot/hw_init.h>
#include <uart.h>

#include <errno.h>


#define SPI_CS_PIN  10
#define CAN_INT_PIN 2

mcp2515_can can(SPI_CS_PIN);
can_device dev(&can, CAN_INT_PIN, CAN_500KBPS, MCP_8MHz);

uint8_t command_handler(uint8_t buffer[8], uint8_t len)
{
    PIND = (buffer[1] & 0xF) << 4 | (PIND & 0x0F);

    usart_print("Received a CAN Garage Command : ");
    usart_hex(buffer[0]);
    usart_transmit('\n');

    return CANIOT_OK;
}

uint8_t telemetry_builder(uint8_t buffer[8], uint8_t &len)
{
    buffer[0] = PINC & 0xF;
    buffer[1] = PIND >> 4;
    len = 4;
    
    return CANIOT_OK;
}

void io_init(void)
{
    // ports configured as ouput
    // PD0, PD1 for USART RT and TX
    // PD2 for INT0 (external interrupt)
    // PD3 for INT1 (external interrupt)

    // DIGITAL OUTPUT PD4 to PD7
    DDRD |= 0b11110000;
    PORTD &= ~0b11110000; // set to 0 (clear)

    // DIGITAL INPUT PC0 to PC3
    DDRC &= ~0b1111;
    // reading value from here
    // PINC
    
    // ANALOG
    ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2) | (1 << ADEN); // enable analog and set prescaler to maximum precision
    ADMUX &= ~(1 << REFS0 | 1 << REFS1);    // set Analog REF to "AREF, Internal Vref turned off"
}

// ADC 6 or 7
uint16_t read_analog(uint8_t analog_pin)
{
    uint8_t low, high;

    // set pin
    ADMUX |= analog_pin & 0x07;
    
    // start conversion
    ADCSRA |= 1 << ADSC;

    while(ADCSRA & (1 << ADSC));

    low  = ADCL;
	high = ADCH;

    return (high << 8) | low;
}

int main()
{
    hw_init();
    io_init();
    usart_init();

    dev.print_identification();
    dev.initialize();

    dev.set_telemetry_builder(telemetry_builder);
    dev.set_command_handler(command_handler);

    dev.p_config->telemetry_period = 10*60; // sec

    debug_masks_filters();
    
    while (1)
    {
        dev.process();
    }
}
