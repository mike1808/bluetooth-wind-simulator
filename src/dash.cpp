#include "dash.h"

int16_t rev_limits[dash_width][2];

void find_rev_limits() {
	int16_t byteWidth = (dash_width + 7) / 8; // Bitmap scanline pad = whole byte
	uint8_t byte = 0;

	for (int16_t y = 0; y < dash_height; y++) {
		for (uint16_t x = 0; x < dash_width; x++) {
			if (x & 7)
				byte >>= 1;
			else
				byte = dash_bits[y * byteWidth + x / 8];

			if (byte & 0x01) {
				if (rev_limits[x][0] == 0) {
					rev_limits[x][0] = y + 2;
				} else {
					rev_limits[x][1] = y - 1;
				}
			}
		}
	}
}

int16_t lastRevsX = 0;

void drawRevs(Adafruit_SSD1306 &display, int percent) {
	int16_t xTarget = (float(percent) / 100) * dash_width;

	if (lastRevsX == xTarget);

	int16_t xEnd;
	int16_t xStart;
	uint16_t c;


	if (lastRevsX < xTarget) {
		xStart = lastRevsX;
		xEnd = xTarget;
		c = SSD1306_WHITE;
	} else {
		xStart = xTarget;
		xEnd = lastRevsX;
		c = SSD1306_BLACK;
	}

	Serial.printf("start, end [%d, %d]\r\n", xStart, xEnd);

	lastRevsX = xTarget;

	for (int16_t x = xStart; x < xEnd; x++) {
		Serial.printf("%d [%d, %d]\r\n", x, rev_limits[x][0], rev_limits[x][1]);
		for (int16_t y = rev_limits[x][0]; y < rev_limits[x][1]; y++) {
			display.drawPixel(x, y, c);
		}
	}
}