#include <Arduino.h>
#include <TauCam.h>

#ifndef TAU_UART_RX_PIN
#define TAU_UART_RX_PIN 20
#endif

#ifndef TAU_UART_TX_PIN
#define TAU_UART_TX_PIN 21
#endif

#ifndef TAU_UART_BAUD
#define TAU_UART_BAUD 57600
#endif

#define CROSSHAIR_INNER_SIZE 100
#define CROSSHAIR_OUTER_SIZE 200
#define CROSSHAIR_INNER_THICKNESS 1
#define CROSSHAIR_OUTER_THICKNESS 3

int counter = 0;

namespace
{
    HardwareSerial TauUart(0);

    void tauWriteByte(uint8_t byte)
    {
        TauUart.write(byte);
    }

    uint32_t tauRxByteCount = 0; // TEMP: serial number diagnostic

    uint8_t tauReadByte()
    {
        tauRxByteCount++;
        return static_cast<uint8_t>(TauUart.read());
    }

    uint32_t tauAvailableBytes()
    {
        return static_cast<uint32_t>(TauUart.available());
    }

    void tauDelay(uint32_t durationMs)
    {
        delay(durationMs);
    }

    TauCam tauCam(tauWriteByte, tauReadByte, tauAvailableBytes, tauDelay);

}

void drawCrosshair(
    uint16_t posX = TAU_CANVAS_WIDTH / 2, 
    uint16_t posY = TAU_CANVAS_HEIGHT / 2,
    uint16_t extendWidth = (TAU_CANVAS_WIDTH - TAU_CANVAS_HEIGHT) / 2, // default = aspect ratio correction
    uint8_t innerColor = TAU_COLOR_BLACK, 
    uint8_t outerColor = TAU_COLOR_BLACK)
{
    uint16_t innerSizeHorizontal = CROSSHAIR_INNER_SIZE + extendWidth;
    uint16_t outerSizeHorizontal = CROSSHAIR_OUTER_SIZE + extendWidth;

    // inner crosshair
    // horizontal line
    tauCam.drawRectangle(100, posX - innerSizeHorizontal / 2, posY, innerSizeHorizontal, CROSSHAIR_INNER_THICKNESS, innerColor);
    // vertical line
    tauCam.drawRectangle(101, posX, posY - CROSSHAIR_INNER_SIZE / 2, CROSSHAIR_INNER_THICKNESS, CROSSHAIR_INNER_SIZE, innerColor);

    // outer crosshair (4 lines)
    // horizontal line left
    tauCam.drawRectangle(102, posX - innerSizeHorizontal / 2 - outerSizeHorizontal + innerSizeHorizontal, posY - 1, outerSizeHorizontal - innerSizeHorizontal, CROSSHAIR_OUTER_THICKNESS, outerColor);
    // horizontal line right
    tauCam.drawRectangle(103, posX + innerSizeHorizontal / 2 , posY - 1, outerSizeHorizontal - innerSizeHorizontal, CROSSHAIR_OUTER_THICKNESS, outerColor);
    // vertical line top
    tauCam.drawRectangle(104, posX - 1, posY - CROSSHAIR_INNER_SIZE / 2 - CROSSHAIR_OUTER_SIZE + CROSSHAIR_INNER_SIZE, CROSSHAIR_OUTER_THICKNESS, CROSSHAIR_OUTER_SIZE - CROSSHAIR_INNER_SIZE, outerColor);
    // vertical line bottom
    tauCam.drawRectangle(105, posX - 1, posY + CROSSHAIR_INNER_SIZE / 2, CROSSHAIR_OUTER_THICKNESS, CROSSHAIR_OUTER_SIZE - CROSSHAIR_INNER_SIZE, outerColor);
}

void updateHUD()
{
    /*
    // Example: update crosshair position based on some logic
    static uint16_t posX = TAU_CANVAS_WIDTH / 2;
    static uint16_t posY = TAU_CANVAS_HEIGHT / 2;

    // Update position (for demonstration, moving in a circle)
    static float angle = 0.0f;
    angle += 0.01f; // Increment angle
    posX = static_cast<uint16_t>(TAU_CANVAS_WIDTH / 2 + (TAU_CANVAS_WIDTH / 4) * cos(angle));
    posY = static_cast<uint16_t>(TAU_CANVAS_HEIGHT / 2 + (TAU_CANVAS_HEIGHT / 4) * sin(angle));

    drawCrosshair(posX, posY);
    */

    

    uint32_t cameraSerial = 0;
    uint32_t sensorSerial = 0;
    uint8_t serialStatus = tauCam.getSerialNumbers(cameraSerial, sensorSerial);

    char serialNumberStr[20];
    if (serialStatus == TAU_OK)
    {
        sprintf(serialNumberStr, "Serial: %lu", (unsigned long)cameraSerial);
    }
    else
    {
        sprintf(serialNumberStr, "Serial err: 0x%02X", serialStatus);
    }

    tauCam.printText(
        49,
        serialNumberStr,
        40,
        400,
        TAU_COLOR_BLUE,
        TAU_COLOR_WHITE,
        TAU_FONT_ALIGN_LEFT,
        TAU_FONT_SMALL);


    char videoStandardStr[20];
    TauVideoStandard currentStandard;
    uint8_t standardStatus = tauCam.get(TauCmd::VideoStandard, currentStandard);

    if (standardStatus != TAU_OK)
    {
        sprintf(videoStandardStr, "Video err: 0x%02X", standardStatus);
    }
    else
    {
        switch (currentStandard)
        {
            case TauVideoStandard::Ntsc30:
                sprintf(videoStandardStr, "Video Std: NTSC 30");
                break;
            case TauVideoStandard::Ntsc60:
                sprintf(videoStandardStr, "Video Std: NTSC 60");
                break;
            case TauVideoStandard::Pal25:
                sprintf(videoStandardStr, "Video Std: PAL 25");
                break;
            case TauVideoStandard::Pal50:
                sprintf(videoStandardStr, "Video Std: PAL 50");
                break;
            default:
                sprintf(videoStandardStr, "Video Std: Unknown");
                break;
        }
    }

    tauCam.printText(
        50,
        videoStandardStr,
        320,
        400,
        TAU_COLOR_BLUE,
        TAU_COLOR_WHITE,
        TAU_FONT_ALIGN_LEFT,
        TAU_FONT_SMALL);

    char counterStr[20];
    sprintf(counterStr, "Counter: %d", counter);

    tauCam.printText(
        51,
        counterStr,
        0,
        0,
        TAU_COLOR_BLUE,
        TAU_COLOR_WHITE,
        TAU_FONT_ALIGN_LEFT,
        TAU_FONT_SMALL);
}

void setup()
{
    Serial.begin(115200);
    TauUart.begin(TAU_UART_BAUD, SERIAL_8N1, TAU_UART_RX_PIN, TAU_UART_TX_PIN);

    delay(5000); // Wait for the camera readiness time (4 to 5 s for 640 configurations)

    tauCam.set(TauCmd::VideoPalette, TauPalette::Fusion);

    tauCam.set(TauCmd::SpatialThreshold, tauSpatialThresholdAuto(-5)); // Automatic DDE, threshold -5

    tauCam.setVideoStandard(TauVideoStandard::Pal50);
    
    drawCrosshair(320, 256);

    updateHUD();
}



void loop()
{
    delay(1000);
    counter++;
    if (counter > 10)
    {
        counter = 0;
    }
    updateHUD();
}
