#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <driver/spi_master.h>

#define DISPLAY_DIN 13
#define DISPLAY_CLK 33
#define DISPLAY_CS  25
#define DISPLAY_DC  27
#define DISPLAY_RST 26

#define WIDTH  320
#define HEIGHT 240

#define CHUNK 1024

extern uint16_t buffer[HEIGHT][WIDTH];
extern spi_device_handle_t display_handle;

void display_init(void);
void display_draw(int, int, int, int, uint16_t);
void display_write_buffer(void);
void display_send_cmd(uint8_t);
void display_send_data(uint8_t *, uint16_t);

#endif
