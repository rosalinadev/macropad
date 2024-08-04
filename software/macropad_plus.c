// ===================================================================================
// Project:   MacroPad Plus for CH551, CH552 and CH554
// Version:   v1.0
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Firmware example implementation for the MacroPad Plus.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: 16 MHz internal
// - Adjust the firmware parameters in src/config.h if necessary.
// - Customize the macro functions in the corresponding section below.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash' immediatly afterwards.
// - To compile the firmware using the Arduino IDE, follow the instructions in the 
//   .ino file.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a HID device with
//   keyboard, mouse and joystick interface.
// - Press a macro key or turn the knob and see what happens.
// - To enter bootloader hold down rotary encoder switch while connecting the 
//   MacroPad to USB. All NeoPixels will light up white as long as the device is in 
//   bootloader mode (about 10 seconds).


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "src/config.h"                     // user configurations
#include "src/system.h"                     // system functions
#include "src/delay.h"                      // delay functions
#include "src/neo.h"                        // NeoPixel functions
#include "src/usb_composite.h"              // USB HID composite functions

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

#pragma disable_warning 110                 // Keep calm, EVELYN!

// ===================================================================================
// Macro Functions which associate Actions with Events (Customize your MacroPad here!)
// ===================================================================================
/*
  The list of available USB HID functions can be found in src/usb_composite.h
  The keys are enumerated the following way:
                  -----
  +---+---+---+ /       \
  | 1 | 2 | 3 | |encoder|
  +---+---+---+ \       /
                  -----
*/

// Key 1 -> F13
// ---------------------------------------------

// Define action(s) if key1 was pressed
inline void KEY1_PRESSED() {
  KBD_press(KBD_KEY_F13);                             // press F13 key
}

// Define action(s) if key1 was released
inline void KEY1_RELEASED() {
  KBD_release(KBD_KEY_F13);                           // release F13 key
}

// Key 2 -> F14
// -----------------------------------------------

// Define action(s) if key2 was pressed
inline void KEY2_PRESSED() {
  KBD_press(KBD_KEY_F14);                             // press F14 key
}

// Define action(s) if key2 was released
inline void KEY2_RELEASED() {
  KBD_release(KBD_KEY_F14);                           // release F14 key
}

// Key 3 -> F15
// ---------------------------------------------

// Define action(s) if key3 was pressed
inline void KEY3_PRESSED() {
  KBD_press(KBD_KEY_F15);                             // press F15 key
}

// Define action(s) if key3 was released
inline void KEY3_RELEASED() {
  KBD_release(KBD_KEY_F15);                           // release F15 key
}

// Rotary encoder -> F16-F18
// ---------------------------------------------

// Define action(s) if encoder was rotated counter-clockwise
inline void ENC_CCW_ACTION() {
  KBD_type(KBD_KEY_F16);                             // press & release F16 key
}

// Define action(s) if encoder switch was pressed
inline void ENC_SW_PRESSED() {
  KBD_press(KBD_KEY_F17);                             // press F17 key
}

// Define action(s) if encoder switch was released
inline void ENC_SW_RELEASED() {
  KBD_release(KBD_KEY_F17);                           // release F17 key
}

// Define action(s) if encoder was rotated clockwise
inline void ENC_CW_ACTION() {
  KBD_type(KBD_KEY_F18);                             // press & release F18 key
}

// ===================================================================================
// NeoPixel Configuration
// ===================================================================================

// Global NeoPixel brightness
#define NEO_BRIGHT_KEYS   2         // NeoPixel brightness for keys (0..2)

// Key colors (hue value: 0..191)
#define NEO_KEY1          0         // red
#define NEO_KEY2          32        // yellow
#define NEO_KEY3          64        // green
#define NEO_KEY4          96        // cyan
#define NEO_KEY5          128       // blue
#define NEO_KEY6          160       // magenta

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Variables
  __bit key1last = 0;                             // last state of key 1
  __bit key2last = 0;                             // last state of key 2
  __bit key3last = 0;                             // last state of key 3
  __bit encAlast = 0;                             // last state of enc A
  __bit encSWlast = 0;                            // last state of encoder switch
  __idata uint8_t i;                              // temp variable

  // Setup
  NEO_init();                                     // init NeoPixels
  CLK_config();                                   // configure system clock
  DLY_ms(10);                                     // wait for clock to settle
  NEO_clearAll();                                 // clear NeoPixels

  // Enter bootloader if rotary encoder switch is pressed
  if(!PIN_read(PIN_ENC_SW)) {                     // encoder switch pressed?
    for(i=3*NEO_COUNT; i; i--) NEO_sendByte(127); // light up all pixels
    BOOT_now();                                   // enter bootloader
  }

  // Pan flag setup
  // pink:   255, 33,  140
  // yellow: 255, 216, 0
  // blue:   33,  177, 255
  NEO_writeColor(0, 255, 33,  140);
  NEO_writeColor(1, 255, 216, 0  );
  NEO_writeColor(2, 33,  177, 255);
  NEO_update();

  // Init USB HID device
  HID_init();                                     // init USB HID device
  DLY_ms(500);                                    // wait for Windows
  WDT_start();                                    // start watchdog timer

  // Loop
  while(1) {

    // Handle key 1
    // ------------
    if(!PIN_read(PIN_KEY1) != key1last) {         // key state changed?
      key1last = !key1last;                       // update last state flag
      if(key1last) {                              // key was pressed?
        KEY1_PRESSED();                           // take proper action
      }
      else {                                      // key was released?
        KEY1_RELEASED();                          // take proper action
      }
    }

    // Handle key 2
    // ------------
    if(!PIN_read(PIN_KEY2) != key2last) {         // key state changed?
      key2last = !key2last;                       // update last state flag
      if(key2last) {                              // key was pressed?
        KEY2_PRESSED();                           // take proper action
      }
      else {                                      // key was released?
        KEY2_RELEASED();                          // take proper action
      }
    }

    // Handle key 3
    // ------------
    if(!PIN_read(PIN_KEY3) != key3last) {         // key state changed?
      key3last = !key3last;                       // update last state flag
      if(key3last) {                              // key was pressed?
        KEY3_PRESSED();                           // take proper action
      }
      else {                                      // key was released?
        KEY3_RELEASED();                          // take proper action
      }
    }

    // Handle rotary encoder
    // ---------------------
    if(!PIN_read(PIN_ENC_A) != encAlast) {        // encoder turned ?
      encAlast = !encAlast;                       // update last state flag
      if(encAlast) {                              // encoder started turning
        if(PIN_read(PIN_ENC_B)) {                 // clockwise ?
          ENC_CW_ACTION();                        // take proper action
        }
        else {                                    // counter-clockwise ?
          ENC_CCW_ACTION();                       // take proper action
        }
      }
    }

    // Handle encoder switch
    // ------------
    if(!PIN_read(PIN_ENC_SW) != encSWlast) {      // key state changed?
      encSWlast = !encSWlast;                     // update last state flag
      if(encSWlast) {                             // key was pressed?
        ENC_SW_PRESSED();                         // take proper action
      }
      else {                                      // key was released?
        ENC_SW_RELEASED();                        // take proper action
      }
    }

    DLY_ms(2);                                    // debounce
    WDT_reset();                                  // reset watchdog
  }
}
