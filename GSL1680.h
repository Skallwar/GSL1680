/**************************************************************************/
/*!
    @file     GSL1680.h
    @author   Skallwar

    @thanks to wolfmanjm for the source code this lib is bassed on : https://github.com/wolfmanjm/GSL1680
*/

#ifndef GSL1680_H
#define GSL1680_H

#include <stdint.h>

class GSL1680 {
    public:
        GSL1680();
        GSL1680(bool error, bool info);

        void begin(uint8_t WAKE, uint8_t INTRPT);
        uint8_t dataread();
        uint8_t readFingerID(int NB);
        uint32_t readFingerX(int NB);
        uint32_t readFingerY(int NB);

    private:
        bool GSL1680_DEBUG_ERROR;
        bool GSL1680_DEBUG_INFO;
        void clear_reg();
        void reset();
        void loadfw();
        void startchip();
        void sleep();
        void datasend(uint8_t REG, uint8_t DATA[], uint16_t NB);
};

struct Finger {
    uint8_t fingerID;
    uint32_t x;
    uint32_t y;
};

struct Touch_event {
    uint8_t NBfingers;
    struct Finger fingers[5];
};

#endif
