#include "hardware/structs/iobank0.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include <stdio.h>

#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#endif

#define GPIN 14
#define GPOUT 15

uint32_t gpio_get_events(uint gpio) {
  int32_t mask = 0xF << 4 * (gpio % 8);
  return (iobank0_hw->intr[gpio / 8] & mask) >> 4 * (gpio % 8);
}
void gpio_clear_events(uint gpio, uint32_t events) {
  gpio_acknowledge_irq(gpio, events);
}

int64_t trigger_fall(alarm_id_t id, void *user_data) {
  printf("fall timer %d fired!\n", (int)id);
  gpio_put(GPIN, 0);
  return 0;
}

int64_t pulse_follow_rise(alarm_id_t id, void *user_data) {
  printf("rise timer %d fired!\n", (int)id);
  gpio_put(GPOUT, 1);
  return 0;
}
int64_t pulse_follow_fall(alarm_id_t id, void *user_data) {
  gpio_put(GPOUT, 0);
  gpio_put(PICO_DEFAULT_LED_PIN, 1);
  return 0;
}

bool trigger_rise(struct repeating_timer *t) {
  // printf("Repeat at %lld\n", time_us_64());
  add_alarm_in_ms(10, trigger_fall, NULL, true);
  add_alarm_in_ms(100, pulse_follow_rise, NULL, true);
  add_alarm_in_ms(110, pulse_follow_fall, NULL, true);
  gpio_put(GPIN, 1);
  gpio_put(PICO_DEFAULT_LED_PIN, 0);
  return true;
}

int main() {
  uint64_t tStart = 0;
  uint64_t tEnd = 0;
  uint64_t tElapsed = 0;
  struct repeating_timer timer;
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;

  stdio_init_all();

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  gpio_set_function(GPIN, GPIO_FUNC_SIO);
  gpio_set_dir(GPIN, true);

  gpio_set_function(GPOUT, GPIO_FUNC_SIO);
  gpio_set_dir(GPOUT, true);

  add_repeating_timer_ms(1000, trigger_rise, NULL, &timer);

  while (true) {
    sleep_ms(5000);
  }
}
