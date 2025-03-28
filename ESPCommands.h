#ifndef ESPCommand
#define ESPCommand

enum ESPCommand {
    LEFT = 0x31,
    RIGHT = 0x30,
    FRONT = 0x33,
    BACK = 0x32,
    STOP = 0x35,
    FRONTLEFT = 0x34,
    FRONTRIGHT = 0x36,
    BACKLEFT = 0x37,
    BACKRIGHT = 0x38,
		SLOW = 0x39,
		FAST = 0x40,
		VICTORY = 0x41
};

#endif

