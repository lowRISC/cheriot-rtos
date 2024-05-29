#pragma once
#include <cdefs.h>
#include <stdint.h>

/**
 * A Simple Driver for the Sonata's RGB LEDs
 */

struct SonataRGBLEDCtrl
{
  uint32_t rgbled0;
  uint32_t rgbled1;
  uint32_t ctrl;
  uint32_t status;

  void wait_idle() volatile
  {
    while ((status & 0x1) == 0);
  }

  void set_rgb(uint8_t r, uint8_t g, uint8_t b, uint32_t led) volatile
  {
    uint32_t rgb;

    rgb = (uint32_t)r | ((uint32_t)g << 8) | ((uint32_t)b << 16);

    wait_idle();

    if (led == 0) {
      rgbled0 = rgb;
    } else if (led == 1) {
      rgbled1 = rgb;
    }
  }

  void update() volatile
  {
    wait_idle();
    ctrl = 0x1;
  }

  void clear() volatile
  {
    wait_idle();
    ctrl = 0x2;
  }
};

