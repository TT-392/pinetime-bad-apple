// the following copyright notice applies to this file. It is taken from https://github.com/xriss/timecake

// Copyright (c) 2020 Kriss Blank <kriss@wetgenes.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#pragma once
  
#define LCD_SCK             2
#define LCD_MOSI            3
#define LCD_MISO            4
#define LCD_SELECT         25
#define LCD_COMMAND        18
#define LCD_RESET          26

#define LCD_BACKLIGHT_LOW  14
#define LCD_BACKLIGHT_MID  22
#define LCD_BACKLIGHT_HIGH 23

// sitronix ST7789V LCD commands from https://wiki.pine64.org/images/5/54/ST7789V_v1.6.pdf

#define CMD_NOP       0x00 // NOP
#define CMD_SWRESET   0x01 // Software Reset
#define CMD_RDDID     0x04 // Read Display ID
#define CMD_RDDST     0x09 // Read Display Status
#define CMD_RDDPM     0x0A // Read Display Power Mode
#define CMD_RDDMADCTL 0x0B // Read Display MADCTL
#define CMD_RDDCOLMOD 0x0C // Read Display Pixel Format
#define CMD_RDDIM     0x0D // Read Display Image Mode
#define CMD_RDDSM     0x0E // Read Display Signal Mode
#define CMD_RDDSDR    0x0F // Read Display Self-Diagnostic Result
#define CMD_SLPIN     0x10 // Sleep in
#define CMD_SLPOUT    0x11 // Sleep Out
#define CMD_PTLON     0x12 // Partial Display Mode On
#define CMD_NORON     0x13 // Normal Display Mode On
#define CMD_INVOFF    0x20 // Display Inversion Off
#define CMD_INVON     0x21 // Display Inversion On
#define CMD_GAMSET    0x26 // Gamma Set
#define CMD_DISPOFF   0x28 // Display Off
#define CMD_DISPON    0x29 // Display On
#define CMD_CASET     0x2A // Column Address Set
#define CMD_RASET     0x2B // Row Address Set
#define CMD_RAMWR     0x2C // Memory Write
#define CMD_RAMRD     0x2E // Memory Read
#define CMD_PTLAR     0x30 // Partial Area
#define CMD_VSCRDEF   0x33 // Vertical Scrolling Definition
#define CMD_TEOFF     0x34 // Tearing Effect Line OFF
#define CMD_TEON      0x35 // Tearing Effect Line On
#define CMD_MADCTL    0x36 // Memory Data Access Control
#define CMD_VSCSAD    0x37 // Vertical Scroll Start Address of RAM
#define CMD_IDMOFF    0x38 // Idle Mode Off
#define CMD_IDMON     0x39 // Idle mode on
#define CMD_COLMOD    0x3A // Interface Pixel Format
#define CMD_WRMEMC    0x3C // Write Memory Continue
#define CMD_RDMEMC    0x3E // Read Memory Continue
#define CMD_STE       0x44 // Set Tear Scanline
#define CMD_GSCAN     0x45 // Get Scanline
#define CMD_WRDISBV   0x51 // Write Display Brightness
#define CMD_RDDISBV   0x52 // Read Display Brightness Value
#define CMD_WRCTRLD   0x53 // Write CTRL Display
#define CMD_RDCTRLD   0x54 // Read CTRL Value Display
#define CMD_WRCACE    0x55 // Write Content Adaptive Brightness Control and Color Enhancement
#define CMD_RDCABC    0x56 // Read Content Adaptive Brightness Control
#define CMD_WRCABCMB  0x5E // Write CABC Minimum Brightness
#define CMD_RDCABCMB  0x5F // Read CABC Minimum Brightness
#define CMD_RDABCSDR  0x68 // Read Automatic Brightness Control Self-Diagnostic Result
#define CMD_RDID1     0xDA // Read ID1
#define CMD_RDID2     0xDB // Read ID2
#define CMD_RDID3     0xDC // Read ID3
#define CMD_RAMCTRL   0xB0 // RAM Control
#define CMD_RGBCTRL   0xB1 // RGB Interface Control
#define CMD_PORCTRL   0xB2 // Porch Setting
#define CMD_FRCTRL1   0xB3 // Frame Rate Control 1 0xIn partial mode/ idle colors)
#define CMD_PARCTRL   0xB5 // Partial Control
#define CMD_GCTRL     0xB7 // Gate Control
#define CMD_GTADJ     0xB8 // Gate On Timing Adjustment
#define CMD_DGMEN     0xBA // Digital Gamma Enable
#define CMD_VCOMS     0xBB // VCOM Setting
#define CMD_LCMCTRL   0xC0 // LCM Control
#define CMD_IDSET     0xC1 // ID Code Setting
#define CMD_VDVVRHEN  0xC2 // VDV and VRH Command Enable
#define CMD_VRHS      0xC3 // VRH Set
#define CMD_VDVS      0xC4 // VDV Set
#define CMD_VCMOFSET  0xC5 // VCOM Offset Set
#define CMD_FRCTRL2   0xC6 // Frame Rate Control in Normal Mode
#define CMD_CABCCTRL  0xC7 // CABC Control
#define CMD_REGSEL1   0xC8 // Register Value Selection 1
#define CMD_REGSEL2   0xCA // Register Value Selection 2
#define CMD_PWMFRSEL  0xCC // PWM Frequency Selection
#define CMD_PWCTRL1   0xD0 // Power Control 1
#define CMD_VAPVANEN  0xD2 // Enable VAP/VAN signal output
#define CMD_CMD2EN    0xDF // Command 2 Enable
#define CMD_PVGAMCTRL 0xE0 // Positive Voltage Gamma Control
#define CMD_NVGAMCTRL 0xE1 // Negative Voltage Gamma Control
#define CMD_DGMLUTR   0xE2 // Digital Gamma Look-up Table for Red
#define CMD_DGMLUTB   0xE3 // Digital Gamma Look-up Table for Blue
#define CMD_GATECTRL  0xE4 // Gate Control
#define CMD_SPI2EN    0xE7 // SPI2 Enable
#define CMD_PWCTRL2   0xE8 // Power Control 2
#define CMD_EQCTRL    0xE9 // Equalize time control
#define CMD_PROMCTRL  0xEC // Program Mode Control
#define CMD_PROMEN    0xFA // Program Mode Enable
#define CMD_NVMSET    0xFC // NVM Setting
#define CMD_PROMACT   0xFE // Program action 
