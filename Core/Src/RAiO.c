/*
 * RAiO.c
 *
 *  Created on: Jan 14, 2022
 *      Author: DeepOcean
 */
#include "main.h"
#include "RAiO.h"
#include "Display.h"


I2C_HandleTypeDef hi2c1;
//UART_HandleTypeDef hlpuart1;

//void RA8876_Init(void)
//{
//	printf("\n\rStart Initilze");
//
//}

//**************************************************************//
/*
File Name : xRAiO.cpp
Author    : RAiO Application Team
Edit Date : 09/13/2017
Version   : v2.0  1.modify bte_DestinationMemoryStartAddr bug
                  2.modify ra8876SdramInitial Auto_Refresh
                  3.modify ra8876PllInitial
*/
//**************************************************************//

//#define mydebug
////**************************************************************//
// Ra8876_Lite::Ra8876_Lite(int xnscs,int xnreset)
//{
//  _xnscs = xnscs;
//  _xnreset = xnreset;
//}
////**************************************************************//

////**************************************************************//
//void begin(void)
//{
//  uint16_t temp;
//  //initialize the bus for Due
//  SPI.begin(_xnscs);
//  //On the Due, the system clock can be divided by values from 1 to 255.
//  //The default value is 21, which sets the clock to 4 MHz like other Arduino boards
//  SPI.setClockDivider(_xnscs,21);
//  SPI.setDataMode(_xnscs,SPI_MODE0);
//  SPI.setBitOrder(MSBFIRST);
//
//  //ra8876 hardware reset
//  pinMode(_xnreset, OUTPUT);
//  digitalWrite(_xnreset, HIGH);
//  xDelay(1);
//  digitalWrite(_xnreset, LOW);
//  xDelay(1);
//  digitalWrite(_xnreset, HIGH);
//  xDelay(10);
//  if(!checkIcReady())
//  {return 0;}
//
//  #ifdef mydebug
//  Serial.println("mydebug");
//  #endif
//
//  //read ID code must disable pll, 01h bit7 set 0
//  lcdRegDataWrite(0x01,0x08);
//  xDelay(1);
//  if ((lcdRegDataRead(0xff) != 0x76)&&(lcdRegDataRead(0xff) != 0x77))
//  {Serial.println("RA8876 or RA8877 not found!");
//    return 0;}
//  else
//  {Serial.println("RA8876 or RA8877 connect pass!");}
//
//  if(!ra8876Initialize())
//  {Serial.println("ra8876 or RA8877 initial fail!");
//   return 0;}
//  else
//  {Serial.println("RA8876 or RA8877 initial Pass!");}
//
//  SPI.setClockDivider(_xnscs,2);
//  return 1;
//}
//**************************************************************//

void RA8876_Reset()
{
	printf("\n\rSOFTWARE RESET\n");
	HAL_GPIO_WritePin(GPIOA, R_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOA, R_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOA, R_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
}


void RA8876_Init()
{
    RA8876_Reset();            	// RA8876 hardware reset

    ra8876PllInitial();    		//  RA8876 PLL initialization
    ra8876SdramInitial();

  //******************************* RA8876 General setting *******************************//
    lcdRegWrite(RA8876_CCR);     // 0x01, Chip Configuration Register
    lcdDataWrite(RA8876_PLL_ENABLE<<7|RA8876_WAIT_NO_MASK<<6|RA8876_KEY_SCAN_DISABLE<<5|RA8876_TFT_OUTPUT24<<3
    |RA8876_I2C_MASTER_DISABLE<<2|RA8876_SERIAL_IF_ENABLE<<1|RA8876_HOST_DATA_BUS_8BIT);

    lcdRegWrite(RA8876_MACR);        // 0x02, Memory Access Control Register
    lcdDataWrite(RA8876_DIRECT_WRITE<<6|RA8876_READ_MEMORY_LRTB<<4|RA8876_WRITE_MEMORY_LRTB<<1);

    lcdRegWrite(RA8876_ICR);         // 0x03, Input Control Register
    lcdDataWrite(RA8876_LVDS_FORMAT<<3|RA8876_GRAPHIC_MODE<<2|RA8876_MEMORY_SELECT_IMAGE);

    lcdRegWrite(RA8876_MPWCTR);      // 0x10, Main/PIP Window Control Register
    lcdDataWrite(RA8876_IMAGE_COLOCR_DEPTH_24BPP<<2|TFT_MODE);

    lcdRegWrite(RA8876_PIPCDEP);   // 0x11
    lcdDataWrite(RA8876_PIP1_COLOR_DEPTH_24BPP<<2|RA8876_PIP2_COLOR_DEPTH_24BPP);

    lcdRegWrite(RA8876_AW_COLOR);    // 0x5E, Colour Depth of CAnvas & Active Window
    lcdDataWrite(RA8876_CANVAS_BLOCK_MODE<<2|RA8876_CANVAS_COLOR_DEPTH_24BPP);

    xTFT_Com_Data(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_24BPP<<5|RA8876_S1_COLOR_DEPTH_24BPP<<2|RA8876_S0_COLOR_DEPTH_24BPP);//92h

   //******************************* RA8876 TFT timing setting *******************************//
    lcdRegWrite(RA8876_DPCR);        // 0x12, Display Configuration Register
    lcdDataWrite(XPCLK_INV<<7|RA8876_DISPLAY_ON<<6|RA8876_COLOR_BAR_DISABLE<<5|RA8876_VDIR_TB<<3|RA8876_OUTPUT_RGB);

    lcdRegWrite(RA8876_PCSR);        // 0x13, Panel Scan Clock & Data Setting Register
    lcdDataWrite(XHSYNC_INV<<7|XVSYNC_INV<<6|XDE_INV<<5);

	lcdHorizontalWidthVerticalHeight(HDW,VDH);
	lcdHorizontalNonDisplay(HND);
	lcdHsyncStartPosition(HST);
	lcdHsyncPulseWidth(HPW);
	lcdVerticalNonDisplay(VND);
	lcdVsyncStartPosition(VST);
	lcdVsyncPulseWidth(VPW);

	//************************ RA8876 Image display memory and windows initialized setting ************************//
	displayImageStartAddr(PAGE1_START_ADDR);
	displayImageWidth(SCREEN_WIDTH);
	displayWindowStartXY(0,0);
	canvasImageStartAddr(PAGE1_START_ADDR);
	canvasImageWidth(SCREEN_WIDTH);
	activeWindowXY(0,0);
	activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT);

	//************************ RA8876 Text and CGROM setting ************************//
	textColor(COLOR16M_WHITE, COLOR16M_BLACK);
	setTextCursor(0,0);
	setTextParameter1(RA8876_SELECT_INTERNAL_CGROM,RA8876_CHAR_HEIGHT_24,RA8876_SELECT_8859_1);
	setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE,RA8876_TEXT_CHROMA_KEY_ENABLE,RA8876_TEXT_WIDTH_ENLARGEMENT_X2,RA8876_TEXT_HEIGHT_ENLARGEMENT_X2);

	HAL_GPIO_WritePin(GPIOA, RST_Pin, GPIO_PIN_SET);
}


void xTFT_Com_Data(uint16_t command,uint16_t data)
{
	lcdRegWrite(command);
	lcdDataWrite(data);
}

//**************************************************************//
//**************************************************************//
void lcdRegWrite(uint8_t command)
{
	HAL_StatusTypeDef ret;

	ret = HAL_I2C_Master_Transmit(&hi2c1, RA_CMD, &command, 1, HAL_MAX_DELAY);

		    if ( ret != HAL_OK )
				printf("\rRegister Transmit ERROR\n");
//		    else
//		    	printf("\rRegister Write = 0x%X\n", command);
//	HAL_Delay(10);
}
//**************************************************************//
//**************************************************************//
void lcdDataWrite(uint8_t data)
{
	HAL_StatusTypeDef ret;
//	uint8_t buf[16];
//
//	buf[0] = data>>4;
//	buf[1] = data;
//	printf("\rbuf = 0x%X , 0x%X\n", buf[0],buf[1]);
//	printf("\rData Write = %d\n", data);
	ret = HAL_I2C_Master_Transmit(&hi2c1, RA_Data_Write, &data, 1, HAL_MAX_DELAY);

	    if ( ret != HAL_OK )
			printf("\rData Transmit ERROR\n");
//	    else
//	    	printf("\rData Write = 0x%X\n", buf[0]);
//	HAL_Delay(10);
}
//**************************************************************//
//**************************************************************//
uint8_t lcdDataRead(void)
{
	HAL_StatusTypeDef ret;
	uint8_t buf[16];
	uint16_t dat = 0;
//	printf("\rData Read\n");
	ret = HAL_I2C_Master_Transmit(&hi2c1, RA_Data_Read, 0, 1, HAL_MAX_DELAY);
	 if ( ret != HAL_OK )
		 printf("\rData Read ERROR\n");
	 else{
		HAL_I2C_Master_Receive(&hi2c1,RA_Data_Read,buf,strlen((char*)buf),HAL_MAX_DELAY);
		dat = buf[0];
//		printf("\n\rData Read :%d = 0x%X\n", dat,buf[0]);
	 }
	HAL_Delay(10);
	return dat;
}
////**************************************************************//
////**************************************************************//
uint8_t lcdStatusRead(void)
{
	HAL_StatusTypeDef ret;
	uint8_t buf[16];
	uint8_t dat = 0x00;
//	printf("\rData Read\n");
		ret = HAL_I2C_Master_Transmit(&hi2c1, RA_Status, 0, 1, HAL_MAX_DELAY);
		 if ( ret != HAL_OK )
			 printf("\rData Read ERROR\n");
		 else{
			HAL_I2C_Master_Receive(&hi2c1,RA_Status,buf,strlen((char*)buf),HAL_MAX_DELAY);
			dat = buf[0];
//			printf("\n\rStatus Read : %d = 0x%X\n", dat,buf[0]);
		 }
	HAL_Delay(10);
	return dat;
}
//**************************************************************//
//**************************************************************//
void lcdRegDataWrite(uint8_t reg,uint8_t data)
{
  lcdRegWrite(reg);
  lcdDataWrite(data);
}
//**************************************************************//
//**************************************************************//
uint16_t lcdRegDataRead(uint16_t reg)
{
  lcdRegWrite(reg);
  return lcdDataRead();
}

//**************************************************************//
// support SPI interface to write 16bpp data after Regwrite 04h
//**************************************************************//
void lcdDataWrite16bbp(uint16_t data)
{
//  SPI.transfer(_xnscs,RA8876_SPI_DATAWRITE,SPI_CONTINUE);
//  SPI.transfer(_xnscs,data,SPI_CONTINUE);
//  SPI.transfer(_xnscs,data>>8);
}

//**************************************************************//
//RA8876 register
//**************************************************************//
/*[Status Register] bit7  Host Memory Write FIFO full
0: Memory Write FIFO is not full.
1: Memory Write FIFO is full.
Only when Memory Write FIFO is not full, MPU may write another one pixel.*/
//**************************************************************//
void checkWriteFifoNotFull(void)
{  uint16_t i;
   for(i=0;i<10;i++) //Please according to your usage to modify i value.
   {
    if( (lcdStatusRead()&0x80)==0 ){break;}
   }
}

//**************************************************************//
/*[Status Register] bit6  Host Memory Write FIFO empty
0: Memory Write FIFO is not empty.
1: Memory Write FIFO is empty.
When Memory Write FIFO is empty, MPU may write 8bpp data 64
pixels, or 16bpp data 32 pixels, 24bpp data 16 pixels directly.*/
//**************************************************************//
void checkWriteFifoEmpty(void)
{ uint16_t i;
   for(i=0;i<10;i++)   //Please according to your usage to modify i value.
   {
    if( (lcdStatusRead()&0x40)==0x40 ){break;}
   }
}

//**************************************************************//
/*[Status Register] bit5  Host Memory Read FIFO full
0: Memory Read FIFO is not full.
1: Memory Read FIFO is full.
When Memory Read FIFO is full, MPU may read 8bpp data 32
pixels, or 16bpp data 16 pixels, 24bpp data 8 pixels directly.*/
//**************************************************************//
void checkReadFifoNotFull(void)
{ uint16_t i;
  for(i=0;i<10;i++)  //Please according to your usage to modify i value.
  {if( (lcdStatusRead()&0x20)==0x00 ){break;}}
}

//**************************************************************//
/*[Status Register] bit4   Host Memory Read FIFO empty
0: Memory Read FIFO is not empty.
1: Memory Read FIFO is empty.*/
//**************************************************************//
void checkReadFifoNotEmpty(void)
{ uint16_t i;
  for(i=0;i<10;i++)// //Please according to your usage to modify i value.
  {if( (lcdStatusRead()&0x10)==0x00 ){break;}}
}

//**************************************************************//
/*[Status Register] bit3   Core task is busy
Following task is running:
BTE, Geometry engine, Serial flash DMA, Text write or Graphic write
0: task is done or idle.   1: task is busy*/
//**************************************************************//
void check2dBusy(void)
{  uint16_t i;
   for(i=0;i<10;i++)   //Please according to your usage to modify i value.
   {
    HAL_Delay(1);
    if( (lcdStatusRead()&0x08)==0x00 )
    {break;}
   }
}

//**************************************************************//
/*[Status Register] bit2   SDRAM ready for access
0: SDRAM is not ready for access   1: SDRAM is ready for access*/
//**************************************************************//
void checkSdramReady(void){
	uint8_t temp;

	do{
		temp = (lcdStatusRead() & 0x08);
	} while(temp);
}

//**************************************************************//
/*[Status Register] bit1  Operation mode status
0: Normal operation state  1: Inhibit operation state
Inhibit operation state means internal reset event keep running or
initial display still running or chip enter power saving state.	*/
//**************************************************************//
void checkIcReady(void){
	uint8_t temp;

	do{
		temp = (lcdStatusRead() & 0x02);
	} while(temp);
}
//**************************************************************//
//**************************************************************//
//[05h] [06h] [07h] [08h] [09h] [0Ah]
//------------------------------------//----------------------------------*/
void ra8876PllInitial(void)
{
/*(1) 10MHz <= OSC_FREQ <= 15MHz
  (2) 10MHz <= (OSC_FREQ/PLLDIVM) <= 40MHz
  (3) 250MHz <= [OSC_FREQ/(PLLDIVM+1)]x(PLLDIVN+1) <= 600MHz
PLLDIVM:0
PLLDIVN:1~63
PLLDIVK:CPLL & MPLL = 1/2/4/8.SPLL = 1/2/4/8/16/32/64/128.
ex:
 OSC_FREQ = 10MHz
 Set X_DIVK=2
 Set X_DIVM=0
 => (X_DIVN+1)=(XPLLx4)/10*/
//uint16_t x_Divide,PLLC1,PLLC2;
//uint16_t pll_m_lo, pll_m_hi;
//uint16_t temp;

	// Set tft output pixel clock
		if(SCAN_FREQ>=79)								//&&(SCAN_FREQ<=100))
		{
			lcdRegDataWrite(0x05,0x04);				//PLL Divided by 4
			lcdRegDataWrite(0x06,(SCAN_FREQ*4/OSC_FREQ)-1);
		}
		else if((SCAN_FREQ>=63)&&(SCAN_FREQ<=78))
		{
			lcdRegDataWrite(0x05,0x05);				//PLL Divided by 4
			lcdRegDataWrite(0x06,(SCAN_FREQ*8/OSC_FREQ)-1);
		}
		else if((SCAN_FREQ>=40)&&(SCAN_FREQ<=62))
		{
			lcdRegDataWrite(0x05,0x06);				//PLL Divided by 8
			lcdRegDataWrite(0x06,(SCAN_FREQ*8/OSC_FREQ)-1);
		}
		else if((SCAN_FREQ>=32)&&(SCAN_FREQ<=39))
		{
			lcdRegDataWrite(0x05,0x07);				//PLL Divided by 8
			lcdRegDataWrite(0x06,(SCAN_FREQ*16/OSC_FREQ)-1);
		}
		else if((SCAN_FREQ>=16)&&(SCAN_FREQ<=31))
		{
			lcdRegDataWrite(0x05,0x16);				//PLL Divided by 16
			lcdRegDataWrite(0x06,(SCAN_FREQ*16/OSC_FREQ)-1);
		}
		else if((SCAN_FREQ>=8)&&(SCAN_FREQ<=15))
		{
			lcdRegDataWrite(0x05,0x26);				//PLL Divided by 32
			lcdRegDataWrite(0x06,(SCAN_FREQ*32/OSC_FREQ)-1);
		}
		else if((SCAN_FREQ>0)&&(SCAN_FREQ<=7))
		{
			lcdRegDataWrite(0x05,0x36);				//PLL Divided by 64
			lcdRegDataWrite(0x06,(SCAN_FREQ*64/OSC_FREQ)-1);
		}

		HAL_Delay(1);
		// Set internal Buffer Ram clock
		if(DRAM_FREQ>=158)							//
		{
			lcdRegDataWrite(0x07,0x02);				//PLL Divided by 4
			lcdRegDataWrite(0x08,(DRAM_FREQ*2/OSC_FREQ)-1);
		}
		else if((DRAM_FREQ>=125)&&(DRAM_FREQ<=157))
		{
			lcdRegDataWrite(0x07,0x03);				//PLL Divided by 4
			lcdRegDataWrite(0x08,(DRAM_FREQ*4/OSC_FREQ)-1);
		}
		else if((DRAM_FREQ>=79)&&(DRAM_FREQ<=124))
		{
			lcdRegDataWrite(0x07,0x04);				//PLL Divided by 4
			lcdRegDataWrite(0x08,(DRAM_FREQ*4/OSC_FREQ)-1);
		}
		else if((DRAM_FREQ>=63)&&(DRAM_FREQ<=78))
		{
			lcdRegDataWrite(0x07,0x05);				//PLL Divided by 4
			lcdRegDataWrite(0x08,(DRAM_FREQ*8/OSC_FREQ)-1);
		}
		else if((DRAM_FREQ>=40)&&(DRAM_FREQ<=62))
		{
			lcdRegDataWrite(0x07,0x06);				//PLL Divided by 8
			lcdRegDataWrite(0x08,(DRAM_FREQ*8/OSC_FREQ)-1);
		}
		else if((DRAM_FREQ>=32)&&(DRAM_FREQ<=39))
		{
			lcdRegDataWrite(0x07,0x07);				//PLL Divided by 16
			lcdRegDataWrite(0x08,(DRAM_FREQ*16/OSC_FREQ)-1);
		}
		else if(DRAM_FREQ<=31)
		{
			lcdRegDataWrite(0x07,0x06);				//PLL Divided by 8
			lcdRegDataWrite(0x08,(30*8/OSC_FREQ)-1);	//set to 30MHz if out off range
		}

		HAL_Delay(1);
		// Set Core clock
		if(CORE_FREQ>=158)
		{
			lcdRegDataWrite(0x09,0x02);				//PLL Divided by 2
			lcdRegDataWrite(0x0A,(CORE_FREQ*2/OSC_FREQ)-1);
		}
		else if((CORE_FREQ>=125)&&(CORE_FREQ<=157))
		{
			lcdRegDataWrite(0x09,0x03);				//PLL Divided by 4
			lcdRegDataWrite(0x0A,(CORE_FREQ*4/OSC_FREQ)-1);
		}
		else if((CORE_FREQ>=79)&&(CORE_FREQ<=124))
		{
			lcdRegDataWrite(0x09,0x04);				//PLL Divided by 4
			lcdRegDataWrite(0x0A,(CORE_FREQ*4/OSC_FREQ)-1);
		}
		else if((CORE_FREQ>=63)&&(CORE_FREQ<=78))
		{
			lcdRegDataWrite(0x09,0x05);				//PLL Divided by 8
			lcdRegDataWrite(0x0A,(CORE_FREQ*8/OSC_FREQ)-1);
		}
		else if((CORE_FREQ>=40)&&(CORE_FREQ<=62))
		{
			lcdRegDataWrite(0x09,0x06);				//PLL Divided by 8
			lcdRegDataWrite(0x0A,(CORE_FREQ*8/OSC_FREQ)-1);
		}
		else if((CORE_FREQ>=32)&&(CORE_FREQ<=39))
		{
			lcdRegDataWrite(0x09,0x06);				//PLL Divided by 8
			lcdRegDataWrite(0x0A,(CORE_FREQ*8/OSC_FREQ)-1);
		}
		else if(CORE_FREQ<=31)
		{
			lcdRegDataWrite(0x09,0x06);				//PLL Divided by 8
			lcdRegDataWrite(0x0A,(30*8/OSC_FREQ)-1);	//set to 30MHz if out off range
		}

		lcdRegDataWrite(0x01,0x08);
	    HAL_Delay(100);                 //wait for pll stable
}

//**************************************************************//
void ra8876SdramInitial(void)
{
//uint16_t	CAS_Latency;
//uint16_t	Auto_Refresh;

    unsigned char CAS_Latency;
    unsigned short Auto_Refresh;
//    uint16_t temp;

    if(DRAM_FREQ<=133)
        CAS_Latency=2;
    else
        CAS_Latency=3;

    Auto_Refresh = (64*DRAM_FREQ*1000)/(4096);
    Auto_Refresh = Auto_Refresh-2;              // Start [refresh] in advance to avoid just reaching the limits.

    lcdRegDataWrite(0xE0,0x28);
    lcdRegDataWrite(0xE1,CAS_Latency);
    lcdRegDataWrite(0xE2,Auto_Refresh);
    lcdRegDataWrite(0xE3,Auto_Refresh>>8);
    lcdRegDataWrite(0xE4,0x01);

    HAL_Delay(100);                 //wait for pll stable
	checkSdramReady();
}
 //**************************************************************//
//**************************************************************//
 void displayOn(void)
 {
//  if(on)
   lcdRegDataWrite(RA8876_DPCR, XPCLK_INV<<7|RA8876_DISPLAY_ON<<6|RA8876_OUTPUT_RGB);
//  else
//   lcdRegDataWrite(RA8876_DPCR, XPCLK_INV<<7|RA8876_DISPLAY_OFF<<6|RA8876_OUTPUT_RGB);
//
//  xDelay(20);
 }
//**************************************************************//
//**************************************************************//
void lcdHorizontalWidthVerticalHeight(uint16_t width,uint16_t height)
{uint16_t temp;
   temp=(width/8)-1;
   lcdRegDataWrite(RA8876_HDWR,temp);
   temp=width%8;
   lcdRegDataWrite(RA8876_HDWFTR,temp);
   temp=height-1;
   lcdRegDataWrite(RA8876_VDHR0,temp);
   temp=(height-1)>>8;
   lcdRegDataWrite(RA8876_VDHR1,temp);
}
//**************************************************************//
//**************************************************************//
void lcdHorizontalNonDisplay(uint16_t numbers)
{uint16_t temp;
 if(numbers<8)
  {
   lcdRegDataWrite(RA8876_HNDR,0x00);
   lcdRegDataWrite(RA8876_HNDFTR,numbers);
  }
 else
  {
  temp=(numbers/8)-1;
  lcdRegDataWrite(RA8876_HNDR,temp);
  temp=numbers%8;
  lcdRegDataWrite(RA8876_HNDFTR,temp);
  }
}
//**************************************************************//
//**************************************************************//
void lcdHsyncStartPosition(uint16_t numbers)
{uint16_t temp;
 if(numbers<8)
  {
   lcdRegDataWrite(RA8876_HSTR,0x00);
  }
  else
  {
   temp=(numbers/8)-1;
   lcdRegDataWrite(RA8876_HSTR,temp);
  }
}
//**************************************************************//
//**************************************************************//
void lcdHsyncPulseWidth(uint16_t numbers)
{uint16_t temp;
 if(numbers<8)
  {
   lcdRegDataWrite(RA8876_HPWR,0x00);
  }
  else
  {
   temp=(numbers/8)-1;
   lcdRegDataWrite(RA8876_HPWR,temp);
  }
}
//**************************************************************//
//**************************************************************//
void lcdVerticalNonDisplay(uint16_t numbers)
{uint16_t temp;
  temp=numbers-1;
  lcdRegDataWrite(RA8876_VNDR0,temp);
  lcdRegDataWrite(RA8876_VNDR1,temp>>8);
}
//**************************************************************//
//**************************************************************//
void lcdVsyncStartPosition(uint16_t numbers)
{uint16_t temp;
  temp=numbers-1;
  lcdRegDataWrite(RA8876_VSTR,temp);
}
//**************************************************************//
//**************************************************************//
void lcdVsyncPulseWidth(uint16_t numbers)
{uint16_t temp;
  temp=numbers-1;
  lcdRegDataWrite(RA8876_VPWR,temp);
}
//**************************************************************//
//**************************************************************//
void displayImageStartAddr(uint16_t addr)
{
  lcdRegDataWrite(RA8876_MISA0,addr);//20h
  lcdRegDataWrite(RA8876_MISA1,addr>>8);//21h
  lcdRegDataWrite(RA8876_MISA2,addr>>16);//22h
  lcdRegDataWrite(RA8876_MISA3,addr>>24);//23h
}
//**************************************************************//
//**************************************************************//
void displayImageWidth(uint16_t width)
{
  lcdRegDataWrite(RA8876_MIW0,width); //24h
  lcdRegDataWrite(RA8876_MIW1,width>>8); //25h
}
//**************************************************************//
//**************************************************************//
void displayWindowStartXY(uint16_t x0,uint16_t y0)
{
  lcdRegDataWrite(RA8876_MWULX0,x0);//26h
  lcdRegDataWrite(RA8876_MWULX1,x0>>8);//27h
  lcdRegDataWrite(RA8876_MWULY0,y0);//28h
  lcdRegDataWrite(RA8876_MWULY1,y0>>8);//29h
}
//**************************************************************//
//**************************************************************//
void canvasImageStartAddr(uint16_t addr)
{
  lcdRegDataWrite(RA8876_CVSSA0,addr);//50h
  lcdRegDataWrite(RA8876_CVSSA1,addr>>8);//51h
  lcdRegDataWrite(RA8876_CVSSA2,addr>>16);//52h
  lcdRegDataWrite(RA8876_CVSSA3,addr>>24);//53h
}
//**************************************************************//
//**************************************************************//
void canvasImageWidth(uint16_t width)
{
  lcdRegDataWrite(RA8876_CVS_IMWTH0,width);//54h
  lcdRegDataWrite(RA8876_CVS_IMWTH1,width>>8); //55h
}
//**************************************************************//
//**************************************************************//
void activeWindowXY(uint16_t x0,uint16_t y0)
{
  lcdRegDataWrite(RA8876_AWUL_X0,x0);//56h
  lcdRegDataWrite(RA8876_AWUL_X1,x0>>8);//57h
  lcdRegDataWrite(RA8876_AWUL_Y0,y0);//58h
  lcdRegDataWrite(RA8876_AWUL_Y1,y0>>8);//59h
}
//**************************************************************//
//**************************************************************//
void activeWindowWH(uint16_t width,uint16_t height)
{
  lcdRegDataWrite(RA8876_AW_WTH0,width);//5ah
  lcdRegDataWrite(RA8876_AW_WTH1,width>>8);//5bh
  lcdRegDataWrite(RA8876_AW_HT0,height);//5ch
  lcdRegDataWrite(RA8876_AW_HT1,height>>8);//5dh
}
//**************************************************************//
//**************************************************************//
void  setPixelCursor(uint16_t x,uint16_t y)
{
  lcdRegDataWrite(RA8876_CURH0,x); //5fh
  lcdRegDataWrite(RA8876_CURH1,x>>8);//60h
  lcdRegDataWrite(RA8876_CURV0,y);//61h
  lcdRegDataWrite(RA8876_CURV1,y>>8);//62h
}
//**************************************************************//
//**************************************************************//
void bte_Source0_MemoryStartAddr(uint16_t addr)
{
  lcdRegDataWrite(RA8876_S0_STR0,addr);//93h
  lcdRegDataWrite(RA8876_S0_STR1,addr>>8);//94h
  lcdRegDataWrite(RA8876_S0_STR2,addr>>16);//95h
  lcdRegDataWrite(RA8876_S0_STR3,addr>>24);////96h
}
//**************************************************************//
//**************************************************************//
void bte_Source0_ImageWidth(uint16_t width)
{
  lcdRegDataWrite(RA8876_S0_WTH0,width);//97h
  lcdRegDataWrite(RA8876_S0_WTH1,width>>8);//98h
}
//**************************************************************//
//**************************************************************//
void bte_Source0_WindowStartXY(uint16_t x0,uint16_t y0)
{
  lcdRegDataWrite(RA8876_S0_X0,x0);//99h
  lcdRegDataWrite(RA8876_S0_X1,x0>>8);//9ah
  lcdRegDataWrite(RA8876_S0_Y0,y0);//9bh
  lcdRegDataWrite(RA8876_S0_Y1,y0>>8);//9ch
}
//**************************************************************//
//**************************************************************//
void bte_Source1_MemoryStartAddr(uint16_t addr)
{
  lcdRegDataWrite(RA8876_S1_STR0,addr);//9dh
  lcdRegDataWrite(RA8876_S1_STR1,addr>>8);//9eh
  lcdRegDataWrite(RA8876_S1_STR2,addr>>16);//9fh
  lcdRegDataWrite(RA8876_S1_STR3,addr>>24);//a0h
}
//**************************************************************//
//**************************************************************//
void bte_Source1_ImageWidth(uint16_t width)
{
  lcdRegDataWrite(RA8876_S1_WTH0,width);//a1h
  lcdRegDataWrite(RA8876_S1_WTH1,width>>8);//a2h
}
//**************************************************************//
//**************************************************************//
void bte_Source1_WindowStartXY(uint16_t x0,uint16_t y0)
{
  lcdRegDataWrite(RA8876_S1_X0,x0);//a3h
  lcdRegDataWrite(RA8876_S1_X1,x0>>8);//a4h
  lcdRegDataWrite(RA8876_S1_Y0,y0);//a5h
  lcdRegDataWrite(RA8876_S1_Y1,y0>>8);//a6h
}
//**************************************************************//
//**************************************************************//
void  bte_DestinationMemoryStartAddr(uint16_t addr)
{
  lcdRegDataWrite(RA8876_DT_STR0,addr);//a7h
  lcdRegDataWrite(RA8876_DT_STR1,addr>>8);//a8h
  lcdRegDataWrite(RA8876_DT_STR2,addr>>16);//a9h
  lcdRegDataWrite(RA8876_DT_STR3,addr>>24);//aah
}
//**************************************************************//
//**************************************************************//
void  bte_DestinationImageWidth(uint16_t width)
{
  lcdRegDataWrite(RA8876_DT_WTH0,width);//abh
  lcdRegDataWrite(RA8876_DT_WTH1,width>>8);//ach
}
//**************************************************************//
//**************************************************************//
void  bte_DestinationWindowStartXY(uint16_t x0,uint16_t y0)
{
  lcdRegDataWrite(RA8876_DT_X0,x0);//adh
  lcdRegDataWrite(RA8876_DT_X1,x0>>8);//aeh
  lcdRegDataWrite(RA8876_DT_Y0,y0);//afh
  lcdRegDataWrite(RA8876_DT_Y1,y0>>8);//b0h
}
//**************************************************************//
//**************************************************************//
void  bte_WindowSize(uint16_t width, uint16_t height)
{
  lcdRegDataWrite(RA8876_BTE_WTH0,width);//b1h
  lcdRegDataWrite(RA8876_BTE_WTH1,width>>8);//b2h
  lcdRegDataWrite(RA8876_BTE_HIG0,height);//b3h
  lcdRegDataWrite(RA8876_BTE_HIG1,height>>8);//b4h
}

//**************************************************************//
/*These 8 bits determine prescaler value for Timer 0 and 1.*/
/*Time base is ?Core_Freq / (Prescaler + 1)?*/
//**************************************************************//
void pwm_Prescaler(uint16_t prescaler)
{
  lcdRegDataWrite(RA8876_PSCLR,prescaler);//84h
}
//**************************************************************//
//**************************************************************//
void pwm_ClockMuxReg(uint16_t pwm1_clk_div, uint16_t pwm0_clk_div, uint16_t xpwm1_ctrl, uint16_t xpwm0_ctrl)
{
  lcdRegDataWrite(RA8876_PMUXR,pwm1_clk_div<<6|pwm0_clk_div<<4|xpwm1_ctrl<<2|xpwm0_ctrl);//85h
}
//**************************************************************//
//**************************************************************//
void pwm_Configuration(uint16_t pwm1_inverter,uint16_t pwm1_auto_reload,uint16_t pwm1_start,uint16_t
                       pwm0_dead_zone, uint16_t pwm0_inverter, uint16_t pwm0_auto_reload,uint16_t pwm0_start)
 {
  lcdRegDataWrite(RA8876_PCFGR,pwm1_inverter<<6|pwm1_auto_reload<<5|pwm1_start<<4|pwm0_dead_zone<<3|
                  pwm0_inverter<<2|pwm0_auto_reload<<1|pwm0_start);//86h
 }
//**************************************************************//
//**************************************************************//
void pwm0_Duty(uint16_t duty)
{
  lcdRegDataWrite(RA8876_TCMPB0L,duty);//88h
  lcdRegDataWrite(RA8876_TCMPB0H,duty>>8);//89h
}
//**************************************************************//
//**************************************************************//
void pwm0_ClocksPerPeriod(uint16_t clocks_per_period)
{
  lcdRegDataWrite(RA8876_TCNTB0L,clocks_per_period);//8ah
  lcdRegDataWrite(RA8876_TCNTB0H,clocks_per_period>>8);//8bh
}
//**************************************************************//
//**************************************************************//
void pwm1_Duty(uint16_t duty)
{
  lcdRegDataWrite(RA8876_TCMPB1L,duty);//8ch
  lcdRegDataWrite(RA8876_TCMPB1H,duty>>8);//8dh
}
//**************************************************************//
//**************************************************************//
void pwm1_ClocksPerPeriod(uint16_t clocks_per_period)
{
  lcdRegDataWrite(RA8876_TCNTB1L,clocks_per_period);//8eh
  lcdRegDataWrite(RA8876_TCNTB1F,clocks_per_period>>8);//8fh
}
//**************************************************************//
//**************************************************************//
void  ramAccessPrepare(void)
{
  lcdRegWrite(RA8876_MRWDP); //04h
}
//**************************************************************//
//**************************************************************//
void foreGroundColor(uint32_t color)
{
  lcdRegDataWrite(RA8876_FGCR,color>>16);//d2h
  lcdRegDataWrite(RA8876_FGCG,color>>8);//d3h
  lcdRegDataWrite(RA8876_FGCB,color);//d4h
}
//**************************************************************//
//**************************************************************//
void backGroundColor(uint32_t color)
{
  lcdRegDataWrite(RA8876_BGCR,color>>16);//d5h
  lcdRegDataWrite(RA8876_BGCG,color>>8);//d6h
  lcdRegDataWrite(RA8876_BGCB,color);//d7h
}

 /*graphic function*/
//**************************************************************//
/* If use ra8876 graphic mode function turns on */
//**************************************************************//
 void graphicMode(void)
 {
   lcdRegDataWrite(RA8876_ICR,RA8876_LVDS_FORMAT<<3|RA8876_GRAPHIC_MODE<<2|RA8876_MEMORY_SELECT_IMAGE);//03h  //switch to graphic mode
 }
//**************************************************************//


/*support serial IF to write 16bpp pixel*/
//**************************************************************//
//void  putPixel_16bpp(uint16_t x,uint16_t y,uint16_t color)
//{
// setPixelCursor(x,y);
// ramAccessPrepare();
// //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
// //lcdDataWrite(color);
// //lcdDataWrite(color>>8);
// lcdDataWrite16bbp(color);
// //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
//}
////**************************************************************//
///*support ra8876 serial IF to write 16bpp(RGB565) picture data for user operation */
////**************************************************************//
//void  putPicture_16bpp(uint16_t x,uint16_t y,uint16_t width, uint16_t height)
//{uint16_t i,j;
// activeWindowXY(x,y);
// activeWindowWH(width,height);
// setPixelCursor(x,y);
// ramAccessPrepare();
//}
////**************************************************************//
///*support serial IF to write 16bpp(RGB565) picture data byte format from data pointer*/
////**************************************************************//
//void  putPicture_16bpp(uint16_t x,uint16_t y,uint16_t width, uint16_t height, const unsigned char *data)
//{uint16_t i,j;
//
// activeWindowXY(x,y);
// activeWindowWH(width,height);
// setPixelCursor(x,y);
// ramAccessPrepare();
// for(j=0;j<height;j++)
// {
//  for(i=0;i<width;i++)
//  {
//   //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
//   lcdDataWrite(*data);
//   data++;
//   //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
//   lcdDataWrite(*data);
//   data++;
//  }
// }
// checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
// activeWindowXY(0,0);
// activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT);
//}
////**************************************************************//
///*support serial IF to write 16bpp(RGB565) picture data word format from data pointer*/
////**************************************************************//
//void  putPicture_16bpp(uint16_t x,uint16_t y,uint16_t width, uint16_t height, const unsigned short *data)
//{uint16_t i,j;
// activeWindowXY(x,y);
// activeWindowWH(width,height);
// setPixelCursor(x,y);
// ramAccessPrepare();
// for(j=0;j<height;j++)
// {
//  for(i=0;i<width;i++)
//  {
//   //checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
//   lcdDataWrite16bbp(*data);
//   data++;
//   //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
//  }
// }
// checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
// activeWindowXY(0,0);
// activeWindowWH(SCREEN_WIDTH,SCREEN_HEIGHT);
//}


/*text function*/

//**************************************************************//
/* If use ra8876 text mode function turns on */
//**************************************************************//
void textMode(void)
 {
	lcdRegDataWrite(RA8876_ICR,RA8876_LVDS_FORMAT<<3|RA8876_TEXT_MODE<<2|RA8876_MEMORY_SELECT_IMAGE);//03h  //switch to text mode
	textColor(COLOR16M_BLACK, COLOR16M_WHITE);

 }
//**************************************************************//
//**************************************************************//
void textColor(uint32_t foreground_color,uint32_t background_color)
 {
   foreGroundColor(foreground_color);
   backGroundColor(background_color);
 }
//**************************************************************//
//**************************************************************//
void  setTextCursor(uint16_t x,uint16_t y)
{
  lcdRegDataWrite(RA8876_F_CURX0,x); //63h
  lcdRegDataWrite(RA8876_F_CURX1,x>>8);//64h
  lcdRegDataWrite(RA8876_F_CURY0,y);//65h
  lcdRegDataWrite(RA8876_F_CURY1,y>>8);//66h
}
//**************************************************************//
/* source_select = 0 : internal CGROM,  source_select = 1: external CGROM, source_select = 2: user-define*/
/* size_select = 0 : 8*16/16*16, size_select = 1 : 12*24/24*24, size_select = 2 : 16*32/32*32  */
/* iso_select = 0 : iso8859-1, iso_select = 1 : iso8859-2, iso_select = 2 : iso8859-4, iso_select = 3 : iso8859-5*/
//**************************************************************//
void setTextParameter1(uint16_t source_select,uint16_t size_select,uint16_t iso_select)//cch
{
	lcdRegDataWrite(RA8876_CCR0,source_select<<6|size_select<<4|iso_select);//cch
}
//**************************************************************//
/*align = 0 : full alignment disable, align = 1 : full alignment enable*/
/*chroma_key = 0 : text with chroma key disable, chroma_key = 1 : text with chroma key enable*/
/* width_enlarge and height_enlarge can be set 0~3, (00b: X1) (01b : X2)  (10b : X3)  (11b : X4)*/
//**************************************************************//
void setTextParameter2(uint16_t align, uint16_t chroma_key, uint16_t width_enlarge, uint16_t height_enlarge)
{
	lcdRegDataWrite(RA8876_CCR1,align<<7|chroma_key<<6|width_enlarge<<2|height_enlarge);//cdh
}
//**************************************************************//


////**************************************************************//
//void genitopCharacterRomParameter(uint16_t scs_select, uint16_t clk_div, uint16_t rom_select, uint16_t character_select, uint16_t gt_width)
//{
//  if(scs_select==0)
//  lcdRegDataWrite(RA8876_SFL_CTRL,RA8876_SERIAL_FLASH_SELECT0<<7|RA8876_SERIAL_FLASH_FONT_MODE<<6|RA8876_SERIAL_FLASH_ADDR_24BIT<<5|RA8876_FOLLOW_RA8876_MODE<<4|RA8876_SPI_FAST_READ_8DUMMY);//b7h
//  if(scs_select==1)
//  lcdRegDataWrite(RA8876_SFL_CTRL,RA8876_SERIAL_FLASH_SELECT1<<7|RA8876_SERIAL_FLASH_FONT_MODE<<6|RA8876_SERIAL_FLASH_ADDR_24BIT<<5|RA8876_FOLLOW_RA8876_MODE<<4|RA8876_SPI_FAST_READ_8DUMMY);//b7h
//
//  lcdRegDataWrite(RA8876_SPI_DIVSOR,clk_div);//bbh
//
//  lcdRegDataWrite(RA8876_GTFNT_SEL,rom_select<<5);//ceh
//  lcdRegDataWrite(RA8876_GTFNT_CR,character_select<<3|gt_width);//cfh
//}
////**************************************************************//
////support ra8876 internal font and external string font code write from data pointer
////**************************************************************//
void  putString(uint16_t x0,uint16_t y0, char *str)
{
	  textMode();
	  setTextCursor(x0,y0);
	  ramAccessPrepare();
	  while(*str != '\0')
	  {
		  lcdDataWrite(*str);
		  ++str;
//		  checkWriteFifoNotFull();
	  }
//	  check2dBusy();
	  graphicMode();
}

void  putStringData(char *str)
{
	textMode();
	ramAccessPrepare();
    while(*str != '\0')
    {
    	lcdDataWrite(*str);
        ++str;
    }
//	check2dBusy();
    graphicMode();
}

///*put value,base on sprintf*/
////**************************************************************//
////vaule: -2147483648(-2^31) ~ 2147483647(2^31-1)
////len: 1~11 minimum output length
///*
//  [flag]
//    n:???
//    -:??
//    +:?????
//    (space):??????????????
//    0:????(??) ? 0???????
// */
////**************************************************************//
//void  putDec(uint16_t x0,uint16_t y0,rs32 vaule,uint16_t len,const char *flag)
//{
//  char char_buffer[12];
//  switch(len)
//  {
//    case 1:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%1d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="-")
//           {sprintf(char_buffer ,"%-1d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="+")
//           {sprintf(char_buffer ,"%+1d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%01d", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 2:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%2d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="-")
//           { sprintf(char_buffer ,"%-2d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="+")
//           { sprintf(char_buffer ,"%+2d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           { sprintf(char_buffer ,"%02d", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 3:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%3d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="-")
//           {sprintf(char_buffer ,"%-3d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="+")
//           {sprintf(char_buffer ,"%+3d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%03d", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 4:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%4d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="-")
//           { sprintf(char_buffer ,"%-4d", vaule);  putString(x0,y0,char_buffer);}
//           else if(flag=="+")
//           { sprintf(char_buffer ,"%+4d", vaule);  putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           { sprintf(char_buffer ,"%04d", vaule);  putString(x0,y0,char_buffer);}
//           break;
//    case 5:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%5d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="-")
//           {sprintf(char_buffer ,"%-5d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="+")
//           {sprintf(char_buffer ,"%+5d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%05d", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 6:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%6d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="-")
//           {sprintf(char_buffer ,"%-6d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="+")
//           {sprintf(char_buffer ,"%+6d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%06d", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 7:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%7d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="-")
//           {sprintf(char_buffer ,"%-7d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="+")
//           {sprintf(char_buffer ,"%+7d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%07d", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 8:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%8d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="-")
//           {sprintf(char_buffer ,"%-8d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="+")
//           {sprintf(char_buffer ,"%+8d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%08d", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 9:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%9d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="-")
//           {sprintf(char_buffer ,"%-9d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="+")
//           {sprintf(char_buffer ,"%+9d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%09d", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 10:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%10d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="-")
//           {sprintf(char_buffer ,"%-10d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="+")
//           {sprintf(char_buffer ,"%+10d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%010d", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 11:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%11d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="-")
//           {sprintf(char_buffer ,"%-11d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="+")
//           {sprintf(char_buffer ,"%+11d", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%011d", vaule); putString(x0,y0,char_buffer);}
//           break;
//    default:
//           break;
//  }
//}
//
////**************************************************************//
////vaule: (3.4E-38) ~ (3.4E38)
////len: 1~11 minimum output length
////precision: right side of point numbers 1~4
///*
//  [flag]
//    n:???
//    -:??
//    +:?????
//    (space):??????????????
//    #: ???????
//    0:????(??) ? 0???????
//
// */
// //arduino Floats have only 6-7 decimal digits of precision. That means the total number of digits, not the number to the right of the decimal point.
// //Unlike other platforms, where you can get more precision by using a double (e.g. up to 15 digits), on the Arduino, double is the same size as float.
////**************************************************************//
//void  putFloat(uint16_t x0,uint16_t y0,double vaule,uint16_t len,uint16_t precision, const char *flag)
//{
//  char char_buffer[20];
//  switch(len)
//  {
//    case 1:
//           if(flag=="n")
//           {
//             if(precision==1)
//             sprintf(char_buffer ,"%1.1f", vaule); putString(x0,y0,char_buffer);
//             if(precision==2)
//             sprintf(char_buffer ,"%1.2f", vaule); putString(x0,y0,char_buffer);
//             if(precision==3)
//             sprintf(char_buffer ,"%1.3f", vaule); putString(x0,y0,char_buffer);
//             if(precision==4)
//             sprintf(char_buffer ,"%1.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="-")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%-1.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%-1.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%-1.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%-1.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="+")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%+1.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%+1.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%+1.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%+1.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="0")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%01.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%01.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%01.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%01.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           break;
//    case 2:
//           if(flag=="n")
//           {
//           if(precision==1)
//           sprintf(char_buffer ,"%2.1f", vaule); putString(x0,y0,char_buffer);
//           if(precision==2)
//           sprintf(char_buffer ,"%2.2f", vaule); putString(x0,y0,char_buffer);
//           if(precision==3)
//           sprintf(char_buffer ,"%2.3f", vaule); putString(x0,y0,char_buffer);
//           if(precision==4)
//           sprintf(char_buffer ,"%2.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="-")
//           {
//           if(precision==1)
//           sprintf(char_buffer ,"%-2.1f", vaule); putString(x0,y0,char_buffer);
//           if(precision==2)
//           sprintf(char_buffer ,"%-2.2f", vaule); putString(x0,y0,char_buffer);
//           if(precision==3)
//           sprintf(char_buffer ,"%-2.3f", vaule); putString(x0,y0,char_buffer);
//           if(precision==4)
//           sprintf(char_buffer ,"%-2.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="+")
//           {
//           if(precision==1)
//           sprintf(char_buffer ,"%+2.1f", vaule); putString(x0,y0,char_buffer);
//           if(precision==2)
//           sprintf(char_buffer ,"%+2.2f", vaule); putString(x0,y0,char_buffer);
//           if(precision==3)
//           sprintf(char_buffer ,"%+2.3f", vaule); putString(x0,y0,char_buffer);
//           if(precision==4)
//           sprintf(char_buffer ,"%+2.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="0")
//           {
//           if(precision==1)
//           sprintf(char_buffer ,"%02.1f", vaule); putString(x0,y0,char_buffer);
//           if(precision==2)
//           sprintf(char_buffer ,"%02.2f", vaule); putString(x0,y0,char_buffer);
//           if(precision==3)
//           sprintf(char_buffer ,"%02.3f", vaule); putString(x0,y0,char_buffer);
//           if(precision==4)
//           sprintf(char_buffer ,"%02.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           break;
//    case 3:
//           if(flag=="n")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%3.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%3.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%3.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%3.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="-")
//           {
//           if(precision==1)
//           sprintf(char_buffer ,"%-3.1f", vaule); putString(x0,y0,char_buffer);
//           if(precision==2)
//           sprintf(char_buffer ,"%-3.2f", vaule); putString(x0,y0,char_buffer);
//           if(precision==3)
//           sprintf(char_buffer ,"%-3.3f", vaule); putString(x0,y0,char_buffer);
//           if(precision==4)
//           sprintf(char_buffer ,"%-3.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="+")
//           {
//           if(precision==1)
//           sprintf(char_buffer ,"%+3.1f", vaule); putString(x0,y0,char_buffer);
//           if(precision==2)
//           sprintf(char_buffer ,"%+3.2f", vaule); putString(x0,y0,char_buffer);
//           if(precision==3)
//           sprintf(char_buffer ,"%+3.3f", vaule); putString(x0,y0,char_buffer);
//           if(precision==4)
//           sprintf(char_buffer ,"%+3.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="0")
//           {
//           if(precision==1)
//           sprintf(char_buffer ,"%03.1f", vaule); putString(x0,y0,char_buffer);
//           if(precision==2)
//           sprintf(char_buffer ,"%03.2f", vaule); putString(x0,y0,char_buffer);
//           if(precision==3)
//           sprintf(char_buffer ,"%03.3f", vaule); putString(x0,y0,char_buffer);
//           if(precision==4)
//           sprintf(char_buffer ,"%03.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           break;
//    case 4:
//           if(flag=="n")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%4.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%4.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%4.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%4.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="-")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%-4.1f", vaule);  putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%-4.2f", vaule);  putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%-4.3f", vaule);  putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%-4.4f", vaule);  putString(x0,y0,char_buffer);
//           }
//           else if(flag=="+")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%+4.1f", vaule);  putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%+4.2f", vaule);  putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%+4.3f", vaule);  putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%+4.4f", vaule);  putString(x0,y0,char_buffer);
//           }
//           else if(flag=="0")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%04.1f", vaule);  putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%04.2f", vaule);  putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%04.3f", vaule);  putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%04.4f", vaule);  putString(x0,y0,char_buffer);
//           }
//           break;
//    case 5:
//           if(flag=="n")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%5.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%5.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%5.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%5.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="-")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%-5.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%-5.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%-5.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%-5.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="+")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%+5.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%+5.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%+5.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%+5.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="0")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%05.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%05.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%05.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%05.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           break;
//    case 6:
//           if(flag=="n")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%6.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%6.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%6.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%6.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="-")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%-6.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%-6.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%-6.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%-6.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="+")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%+6.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%+6.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%+6.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%+6.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="0")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%06.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%06.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%06.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%06.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           break;
//    case 7:
//           if(flag=="n")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%7.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%7.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%7.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%7.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="-")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%-7.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%-7.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%-7.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%-7.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="+")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%+7.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%+7.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%+7.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%+7.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="0")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%07.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%07.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%07.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%07.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           break;
//    case 8:
//           if(flag=="n")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%8.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%8.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%8.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%8.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="-")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%-8.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%-8.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%-8.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%-8.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="+")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%+8.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%+8.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%+8.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%+8.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="0")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%08.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%08.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%08.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%08.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           break;
//    case 9:
//           if(flag=="n")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%9.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%9.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%9.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%9.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="-")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%-9.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%-9.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%-9.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%-9.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="+")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%+9.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%+9.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%+9.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%+9.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="0")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%09.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%09.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%09.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%09.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           break;
//    case 10:
//           if(flag=="n")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%10.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%10.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%10.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%10.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="-")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%-10.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%-10.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%-10.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%-10.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="+")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%+10.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%+10.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%+10.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%+10.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="0")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%010.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%010.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%010.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%010.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           break;
//    case 11:
//           if(flag=="n")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%11.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%11.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%11.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%11.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="-")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%-11.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%-11.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%-11.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%-11.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="+")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%+11.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%+11.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%+11.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%+11.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           else if(flag=="0")
//           {
//            if(precision==1)
//            sprintf(char_buffer ,"%011.1f", vaule); putString(x0,y0,char_buffer);
//            if(precision==2)
//            sprintf(char_buffer ,"%011.2f", vaule); putString(x0,y0,char_buffer);
//            if(precision==3)
//            sprintf(char_buffer ,"%011.3f", vaule); putString(x0,y0,char_buffer);
//            if(precision==4)
//            sprintf(char_buffer ,"%011.4f", vaule); putString(x0,y0,char_buffer);
//           }
//           break;
//    default:
//           break;
//  }
//}
////**************************************************************//
////vaule: 0x00000000 ~ 0xffffffff
////len: 1~11 minimum output length
///*
//  [flag]
//    n:??,????
//    #:???? 0x ????.
//    0:????(??) ? 0???????
//    x:???? 0x ?????? 0?
// */
////**************************************************************//
//void  putHex(uint16_t x0,uint16_t y0,uint16_t vaule,uint16_t len,const char *flag)
//{
//  char char_buffer[12];
//  switch(len)
//  {
//    case 1:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%1x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%01x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="#")
//           {sprintf(char_buffer ,"%#1x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="x")
//           {sprintf(char_buffer ,"%#01x", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 2:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%2x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           { sprintf(char_buffer ,"%02x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="#")
//           { sprintf(char_buffer ,"%#2x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="x")
//           { sprintf(char_buffer ,"%#02x", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 3:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%3x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%03x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="#")
//           {sprintf(char_buffer ,"%#3x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="x")
//           {sprintf(char_buffer ,"%#03x", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 4:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%4x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           { sprintf(char_buffer ,"%04x", vaule);  putString(x0,y0,char_buffer);}
//           else if(flag=="#")
//           { sprintf(char_buffer ,"%#4x", vaule);  putString(x0,y0,char_buffer);}
//           else if(flag=="x")
//           { sprintf(char_buffer ,"%#04x", vaule);  putString(x0,y0,char_buffer);}
//           break;
//    case 5:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%5x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%05x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="#")
//           {sprintf(char_buffer ,"%#5x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="x")
//           {sprintf(char_buffer ,"%#05x", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 6:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%6x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%06x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="#")
//           {sprintf(char_buffer ,"%#6x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="x")
//           {sprintf(char_buffer ,"%#06x", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 7:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%7x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%07x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="#")
//           {sprintf(char_buffer ,"%#7x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="x")
//           {sprintf(char_buffer ,"%#07x", vaule); putString(x0,y0,char_buffer);}
//           break;
//    case 8:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%8x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%08x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="#")
//           {sprintf(char_buffer ,"%#8x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="x")
//           {sprintf(char_buffer ,"%#08x", vaule); putString(x0,y0,char_buffer);}
//           break;
//case 9:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%9x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%09x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="#")
//           {sprintf(char_buffer ,"%#9x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="x")
//           {sprintf(char_buffer ,"%#09x", vaule); putString(x0,y0,char_buffer);}
//           break;
//case 10:
//           if(flag=="n")
//           {sprintf(char_buffer ,"%10x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="0")
//           {sprintf(char_buffer ,"%010x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="#")
//           {sprintf(char_buffer ,"%#10x", vaule); putString(x0,y0,char_buffer);}
//           else if(flag=="x")
//           {sprintf(char_buffer ,"%#010x", vaule); putString(x0,y0,char_buffer);}
//           break;
//
//    default:
//           break;
//  }
//}
// /*draw function*/
//**************************************************************//


//**************************************************************//
//void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
//{
//  foreGroundColor16bpp(color);
//  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
//  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
//  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
//  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh
//  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
//  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
//  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
//  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh
//  lcdRegDataWrite(RA8876_DCR0,RA8876_DRAW_LINE);//67h,0x80
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
//void drawSquare(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
//{
//  foreGroundColor16bpp(color);
//  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
//  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
//  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
//  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh
//  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
//  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
//  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
//  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh
//  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_SQUARE);//76h,0xa0
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
void drawSquareFill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color)
{
  foreGroundColor(color);
  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh
  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh
  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_SQUARE_FILL);//76h,0xa0
  HAL_Delay(10);
//  check2dBusy();
}
////**************************************************************//
////**************************************************************//
//void drawCircleSquare(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t xr, uint16_t yr, uint16_t color)
//{
//  foreGroundColor16bpp(color);
//  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
//  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
//  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
//  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh
//  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
//  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
//  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
//  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh
//  lcdRegDataWrite(RA8876_ELL_A0,xr);//77h
//  lcdRegDataWrite(RA8876_ELL_A1,xr>>8);//79h
//  lcdRegDataWrite(RA8876_ELL_B0,yr);//7ah
//  lcdRegDataWrite(RA8876_ELL_B1,yr>>8);//7bh
//  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_CIRCLE_SQUARE);//76h,0xb0
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
//void drawCircleSquareFill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t xr, uint16_t yr, uint16_t color)
//{
//  foreGroundColor16bpp(color);
//  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
//  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
//  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
//  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh
//  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
//  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
//  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
//  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh
//  lcdRegDataWrite(RA8876_ELL_A0,xr);//77h
//  lcdRegDataWrite(RA8876_ELL_A1,xr>>8);//78h
//  lcdRegDataWrite(RA8876_ELL_B0,yr);//79h
//  lcdRegDataWrite(RA8876_ELL_B1,yr>>8);//7ah
//  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_CIRCLE_SQUARE_FILL);//76h,0xf0
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
//void drawTriangle(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
//{
//  foreGroundColor16bpp(color);
//  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
//  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
//  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
//  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh
//  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
//  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
//  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
//  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh
//  lcdRegDataWrite(RA8876_DTPH0,x2);//70h
//  lcdRegDataWrite(RA8876_DTPH1,x2>>8);//71h
//  lcdRegDataWrite(RA8876_DTPV0,y2);//72h
//  lcdRegDataWrite(RA8876_DTPV1,y2>>8);//73h
//  lcdRegDataWrite(RA8876_DCR0,RA8876_DRAW_TRIANGLE);//67h,0x82
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
//void drawTriangleFill(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
//{
//  foreGroundColor16bpp(color);
//  lcdRegDataWrite(RA8876_DLHSR0,x0);//68h
//  lcdRegDataWrite(RA8876_DLHSR1,x0>>8);//69h
//  lcdRegDataWrite(RA8876_DLVSR0,y0);//6ah
//  lcdRegDataWrite(RA8876_DLVSR1,y0>>8);//6bh
//  lcdRegDataWrite(RA8876_DLHER0,x1);//6ch
//  lcdRegDataWrite(RA8876_DLHER1,x1>>8);//6dh
//  lcdRegDataWrite(RA8876_DLVER0,y1);//6eh
//  lcdRegDataWrite(RA8876_DLVER1,y1>>8);//6fh
//  lcdRegDataWrite(RA8876_DTPH0,x2);//70h
//  lcdRegDataWrite(RA8876_DTPH1,x2>>8);//71h
//  lcdRegDataWrite(RA8876_DTPV0,y2);//72h
//  lcdRegDataWrite(RA8876_DTPV1,y2>>8);//73h
//  lcdRegDataWrite(RA8876_DCR0,RA8876_DRAW_TRIANGLE_FILL);//67h,0xa2
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
//void drawCircle(uint16_t x0,uint16_t y0,uint16_t r,uint16_t color)
//{
//  foreGroundColor16bpp(color);
//  lcdRegDataWrite(RA8876_DEHR0,x0);//7bh
//  lcdRegDataWrite(RA8876_DEHR1,x0>>8);//7ch
//  lcdRegDataWrite(RA8876_DEVR0,y0);//7dh
//  lcdRegDataWrite(RA8876_DEVR1,y0>>8);//7eh
//  lcdRegDataWrite(RA8876_ELL_A0,r);//77h
//  lcdRegDataWrite(RA8876_ELL_A1,r>>8);//78h
//  lcdRegDataWrite(RA8876_ELL_B0,r);//79h
//  lcdRegDataWrite(RA8876_ELL_B1,r>>8);//7ah
//  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_CIRCLE);//76h,0x80
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
//void drawCircleFill(uint16_t x0,uint16_t y0,uint16_t r,uint16_t color)
//{
//  foreGroundColor16bpp(color);
//  lcdRegDataWrite(RA8876_DEHR0,x0);//7bh
//  lcdRegDataWrite(RA8876_DEHR1,x0>>8);//7ch
//  lcdRegDataWrite(RA8876_DEVR0,y0);//7dh
//  lcdRegDataWrite(RA8876_DEVR1,y0>>8);//7eh
//  lcdRegDataWrite(RA8876_ELL_A0,r);//77h
//  lcdRegDataWrite(RA8876_ELL_A1,r>>8);//78h
//  lcdRegDataWrite(RA8876_ELL_B0,r);//79h
//  lcdRegDataWrite(RA8876_ELL_B1,r>>8);//7ah
//  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_CIRCLE_FILL);//76h,0xc0
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
//void drawEllipse(uint16_t x0,uint16_t y0,uint16_t xr,uint16_t yr,uint16_t color)
//{
//   foreGroundColor16bpp(color);
//  lcdRegDataWrite(RA8876_DEHR0,x0);//7bh
//  lcdRegDataWrite(RA8876_DEHR1,x0>>8);//7ch
//  lcdRegDataWrite(RA8876_DEVR0,y0);//7dh
//  lcdRegDataWrite(RA8876_DEVR1,y0>>8);//7eh
//  lcdRegDataWrite(RA8876_ELL_A0,xr);//77h
//  lcdRegDataWrite(RA8876_ELL_A1,xr>>8);//78h
//  lcdRegDataWrite(RA8876_ELL_B0,yr);//79h
//  lcdRegDataWrite(RA8876_ELL_B1,yr>>8);//7ah
//  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_ELLIPSE);//76h,0x80
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
//void drawEllipseFill(uint16_t x0,uint16_t y0,uint16_t xr,uint16_t yr,uint16_t color)
//{
//   foreGroundColor16bpp(color);
//  lcdRegDataWrite(RA8876_DEHR0,x0);//7bh
//  lcdRegDataWrite(RA8876_DEHR1,x0>>8);//7ch
//  lcdRegDataWrite(RA8876_DEVR0,y0);//7dh
//  lcdRegDataWrite(RA8876_DEVR1,y0>>8);//7eh
//  lcdRegDataWrite(RA8876_ELL_A0,xr);//77h
//  lcdRegDataWrite(RA8876_ELL_A1,xr>>8);//78h
//  lcdRegDataWrite(RA8876_ELL_B0,yr);//79h
//  lcdRegDataWrite(RA8876_ELL_B1,yr>>8);//7ah
//  lcdRegDataWrite(RA8876_DCR1,RA8876_DRAW_ELLIPSE_FILL);//76h,0xc0
//  check2dBusy();
//}
//
///*BTE function*/
////**************************************************************//
////**************************************************************//
//void bteMemoryCopy(uint16_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,uint16_t des_addr,uint16_t des_image_width,
//                                uint16_t des_x,uint16_t des_y,uint16_t copy_width,uint16_t copy_height)
//{
//  bte_Source0_MemoryStartAddr(s0_addr);
//  bte_Source0_ImageWidth(s0_image_width);
//  bte_Source0_WindowStartXY(s0_x,s0_y);
//  //bte_Source1_MemoryStartAddr(des_addr);
//  //bte_Source1_ImageWidth(des_image_width);
//  //bte_Source1_WindowStartXY(des_x,des_y);
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//
//  bte_WindowSize(copy_width,copy_height);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_CODE_12<<4|RA8876_BTE_MEMORY_COPY_WITH_ROP);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
//void bteMemoryCopyWithROP(uint16_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,uint16_t s1_addr,uint16_t s1_image_width,uint16_t s1_x,uint16_t s1_y,
//                                       uint16_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t copy_width,uint16_t copy_height,uint16_t rop_code)
//{
//  bte_Source0_MemoryStartAddr(s0_addr);
//  bte_Source0_ImageWidth(s0_image_width);
//  bte_Source0_WindowStartXY(s0_x,s0_y);
//  bte_Source1_MemoryStartAddr(s1_addr);
//  bte_Source1_ImageWidth(s1_image_width);
//  bte_Source1_WindowStartXY(s1_x,s1_y);
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(copy_width,copy_height);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,rop_code<<4|RA8876_BTE_MEMORY_COPY_WITH_ROP);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
//void bteMemoryCopyWithChromaKey(uint16_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,
//uint16_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t copy_width,uint16_t copy_height, uint16_t chromakey_color)
//{
//  bte_Source0_MemoryStartAddr(s0_addr);
//  bte_Source0_ImageWidth(s0_image_width);
//  bte_Source0_WindowStartXY(s0_x,s0_y);
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(copy_width,copy_height);
//  backGroundColor16bpp(chromakey_color);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_MEMORY_COPY_WITH_CHROMA);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
//void bteMpuWriteWithROP(uint16_t s1_addr,uint16_t s1_image_width,uint16_t s1_x,uint16_t s1_y,uint16_t des_addr,uint16_t des_image_width,
//uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t rop_code,const unsigned char *data)
//{
//   uint16_t i,j;
//  bte_Source1_MemoryStartAddr(s1_addr);
//  bte_Source1_ImageWidth(s1_image_width);
//  bte_Source1_WindowStartXY(s1_x,s1_y);
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(width,height);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,rop_code<<4|RA8876_BTE_MPU_WRITE_WITH_ROP);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  ramAccessPrepare();
//
//  for(i=0;i< height;i++)
//  {
//   for(j=0;j< (width*2);j++)
//   {
//    checkWriteFifoNotFull();
//    lcdDataWrite(*data);
//    data++;
//    }
//   }
//  checkWriteFifoEmpty();
//}
//
////**************************************************************//
////**************************************************************//
//void bteMpuWriteWithROP(uint16_t s1_addr,uint16_t s1_image_width,uint16_t s1_x,uint16_t s1_y,uint16_t des_addr,uint16_t des_image_width,
//uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t rop_code,const unsigned short *data)
//{
//   uint16_t i,j;
//  bte_Source1_MemoryStartAddr(s1_addr);
//  bte_Source1_ImageWidth(s1_image_width);
//  bte_Source1_WindowStartXY(s1_x,s1_y);
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(width,height);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,rop_code<<4|RA8876_BTE_MPU_WRITE_WITH_ROP);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  ramAccessPrepare();
//
// for(j=0;j<height;j++)
// {
//  for(i=0;i<width;i++)
//  {
//   checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
//   lcdDataWrite16bbp(*data);
//   data++;
//   //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
//  }
// }
//  checkWriteFifoEmpty();
//}
////**************************************************************//
////write data after setting
////**************************************************************//
//void bteMpuWriteWithROP(uint16_t s1_addr,uint16_t s1_image_width,uint16_t s1_x,uint16_t s1_y,uint16_t des_addr,uint16_t des_image_width,
//uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t rop_code)
//{
//   uint16_t i,j;
//  bte_Source1_MemoryStartAddr(s1_addr);
//  bte_Source1_ImageWidth(s1_image_width);
//  bte_Source1_WindowStartXY(s1_x,s1_y);
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(width,height);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,rop_code<<4|RA8876_BTE_MPU_WRITE_WITH_ROP);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  ramAccessPrepare();
//}
////**************************************************************//
////**************************************************************//
//void bteMpuWriteWithChromaKey(uint16_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t chromakey_color,const unsigned char *data)
//{
//  uint16_t i,j;
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(width,height);
//  backGroundColor16bpp(chromakey_color);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_MPU_WRITE_WITH_CHROMA);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  ramAccessPrepare();
//
//  for(i=0;i< height;i++)
//  {
//   for(j=0;j< (width*2);j++)
//   {
//    checkWriteFifoNotFull();
//    lcdDataWrite(*data);
//    data++;
//    }
//   }
//  checkWriteFifoEmpty();
//}
//
////**************************************************************//
////**************************************************************//
//void bteMpuWriteWithChromaKey(uint16_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t chromakey_color,const unsigned short *data)
//{
//  uint16_t i,j;
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(width,height);
//  backGroundColor16bpp(chromakey_color);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_MPU_WRITE_WITH_CHROMA);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  ramAccessPrepare();
//
// for(j=0;j<height;j++)
// {
//  for(i=0;i<width;i++)
//  {
//   checkWriteFifoNotFull();//if high speed mcu and without Xnwait check
//   lcdDataWrite16bbp(*data);
//   data++;
//   //checkWriteFifoEmpty();//if high speed mcu and without Xnwait check
//  }
// }
//  checkWriteFifoEmpty();
//}
////**************************************************************//
////write data after setting
////**************************************************************//
//void bteMpuWriteWithChromaKey(uint16_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t chromakey_color)
//{
//  uint16_t i,j;
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(width,height);
//  backGroundColor16bpp(chromakey_color);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_MPU_WRITE_WITH_CHROMA);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  ramAccessPrepare();
//}
////**************************************************************//
////**************************************************************//
//void bteMpuWriteColorExpansion(uint16_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t foreground_color,uint16_t background_color,const unsigned char *data)
//{
//  uint16_t i,j;
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(width,height);
//  foreGroundColor16bpp(foreground_color);
//  backGroundColor16bpp(background_color);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_BUS_WIDTH8<<4|RA8876_BTE_MPU_WRITE_COLOR_EXPANSION);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  ramAccessPrepare();
//
//  for(i=0;i< height;i++)
//  {
//   for(j=0;j< (width/8);j++)
//   {
//    checkWriteFifoNotFull();
//    lcdDataWrite(*data);
//    data++;
//    }
//   }
//  checkWriteFifoEmpty();
//  check2dBusy();
//}
////**************************************************************//
////write data after setting
////**************************************************************//
//void bteMpuWriteColorExpansion(uint16_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t foreground_color,uint16_t background_color)
//{
//  uint16_t i,j;
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(width,height);
//  foreGroundColor16bpp(foreground_color);
//  backGroundColor16bpp(background_color);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_BUS_WIDTH8<<4|RA8876_BTE_MPU_WRITE_COLOR_EXPANSION);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  ramAccessPrepare();
//}
////**************************************************************//
///*background_color do not set the same as foreground_color*/
////**************************************************************//
//void bteMpuWriteColorExpansionWithChromaKey(uint16_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t foreground_color,uint16_t background_color, const unsigned char *data)
//{
//  uint16_t i,j;
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(width,height);
//  foreGroundColor16bpp(foreground_color);
//  backGroundColor16bpp(background_color);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_BUS_WIDTH8<<4|RA8876_BTE_MPU_WRITE_COLOR_EXPANSION_WITH_CHROMA);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  ramAccessPrepare();
//
//  for(i=0;i< height;i++)
//  {
//   for(j=0;j< (width/8);j++)
//   {
//    checkWriteFifoNotFull();
//    lcdDataWrite(*data);
//    data++;
//    }
//   }
//  checkWriteFifoEmpty();
//  check2dBusy();
//}
////**************************************************************//
///*background_color do not set the same as foreground_color*/
////write data after setting
////**************************************************************//
//void bteMpuWriteColorExpansionWithChromaKey(uint16_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t foreground_color,uint16_t background_color)
//{
//  uint16_t i,j;
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(width,height);
//  foreGroundColor16bpp(foreground_color);
//  backGroundColor16bpp(background_color);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_BUS_WIDTH8<<4|RA8876_BTE_MPU_WRITE_COLOR_EXPANSION_WITH_CHROMA);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4);//90h
//  ramAccessPrepare();
//}
////**************************************************************//
////**************************************************************//
//void  btePatternFill(uint16_t p8x8or16x16, uint16_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,
//                                 uint16_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height)
//{
//  bte_Source0_MemoryStartAddr(s0_addr);
//  bte_Source0_ImageWidth(s0_image_width);
//  bte_Source0_WindowStartXY(s0_x,s0_y);
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(width,height);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_CODE_12<<4|RA8876_BTE_PATTERN_FILL_WITH_ROP);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//
//  if(p8x8or16x16 == 0)
//   lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4|RA8876_PATTERN_FORMAT8X8);//90h
//  else
//   lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4|RA8876_PATTERN_FORMAT16X16);//90h
//
//  check2dBusy();
//}
////**************************************************************//
////**************************************************************//
//void  btePatternFillWithChromaKey(uint16_t p8x8or16x16, uint16_t s0_addr,uint16_t s0_image_width,uint16_t s0_x,uint16_t s0_y,
//                                 uint16_t des_addr,uint16_t des_image_width, uint16_t des_x,uint16_t des_y,uint16_t width,uint16_t height,uint16_t chromakey_color)
//{
//  bte_Source0_MemoryStartAddr(s0_addr);
//  bte_Source0_ImageWidth(s0_image_width);
//  bte_Source0_WindowStartXY(s0_x,s0_y);
//  bte_DestinationMemoryStartAddr(des_addr);
//  bte_DestinationImageWidth(des_image_width);
//  bte_DestinationWindowStartXY(des_x,des_y);
//  bte_WindowSize(width,height);
//  backGroundColor16bpp(chromakey_color);
//  lcdRegDataWrite(RA8876_BTE_CTRL1,RA8876_BTE_ROP_CODE_12<<4|RA8876_BTE_PATTERN_FILL_WITH_CHROMA);//91h
//  lcdRegDataWrite(RA8876_BTE_COLR,RA8876_S0_COLOR_DEPTH_16BPP<<5|RA8876_S1_COLOR_DEPTH_16BPP<<2|RA8876_DESTINATION_COLOR_DEPTH_16BPP);//92h
//  if(p8x8or16x16 == 0)
//   lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4|RA8876_PATTERN_FORMAT8X8);//90h
//  else
//   lcdRegDataWrite(RA8876_BTE_CTRL0,RA8876_BTE_ENABLE<<4|RA8876_PATTERN_FORMAT16X16);//90h
//  check2dBusy();
//}
//
// /*DMA Function*/
// //**************************************************************//
// /*If used 32bit address serial flash through ra8876, must be set command to serial flash to enter 4bytes mode first.
// only needs set one times after power on */
// //**************************************************************//
// void setSerialFlash4BytesMode(uint16_t scs_select)
// {
//  if(scs_select==0)
//  {
//  lcdRegDataWrite( RA8876_SPIMCR2, RA8876_SPIM_NSS_SELECT_0<<5|RA8876_SPIM_MODE0);//b9h
//  lcdRegDataWrite( RA8876_SPIMCR2, RA8876_SPIM_NSS_SELECT_0<<5|RA8876_SPIM_NSS_ACTIVE<<4|RA8876_SPIM_MODE0);//b9h
//  lcdRegWrite( RA8876_SPIDR);//b8h
//  xDelay(1);
//  lcdDataWrite(0xB7);//
//  xDelay(1);
//  lcdRegDataWrite( RA8876_SPIMCR2, RA8876_SPIM_NSS_SELECT_0<<5|RA8876_SPIM_NSS_INACTIVE<<4|RA8876_SPIM_MODE0);//b9h
//  }
//  if(scs_select==1)
//  {
//  lcdRegDataWrite( RA8876_SPIMCR2 ,RA8876_SPIM_NSS_SELECT_1<<5|RA8876_SPIM_MODE0);//b9h
//  lcdRegDataWrite( RA8876_SPIMCR2, RA8876_SPIM_NSS_SELECT_1<<5|RA8876_SPIM_NSS_ACTIVE<<4|RA8876_SPIM_MODE0);//b9h
//  lcdRegWrite( RA8876_SPIDR);//b8h
//  xDelay(1);
//  lcdDataWrite(0xB7);//
//  xDelay(1);
//  lcdRegDataWrite( RA8876_SPIMCR2, RA8876_SPIM_NSS_SELECT_1<<5|RA8876_SPIM_NSS_INACTIVE<<4|RA8876_SPIM_MODE0);//b9h
//  }
// }
////**************************************************************//
///* scs = 0 : select scs0, scs = 1 : select scs1, */
////**************************************************************//
 void dma_24bitAddressBlockMode(uint16_t scs_select,uint16_t clk_div,uint16_t x0,uint16_t y0,uint16_t width,uint16_t height,uint16_t picture_width,uint32_t addr)
 {
   if(scs_select==0)
    lcdRegDataWrite(RA8876_SFL_CTRL,RA8876_SERIAL_FLASH_SELECT0<<7|RA8876_SERIAL_FLASH_DMA_MODE<<6|RA8876_SERIAL_FLASH_ADDR_24BIT<<5|RA8876_FOLLOW_RA8876_MODE<<4|RA8876_SPI_FAST_READ_8DUMMY);//b7h
   if(scs_select==1)
    lcdRegDataWrite(RA8876_SFL_CTRL,RA8876_SERIAL_FLASH_SELECT1<<7|RA8876_SERIAL_FLASH_DMA_MODE<<6|RA8876_SERIAL_FLASH_ADDR_24BIT<<5|RA8876_FOLLOW_RA8876_MODE<<4|RA8876_SPI_FAST_READ_8DUMMY);//b7h

  lcdRegDataWrite(RA8876_SPI_DIVSOR,clk_div);//bbh
  lcdRegDataWrite(RA8876_DMA_DX0,x0);//c0h
  lcdRegDataWrite(RA8876_DMA_DX1,x0>>8);//c1h
  lcdRegDataWrite(RA8876_DMA_DY0,y0);//c2h
  lcdRegDataWrite(RA8876_DMA_DY1,y0>>8);//c3h
  lcdRegDataWrite(RA8876_DMAW_WTH0,width);//c6h
  lcdRegDataWrite(RA8876_DMAW_WTH1,width>>8);//c7h
  lcdRegDataWrite(RA8876_DMAW_HIGH0,height);//c8h
  lcdRegDataWrite(RA8876_DMAW_HIGH1,height>>8);//c9h
  lcdRegDataWrite(RA8876_DMA_SWTH0,picture_width);//cah
  lcdRegDataWrite(RA8876_DMA_SWTH1,picture_width>>8);//cbh
  lcdRegDataWrite(RA8876_DMA_SSTR0,addr);//bch
  lcdRegDataWrite(RA8876_DMA_SSTR1,addr>>8);//bdh
  lcdRegDataWrite(RA8876_DMA_SSTR2,addr>>16);//beh
  lcdRegDataWrite(RA8876_DMA_SSTR3,addr>>24);//bfh

  lcdRegDataWrite(RA8876_DMA_CTRL,RA8876_DMA_START);//b6h
  HAL_Delay(100);
//  check2dBusy();
 }
// //**************************************************************//
///* scs = 0 : select scs0, scs = 1 : select scs1, */
////**************************************************************//
// void dma_32bitAddressBlockMode(uint16_t scs_select,uint16_t clk_div,uint16_t x0,uint16_t y0,uint16_t width,uint16_t height,uint16_t picture_width,uint16_t addr)
// {
//   if(scs_select==0)
//    lcdRegDataWrite(RA8876_SFL_CTRL,RA8876_SERIAL_FLASH_SELECT0<<7|RA8876_SERIAL_FLASH_DMA_MODE<<6|RA8876_SERIAL_FLASH_ADDR_32BIT<<5|RA8876_FOLLOW_RA8876_MODE<<4|RA8876_SPI_FAST_READ_8DUMMY);//b7h
//   if(scs_select==1)
//    lcdRegDataWrite(RA8876_SFL_CTRL,RA8876_SERIAL_FLASH_SELECT1<<7|RA8876_SERIAL_FLASH_DMA_MODE<<6|RA8876_SERIAL_FLASH_ADDR_32BIT<<5|RA8876_FOLLOW_RA8876_MODE<<4|RA8876_SPI_FAST_READ_8DUMMY);//b7h
//
//  lcdRegDataWrite(RA8876_SPI_DIVSOR,clk_div);//bbh
//
//  lcdRegDataWrite(RA8876_DMA_DX0,x0);//c0h
//  lcdRegDataWrite(RA8876_DMA_DX1,x0>>8);//c1h
//  lcdRegDataWrite(RA8876_DMA_DY0,y0);//c2h
//  lcdRegDataWrite(RA8876_DMA_DY1,y0>>8);//c3h
//  lcdRegDataWrite(RA8876_DMAW_WTH0,width);//c6h
//  lcdRegDataWrite(RA8876_DMAW_WTH1,width>>8);//c7h
//  lcdRegDataWrite(RA8876_DMAW_HIGH0,height);//c8h
//  lcdRegDataWrite(RA8876_DMAW_HIGH1,height>>8);//c9h
//  lcdRegDataWrite(RA8876_DMA_SWTH0,picture_width);//cah
//  lcdRegDataWrite(RA8876_DMA_SWTH1,picture_width>>8);//cbh
//  lcdRegDataWrite(RA8876_DMA_SSTR0,addr);//bch
//  lcdRegDataWrite(RA8876_DMA_SSTR1,addr>>8);//bdh
//  lcdRegDataWrite(RA8876_DMA_SSTR2,addr>>16);//beh
//  lcdRegDataWrite(RA8876_DMA_SSTR3,addr>>24);//bfh
//
//  lcdRegDataWrite(RA8876_DMA_CTRL,RA8876_DMA_START);//b6h
//  check2dBusy();
// }
//

