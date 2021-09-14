#ifndef _GARAGE_DOOR_CONTROLLER_H
#define _GARAGE_DOOR_CONTROLLER_H

#include "caniot/device.h"

#include <avr/io.h>

#include <mcp_can.h>
#include <caniot/device.h>
#include <caniot/hw_init.h>
#include <pcb1/custom.h>

/*___________________________________________________________________________*/

#define CAN_SPEED                   CAN_500KBPS

#define DOOR_OPEN_CLOSE_DELAY_CS    20*100
#define RELAY_PULSE_DURATION_MS     500

#define TELEMETRY_PERIOD            15

/*___________________________________________________________________________*/

#define LEFT 0
#define RIGHT 1

enum door_state_t : uint8_t
{
    open = 0,
    close = 1,
};

typedef struct
{
    door_state_t state;
    uint8_t relay_port;
    uint8_t contact_port;
    event_t event; // pulse finished event
} door_t;

/*___________________________________________________________________________*/

class GarageDoorController : public CustomBoard
{

public:
    door_t doors[2] = {
        {open, PORTC2, IN2_PORT},  // IN0
        {open, PORTC3, IN3_PORT}   // IN1
    };

    GarageDoorController() : CustomBoard(CAN_SPEED, MCP_16MHz) { }

    void initialize(void);

    void poll_doors_status(void);

protected:
    void actuate(door_t &door);
    inline void actuate_left(void) { actuate(doors[LEFT]); }
    inline void actuate_right(void) { actuate(doors[RIGHT]); }

    static uint8_t command_handler(uint8_t buffer[8], uint8_t len);
    static uint8_t telemetry_builder(uint8_t buffer[8], uint8_t &len);
};

/*___________________________________________________________________________*/

#endif