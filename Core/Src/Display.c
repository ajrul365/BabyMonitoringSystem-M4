/*
 * Display.c
 *
 *  Created on: Jan 21, 2022
 *      Author: DeepOcean
 */
#include "main.h"
#include "RAiO.h"
#include "Display.h"


I2C_HandleTypeDef hi2c1;
//UART_HandleTypeDef hlpuart1;


void xTFT_TFTtestColour(void)
{
//    uint32_t address;

//    if ((TFT_Xres==800)&&(TFT_Yres==480))   {address=0;}
//    else if ((TFT_Xres==1024)&&(TFT_Yres==600))   {address=0x046500;}
//    else if ((TFT_Xres==1280)&&(TFT_Yres==800))   {address=0x0B6D00;}
//    else if ((TFT_Xres==1280)&&(TFT_Yres==768))   {address=0x0B6D00;}

//    xTFT_LUT_24bit(0,0,300,512,Sleep);
//    xTFT_LUT_24bit(0,512,300,512,mario);
//    xTFT_LUT_24bit(300,0,300,512,mario);
//    xTFT_LUT_24bit(300,512,300,512,mario);

//    dma_24bitAddressBlockMode(0,1,0,0,TFT_Xres/2,TFT_Yres/2,TFT_Xres/2,address);
//    dma_24bitAddressBlockMode(0,1,TFT_Xres/2,0,TFT_Xres/2,TFT_Yres/2,TFT_Xres/2,address);
//    dma_24bitAddressBlockMode(0,1,0,TFT_Yres/2,TFT_Xres/2,TFT_Yres/2,TFT_Xres/2,address);
//    dma_24bitAddressBlockMode(0,1,TFT_Xres/2,TFT_Yres/2,TFT_Xres/2,TFT_Yres/2,TFT_Xres/2,address);
}


//void xTFT_LUT_24bit(uint16_t start_page, uint16_t start_col, uint16_t total_page, uint16_t total_col, const unsigned short *fill_data)
//{
//    uint16_t i, j;
//    uint16_t temp1, temp2, temp3;
//
//    activeWindowXY(start_col,start_page);
//    activeWindowWH(total_col,total_page);
//    setPixelCursor(start_col,start_page);
//    ramAccessPrepare();
//	//RA8876_SDRAMAccess();                                  // Memory Read/Write
//
////    R_DC = 1;
//    for(j=0; j<total_page; j++)
//    {
//        for(i=0; i<total_col/2; i++)
//        {
//            temp1 = *fill_data++;
//            temp2 = *fill_data++;
//            temp3 = *fill_data++;
//
////            C_DB = ((temp1&0x00FF)<<8) + ((temp2&0xFF00)>>8);
////            C_WR_CLR;
////            C_WR_SET;
////            C_DB = ((temp3&0x00FF)<<8) + ((temp1&0xFF00)>>8);
////            C_WR_CLR;
////            C_WR_SET;
////            C_DB = ((temp2&0x00FF)<<8) + ((temp3&0xFF00)>>8);
////            C_WR_CLR;
////            C_WR_SET;
//        }
//    }
//}


char* charConvert(uint16_t data, char* buffer)
{
    itoa(data,buffer,10);
    return buffer;
}

void PrintOut (uint8_t temperature, uint8_t noise, uint32_t colour)
{   //char tempChar[10];
	char temp[10];
	char dB[10];

    activeWindowXY(0,0);
    activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT);
    setPixelCursor(0,0);

//    do{
    charConvert(temperature,temp);
    charConvert(noise,dB);

    strcat(temp," C");
    strcat(dB," dB");

    if ((temperature <23)||(temperature >26))
    	colour = 0x00FF0000;
    else
    	colour = 0x00FFFFFF;

    putString (30,100,"TEMPERATURE: ");
    drawSquareFill(30,150,250,200,colour);
    putString (30,150," "); putStringData (temp); HAL_Delay(500);


    if (noise>60)
    	colour = 0x00FF0000;
    else
    	colour = 0x00FFFFFF;

    putString (30,250,"NOISE: ");
    drawSquareFill(30,300,250,350,colour);
    putString (30,300," "); putStringData (dB); HAL_Delay(500);
//    }while (count);
}
