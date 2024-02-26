// Copyright 2023-2024 REMAKE.AI, KAIA.AI, MAKERSPET.COM
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Works
// https://forums.raspberrypi.com/viewtopic.php?t=310298
// https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf
// https://github.com/Gerriko/PicoCapSense/
// .\pioasm.exe touch.pio touch.pio.h
#include <hardware/pio.h>
#include "touch.pio.h"

#define TOUCH_PIN 16
#define STATE_MACHINE 0
#define PRECHARGE_CLOCKS 100000

PIO pio = pio0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(5000);

  uint offset = pio_add_program(pio, &touchsense_program);
  Serial.print("Loaded program at ");
  Serial.println(offset);
  //int sm = pio_claim_unused_sm(pio, false);
  touchsense_program_init(pio, STATE_MACHINE, offset, TOUCH_PIN);
  pio_sm_set_enabled(pio, STATE_MACHINE, true);
  
  //pio_sm_put_blocking(pio, STATE_MACHINE, PRECHARGE_CLOCKS);

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);

  // https://www.raspberrypi.com/documentation/pico-sdk/hardware.html
  uint count = pio_sm_get_rx_fifo_level(pio, STATE_MACHINE);
  uint sum = 0;
  if (count == 8) {
    for (int i = 0; i < count; i++) {
      uint clock_cycles = pio_sm_get_blocking(pio, STATE_MACHINE);
      sum += 0xffffffff - clock_cycles;
    }
    Serial.println(sum);
  }

  digitalWrite(LED_BUILTIN, LOW);
  delay(50);
}
