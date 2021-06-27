#ifndef _GARAGE_DOOR_CONTROLLER_H
#define _GARAGE_DOOR_CONTROLLER_H

#include "caniot/device.h"

#include <avr/io.h>

#include <mcp_can.h>
#include <caniot/device.h>
#include <caniot/hw_init.h>

/*___________________________________________________________________________*/

#define SPI_CS_PIN                  10
#define CAN_INT_PIN                 2

#define CAN_SPEED                   CAN_500KBPS
#define TELEMETRY_PERIOD            10*60

#define DOOR_OPEN_CLOSE_DELAY_CS    20 * 100

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
} door_t;


/*___________________________________________________________________________*/

class GarageDoorController : public can_device
{
protected:
    mcp2515_can can = mcp2515_can(SPI_CS_PIN);

    
public:
    door_t doors[2] = {
        {open, PORTC2, PIND3},
        {open, PORTC3, PIND4}
    };

    GarageDoorController() : can_device(&can, CAN_INT_PIN, CAN_SPEED, MCP_8MHz) { }

    void initialize(void);

    void poll_doors_status(void);

protected:
    void io_init(void);

    void actuate(door_t &door);
    inline void actuate_left(void) { actuate(doors[LEFT]); }
    inline void actuate_right(void) { actuate(doors[RIGHT]); }

private:
    static uint8_t command_handler(uint8_t buffer[8], uint8_t len);
    static uint8_t telemetry_builder(uint8_t buffer[8], uint8_t &len);
};


#endif