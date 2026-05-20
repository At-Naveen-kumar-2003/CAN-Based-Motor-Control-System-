#include <SPI.h>
#include <mcp2515.h>

#define POT_PIN   34
#define CS_PIN    5
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  23

MCP2515 mcp2515(CS_PIN);

struct can_frame canMsg;

bool initCAN() {

    mcp2515.reset();
    delay(100);

    if (mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ)
        != MCP2515::ERROR_OK) {

        return false;
    }

    mcp2515.setNormalMode();

    return true;
}

void setup() {

    Serial.begin(115200);

    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

    analogReadResolution(12);

    while (!initCAN()) {

        delay(1000);
    }
}

void loop() {

    uint16_t potValue = analogRead(POT_PIN);

    canMsg.can_id  = 0x123;
    canMsg.can_dlc = 2;

    canMsg.data[0] = (potValue >> 8) & 0xFF;
    canMsg.data[1] = potValue & 0xFF;

    mcp2515.sendMessage(&canMsg);

    delay(100);
}
