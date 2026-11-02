# Armasight Tau 2 Controller

Starter firmware for an ESP32-C3 Super Mini and FLIR Tau 2 core. It sends `Hello World` through the Tau 2 symbol/text API exposed by [`rynskyi/flir-tau2`](https://github.com/rynskyi/flir-tau2).

## Build and upload

Install PlatformIO Core, then from this project directory run:

```sh
pio run
pio run --target upload
pio device monitor
```

The PlatformIO environment uses the `esp32-c3-devkitm-1` board definition as a starting point for the Super Mini. USB CDC is used for logs; UART0 is used for the Tau connection.

## UART configuration

Defaults in `src/main.cpp`:

- ESP32-C3 RX: GPIO 20
- ESP32-C3 TX: GPIO 21
- UART: 115200 baud, 8 data bits, no parity, 1 stop bit

Change `TAU_UART_RX_PIN`, `TAU_UART_TX_PIN`, and `TAU_UART_BAUD` to match your board wiring and Tau configuration before building.

Wire ESP32 TX to Tau RX, ESP32 RX to Tau TX, and connect signal grounds. **Do not connect the core until you have verified its connector pinout and electrical interface.** Confirm the Tau UART uses compatible logic-level signaling; if it is RS-232-level, use an appropriate transceiver, not a direct ESP32 connection. The Tau core also needs its own correctly rated power supply.

## Symbol demo

The firmware invokes:

```cpp
tauCam.printText("Hello World", 200, 200,
                 TAU_COLOR_TRANSPRNT, TAU_COLOR_WHITE,
                 TAU_FONT_ALIGN_CENTER, TAU_FONT_LARGE);
```

The repository documents a 640 x 512 symbol canvas. If the text is not visible, verify the Tau symbol feature is enabled, UART settings and wiring are correct, and the selected coordinates/font are supported by your core configuration.

## API scope and dependency

The selected library documents these wrappers: palette, zoom, spot meter, orientation, video standard, text, rectangle, and raw `sendCommand`. That is useful for this starter, but it is **not a complete implementation of every Tau 2 ICD command**. The API names and available commands can be extended against the ICD for your exact core firmware.

The project fetches the library directly from GitHub rather than copying its source. Its repository does not expose a license file through GitHub's license endpoint; verify the author's licensing terms before redistributing firmware that includes it. Library source and API: https://github.com/rynskyi/flir-tau2
