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
void display_draw(int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color);
void display_draw_text(const char *text, int16_t x, int16_t y, uint16_t color);
void display_write_buffer(void);
void display_send_cmd(uint8_t cmd);
void display_send_data(uint8_t *data, uint16_t size);

#endif
