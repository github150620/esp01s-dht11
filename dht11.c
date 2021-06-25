// 
// 
// 
#include "driver/gpio.h"

static gpio_num_t dht11_pin;

static void dht11_delay_ms(uint16_t i) {
  while (i--) {
    os_delay_us(1000);
  }
}

static void dht11_SetPinOutput(void) {
  gpio_config_t config;
  config.pin_bit_mask = (1 << dht11_pin);
  config.mode         = GPIO_MODE_OUTPUT_OD;
  gpio_config(&config);
}

static void dht11_SetPinInput(void) {
  gpio_config_t config;
  config.pin_bit_mask = (1 << dht11_pin);
  config.mode         = GPIO_MODE_INPUT;
  config.pull_up_en   = GPIO_PULLUP_DISABLE;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpio_config(&config);
}

void dht11_Init(gpio_num_t data_pin) {
  dht11_pin = data_pin;
}

int dht11_ReadHumiAndTemp(int *humi, int *temp) {
  uint8_t data[5];
  uint8_t i, j;
  
  dht11_SetPinOutput();
  gpio_set_level(dht11_pin, 0); // Pull down >18ms
  delay_ms(20);
  
  dht11_SetPinInput(); // Auto pull up 
  
  for (i=0;i<200;i++) {
    if (gpio_get_level(dht11_pin)==0) break;
    os_delay_us(1);
  }
  if (i >= 200) return -1;
  
  for (i=0;i<200;i++) {
    if (gpio_get_level(dht11_pin)==1) break;
    os_delay_us(1);    
  }
  if (i >= 200) return -2;

  for (i=0;i<200;i++) {
    if (gpio_get_level(dht11_pin)==0) break;
    os_delay_us(1);    
  }
  if (i >= 200) return -3;

  for (j=0;j<40;j++) {
    for (i=0;i<200;i++) {
      if (gpio_get_level(dht11_pin)==1) break;
      //os_delay_us(1);
    }
    if (i >= 200) return -4;

    for (i=0;i<200;i++) {
      if (gpio_get_level(dht11_pin)==0) break;
      //os_delay_us(1);
    }
    if (i >= 200) return -5;

    data[j/8] <<= 1;
    if (i > 5) {
      data[j/8] |= 0x01;
    } else {
      data[j/8] &= ~0x01;
    }
  }
  
  if (((data[0] + data[1] + data[2] + data[3])&0xFF) != data[4]) {
    return -6;
  }
  
  *humi = data[0];
  *temp = data[2];
  
  return 0;
}
