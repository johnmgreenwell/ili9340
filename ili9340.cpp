/***************************************************
  This is an Arduino Library for the Adafruit 2.2" SPI display.
  This library works with the Adafruit 2.2" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/1480
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution

    Modified to add support for custom HAL support, John Greenwell 2024
 ****************************************************/

#include "ili9340.h"

#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

namespace PeripheralIO
{

ILI9340::ILI9340(HAL::SPI& spi, uint8_t cs, uint8_t dc, uint8_t rst)
: Adafruit_GFX(ILI9340_TFTWIDTH, ILI9340_TFTHEIGHT)
, _spi(spi)
, _cs(cs)
, _dc(dc)
, _rst(rst)
{ }

void ILI9340::spiwrite(uint8_t c) {

  //Serial.print("0x"); Serial.print(c, HEX); Serial.print(", ");

    _spi.transfer(c);

//   if (hwSPI) {
// #ifdef __AVR__
//     SPDR = c;
//     while(!(SPSR & _BV(SPIF)));
// #endif
// #if defined(USE_SPI_LIBRARY)
//     SPI.transfer(c);
// #endif
//   } else {
//     // Fast SPI bitbang swiped from LPD8806 library
//     for(uint8_t bit = 0x80; bit; bit >>= 1) {
//       if(c & bit) {
//         //digitalWrite(_mosi, HIGH); 
//         SET_BIT(mosiport, mosipinmask);
//       } else {
//         //digitalWrite(_mosi, LOW); 
//         CLEAR_BIT(mosiport, mosipinmask);
//       }
//       //digitalWrite(_sclk, HIGH);
//       SET_BIT(clkport, clkpinmask);
//       //digitalWrite(_sclk, LOW);
//       CLEAR_BIT(clkport, clkpinmask);
//     }
//   }
}


void ILI9340::writecommand(uint8_t c) {
  _dc.digitalWrite(false);
  //CLEAR_BIT(dcport, dcpinmask);
  //digitalWrite(_dc, LOW);
  //CLEAR_BIT(clkport, clkpinmask);
  //digitalWrite(_sclk, LOW);
  _cs.digitalWrite(false);
  //CLEAR_BIT(csport, cspinmask);
  //digitalWrite(_cs, LOW);

  spiwrite(c);

  _cs.digitalWrite(true);
  //SET_BIT(csport, cspinmask);
  //digitalWrite(_cs, HIGH);
}


void ILI9340::writedata(uint8_t c) {
  _dc.digitalWrite(true);
  //SET_BIT(dcport,  dcpinmask);
  //digitalWrite(_dc, HIGH);
  //CLEAR_BIT(clkport, clkpinmask);
  //digitalWrite(_sclk, LOW);
  _cs.digitalWrite(false);
  //CLEAR_BIT(csport, cspinmask);
  //digitalWrite(_cs, LOW);
  
  spiwrite(c);

  _cs.digitalWrite(true);
  //digitalWrite(_cs, HIGH);
  //SET_BIT(csport, cspinmask);
} 

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void ILI9340::commandList(uint8_t *addr) {

  uint8_t  numCommands, numArgs;
  uint16_t ms;

  numCommands = pgm_read_byte(addr++);   // Number of commands to follow
  while(numCommands--) {                 // For each command...
    writecommand(pgm_read_byte(addr++)); //   Read, issue command
    numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
    ms       = numArgs & DELAY;          //   If hibit set, delay follows args
    numArgs &= ~DELAY;                   //   Mask out delay bit
    while(numArgs--) {                   //   For each argument...
      writedata(pgm_read_byte(addr++));  //     Read, issue argument
    }

    if(ms) {
      ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      delay(ms);
    }
  }
}


void ILI9340::begin(void) {
  //pinMode(_rst, OUTPUT);
  //digitalWrite(_rst, LOW);
  //pinMode(_dc, OUTPUT);
  //pinMode(_cs, OUTPUT);
  _rst.pinMode(GPIO_OUTPUT);
  _rst.digitalWrite(false);
  _dc.pinMode(GPIO_OUTPUT);
  _cs.pinMode(GPIO_OUTPUT);

  // toggle RST low to reset

  _rst.digitalWrite(true);
  HAL::delay_ms(5);
  _rst.digitalWrite(false);
  HAL::delay_ms(20);
  _rst.digitalWrite(true);
  HAL::delay_ms(150);

  /*
  uint8_t x = readcommand8(ILI9340_RDMODE);
  Serial.print("\nDisplay Power Mode: 0x"); Serial.println(x, HEX);
  x = readcommand8(ILI9340_RDMADCTL);
  Serial.print("\nMADCTL Mode: 0x"); Serial.println(x, HEX);
  x = readcommand8(ILI9340_RDPIXFMT);
  Serial.print("\nPixel Format: 0x"); Serial.println(x, HEX);
  x = readcommand8(ILI9340_RDIMGFMT);
  Serial.print("\nImage Format: 0x"); Serial.println(x, HEX);
  x = readcommand8(ILI9340_RDSELFDIAG);
  Serial.print("\nSelf Diagnostic: 0x"); Serial.println(x, HEX);
  */

  //if(cmdList) commandList(cmdList);
  
  writecommand(0xEF);
  writedata(0x03);
  writedata(0x80);
  writedata(0x02);

  writecommand(0xCF);  
  writedata(0x00); 
  writedata(0XC1); 
  writedata(0X30); 

  writecommand(0xED);  
  writedata(0x64); 
  writedata(0x03); 
  writedata(0X12); 
  writedata(0X81); 
 
  writecommand(0xE8);  
  writedata(0x85); 
  writedata(0x00); 
  writedata(0x78); 

  writecommand(0xCB);  
  writedata(0x39); 
  writedata(0x2C); 
  writedata(0x00); 
  writedata(0x34); 
  writedata(0x02); 
 
  writecommand(0xF7);  
  writedata(0x20); 

  writecommand(0xEA);  
  writedata(0x00); 
  writedata(0x00); 
 
  writecommand(ILI9340_PWCTR1);    //Power control 
  writedata(0x23);   //VRH[5:0] 
 
  writecommand(ILI9340_PWCTR2);    //Power control 
  writedata(0x10);   //SAP[2:0];BT[3:0] 
 
  writecommand(ILI9340_VMCTR1);    //VCM control 
  writedata(0x3e); //�Աȶȵ���
  writedata(0x28); 
  
  writecommand(ILI9340_VMCTR2);    //VCM control2 
  writedata(0x86);  //--
 
  writecommand(ILI9340_MADCTL);    // Memory Access Control 
  writedata(ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);

  writecommand(ILI9340_PIXFMT);    
  writedata(0x55); 
  
  writecommand(ILI9340_FRMCTR1);    
  writedata(0x00);  
  writedata(0x18); 
 
  writecommand(ILI9340_DFUNCTR);    // Display Function Control 
  writedata(0x08); 
  writedata(0x82);
  writedata(0x27);  
 
  writecommand(0xF2);    // 3Gamma Function Disable 
  writedata(0x00); 
 
  writecommand(ILI9340_GAMMASET);    //Gamma curve selected 
  writedata(0x01); 
 
  writecommand(ILI9340_GMCTRP1);    //Set Gamma 
  writedata(0x0F); 
  writedata(0x31); 
  writedata(0x2B); 
  writedata(0x0C); 
  writedata(0x0E); 
  writedata(0x08); 
  writedata(0x4E); 
  writedata(0xF1); 
  writedata(0x37); 
  writedata(0x07); 
  writedata(0x10); 
  writedata(0x03); 
  writedata(0x0E); 
  writedata(0x09); 
  writedata(0x00); 
  
  writecommand(ILI9340_GMCTRN1);    //Set Gamma 
  writedata(0x00); 
  writedata(0x0E); 
  writedata(0x14); 
  writedata(0x03); 
  writedata(0x11); 
  writedata(0x07); 
  writedata(0x31); 
  writedata(0xC1); 
  writedata(0x48); 
  writedata(0x08); 
  writedata(0x0F); 
  writedata(0x0C); 
  writedata(0x31); 
  writedata(0x36); 
  writedata(0x0F); 

  writecommand(ILI9340_SLPOUT);    //Exit Sleep 
  delay(120); 		
  writecommand(ILI9340_DISPON);    //Display on 
}


void ILI9340::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1,
 uint16_t y1) {

  writecommand(ILI9340_CASET); // Column addr set
  writedata(x0 >> 8);
  writedata(x0 & 0xFF);     // XSTART 
  writedata(x1 >> 8);
  writedata(x1 & 0xFF);     // XEND

  writecommand(ILI9340_PASET); // Row addr set
  writedata(y0>>8);
  writedata(y0);     // YSTART
  writedata(y1>>8);
  writedata(y1);     // YEND

  writecommand(ILI9340_RAMWR); // write to RAM
}


void ILI9340::pushColor(uint16_t color) {
  _dc.digitalWrite(true);
  //digitalWrite(_dc, HIGH);
  //SET_BIT(dcport, dcpinmask);
  _cs.digitalWrite(false);
  //digitalWrite(_cs, LOW);
  //CLEAR_BIT(csport, cspinmask);

  spiwrite(color >> 8);
  spiwrite(color);

  _cs.digitalWrite(true);
  //SET_BIT(csport, cspinmask);
  //digitalWrite(_cs, HIGH);
}

void ILI9340::drawPixel(int16_t x, int16_t y, uint16_t color) {

  if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

  setAddrWindow(x,y,x+1,y+1);

  _dc.digitalWrite(true);
  //digitalWrite(_dc, HIGH);
  //SET_BIT(dcport, dcpinmask);
  _cs.digitalWrite(false);
  //digitalWrite(_cs, LOW);
  //CLEAR_BIT(csport, cspinmask);

  spiwrite(color >> 8);
  spiwrite(color);

  _cs.digitalWrite(false);
  //SET_BIT(csport, cspinmask);
  //digitalWrite(_cs, HIGH);
}


void ILI9340::drawFastVLine(int16_t x, int16_t y, int16_t h,
 uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;

  if((y+h-1) >= _height) 
    h = _height-y;

  setAddrWindow(x, y, x, y+h-1);

  uint8_t hi = color >> 8, lo = color;

  _dc.digitalWrite(true);
  //SET_BIT(dcport, dcpinmask);
  //digitalWrite(_dc, HIGH);
  _cs.digitalWrite(false);
  //CLEAR_BIT(csport, cspinmask);
  //digitalWrite(_cs, LOW);

  while (h--) {
    spiwrite(hi);
    spiwrite(lo);
  }
  _cs.digitalWrite(true);
  //SET_BIT(csport, cspinmask);
  //digitalWrite(_cs, HIGH);
}


void ILI9340::drawFastHLine(int16_t x, int16_t y, int16_t w,
  uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width)  w = _width-x;
  setAddrWindow(x, y, x+w-1, y);

  uint8_t hi = color >> 8, lo = color;
  _dc.digitalWrite(true);
  //SET_BIT(dcport, dcpinmask);
  _cs.digitalWrite(false);
  //CLEAR_BIT(csport, cspinmask);
  //digitalWrite(_dc, HIGH);
  //digitalWrite(_cs, LOW);
  while (w--) {
    spiwrite(hi);
    spiwrite(lo);
  }
  _cs.digitalWrite(true);
  //SET_BIT(csport, cspinmask);
  //digitalWrite(_cs, HIGH);
}

void ILI9340::fillScreen(uint16_t color) {
  fillRect(0, 0,  _width, _height, color);
}

// fill a rectangle
void ILI9340::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
  uint16_t color) {

  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;

  setAddrWindow(x, y, x+w-1, y+h-1);

  uint8_t hi = color >> 8, lo = color;

  _dc.digitalWrite(true);
  //SET_BIT(dcport, dcpinmask);
  //digitalWrite(_dc, HIGH);
  _cs.digitalWrite(false);
  //CLEAR_BIT(csport, cspinmask);
  //digitalWrite(_cs, LOW);

  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      spiwrite(hi);
      spiwrite(lo);
    }
  }
  _cs.digitalWrite(true);
  //digitalWrite(_cs, HIGH);
  //SET_BIT(csport, cspinmask);
}


// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t ILI9340::Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


void ILI9340::setRotation(uint8_t m) {

  writecommand(ILI9340_MADCTL);
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
     writedata(ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
     _width  = ILI9340_TFTWIDTH;
     _height = ILI9340_TFTHEIGHT;
     break;
   case 1:
     writedata(ILI9340_MADCTL_MV | ILI9340_MADCTL_BGR);
     _width  = ILI9340_TFTHEIGHT;
     _height = ILI9340_TFTWIDTH;
     break;
  case 2:
    writedata(ILI9340_MADCTL_MY | ILI9340_MADCTL_BGR);
     _width  = ILI9340_TFTWIDTH;
     _height = ILI9340_TFTHEIGHT;
    break;
   case 3:
     writedata(ILI9340_MADCTL_MV | ILI9340_MADCTL_MY | ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
     _width  = ILI9340_TFTHEIGHT;
     _height = ILI9340_TFTWIDTH;
     break;
  }
}


void ILI9340::invertDisplay(boolean i) {
  writecommand(i ? ILI9340_INVON : ILI9340_INVOFF);
}


////////// stuff not actively being used, but kept for posterity

/*
uint8_t ILI9340::spiread(void) {
  uint8_t r = 0;

  if (hwSPI) {
#ifdef __AVR__
    SPDR = 0x00;
    while(!(SPSR & _BV(SPIF)));
    r = SPDR;
#endif
#if defined(USE_SPI_LIBRARY)
    r = SPI.transfer(0x00);
#endif
  } else {

    for (uint8_t i=0; i<8; i++) {
      digitalWrite(_sclk, LOW);
      digitalWrite(_sclk, HIGH);
      r <<= 1;
      if (digitalRead(_miso))
	r |= 0x1;
    }
  }
  //Serial.print("read: 0x"); Serial.print(r, HEX);
  
  return r;
}

 uint8_t ILI9340::readdata(void) {
   digitalWrite(_dc, HIGH);
   digitalWrite(_cs, LOW);
   uint8_t r = spiread();
   digitalWrite(_cs, HIGH);
   
   return r;
}
 

 uint8_t ILI9340::readcommand8(uint8_t c) {
   digitalWrite(_dc, LOW);
   digitalWrite(_sclk, LOW);
   digitalWrite(_cs, LOW);
   spiwrite(c);
 
   digitalWrite(_dc, HIGH);
   uint8_t r = spiread();
   digitalWrite(_cs, HIGH);
   return r;
}

*/
 
/*

 uint16_t Adafruit_ILI9340::readcommand16(uint8_t c) {
 digitalWrite(_dc, LOW);
 if (_cs)
 digitalWrite(_cs, LOW);
 
 spiwrite(c);
 pinMode(_sid, INPUT); // input!
 uint16_t r = spiread();
 r <<= 8;
 r |= spiread();
 if (_cs)
 digitalWrite(_cs, HIGH);
 
 pinMode(_sid, OUTPUT); // back to output
 return r;
 }
 
 uint32_t Adafruit_ILI9340::readcommand32(uint8_t c) {
 digitalWrite(_dc, LOW);
 if (_cs)
 digitalWrite(_cs, LOW);
 spiwrite(c);
 pinMode(_sid, INPUT); // input!
 
 dummyclock();
 dummyclock();
 
 uint32_t r = spiread();
 r <<= 8;
 r |= spiread();
 r <<= 8;
 r |= spiread();
 r <<= 8;
 r |= spiread();
 if (_cs)
 digitalWrite(_cs, HIGH);
 
 pinMode(_sid, OUTPUT); // back to output
 return r;
 }
 
 */

}

// EOF
