/*
 * Display.h
 *
 *  Created on: Jan 21, 2022
 *      Author: DeepOcean
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void xTFT_LUT_24bit(uint16_t start_page, uint16_t start_col, uint16_t total_page, uint16_t total_col, const unsigned short *fill_data);
char* charConvert(uint16_t data, char* buffer);
void PrintOut (uint8_t temperature, uint8_t noise, uint32_t colour);







#endif /* INC_DISPLAY_H_ */
