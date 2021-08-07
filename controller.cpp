#include "controller.h"

/*___________________________________________________________________________*/

static uint8_t pulse_finished_handler(void *event)
{
    const uint8_t relay_port = CONTAINER_OF(event, door_t, event)->relay_port;

    PORTC &= ~(1 << relay_port);

    return CANIOT_OK;
}

void GarageDoorController::actuate(door_t &door)
{
    PORTC |= 1 << door.relay_port;

    schedule(door.event, RELAY_TRIGGER_DELAY_MS);
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

/*___________________________________________________________________________*/

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

    doors[LEFT].event.handler = pulse_finished_handler;
    doors[RIGHT].event.handler = pulse_finished_handler;

    // todo set in configuration
    config.set_telemetry_period(TELEMETRY_PERIOD);
}

uint8_t GarageDoorController::command_handler(uint8_t buffer[8], uint8_t len)
{
    GarageDoorController * ctrl = (GarageDoorController*) get_instance();

    if (AS(buffer, CR_t)->relays.r1)
    {
        ctrl->actuate_left();
    }

    if (AS(buffer, CR_t)->relays.r2)
    {
        ctrl->actuate_right();
    }

    return CANIOT_OK;
}

uint8_t GarageDoorController::telemetry_builder(uint8_t buffer[8], uint8_t &len)
{
    GarageDoorController * ctrl = (GarageDoorController*) get_instance();

    AS(buffer, CR_t)->contacts = {
        .c1 = ctrl->doors[LEFT].state,
        .c2 = ctrl->doors[RIGHT].state,
    };

    CANIOT_GET_LEN(len, CRT);

    return CANIOT_OK;
}