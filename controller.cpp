#include "controller.h"

void GarageDoorController::io_init(void)
{
    // outputs relays
    DDRC |= (1 << PORTC2) | (1 << PORTC3);
    PORTC &= ~((1 << PORTC2) | (1 << PORTC3));

    // inputs
    DDRD &= ~((1 << PORTD3) | (1 << PORTD4));
    PORTD &= ~((1 << PORTD3) | (1 << PORTD4)); // disable pull up

    poll_doors_status();
}

void GarageDoorController::initialize(void)
{
    hw_init();
    io_init();
    usart_init();

    set_command_handler(command_handler);
    set_telemetry_builder(telemetry_builder);

    can_device::initialize();

    p_config->telemetry_period = TELEMETRY_PERIOD;
}

uint8_t GarageDoorController::command_handler(uint8_t buffer[8], uint8_t len)
{
    GarageDoorController * ctrl = (GarageDoorController*) get_instance();

    if (buffer[0] & 0b01)
    {
        ctrl->actuate_left();
    }

    if (buffer[0] & 0b10)
    {
        ctrl->actuate_right();
    }

    return CANIOT_OK;
}

uint8_t GarageDoorController::telemetry_builder(uint8_t buffer[8], uint8_t &len)
{
    GarageDoorController * ctrl = (GarageDoorController*) get_instance();

    buffer[1] |= (ctrl->doors[LEFT].state) | ((ctrl->doors[RIGHT].state) << 1);
    
    CANIOT_GET_LEN(len, CRT);

    return CANIOT_OK;
}

/*___________________________________________________________________________*/

void GarageDoorController::actuate(door_t &door)
{
    PORTC |= 1 << door.relay_port;

    _delay_ms(RELAY_TRIGGER_DELAY_MS);

    PORTC &= ~(1 << door.relay_port);
}

// we suppose that the doors are not moving down or up
void GarageDoorController::poll_doors_status(void)
{
    for (uint_fast8_t idx = 0; idx < sizeof(doors) / sizeof(door_t); idx++)
    {
        door_state_t previous = doors[idx].state;

        doors[idx].state = (door_state_t) (PIND >> doors[idx].contact_port);

        // on state change, send a new telemetry message
        if (doors[idx].state != previous)
        {
            SET_FLAG_TELEMETRY(flags);
        }
    }
}