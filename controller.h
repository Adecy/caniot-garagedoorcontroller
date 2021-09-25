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

#define RELAY_PULSE_DURATION_MS     500

#define TELEMETRY_PERIOD            5*60    // s

/*___________________________________________________________________________*/

#define LEFT    0
#define RIGHT   1
#define GATE    2

enum door_state_t : uint8_t
{
    open = 1,
    close = 0,
};

typedef struct
{
    door_state_t state;
    uint8_t relay_port;
    uint8_t inx;
    event_t event; // pulse finished event
} door_t;

/*___________________________________________________________________________*/

class GarageDoorController : public CustomBoard
{

public:
    door_t doors[3] = {
        {open, PORTC2, IN2},
        {open, PORTC3, IN3},
        {open, 0xFF, IN1},
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