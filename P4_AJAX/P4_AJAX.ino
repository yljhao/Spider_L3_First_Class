/*****************************************************************************
*
*  WiFiSwitch.ino - Remote switch with http server interface. 
*
*  Copyright (c) 2014, FunMaker Ltd.
*
*
*  This library porting from CC3000 host driver, which works with 
*  Spider_L3S WiFi module.
*
*  Spider_L3S wifi module is developed by Funmaker, we are actively 
*  involved in Taiwan maker community, and we aims to support makers 
*  to make more creative projects. 
*
*  You can support us, by buying this wifi module, and we are looking
*  forward to see your awesome projects!
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include "EEPROM.h"
#include "Wire.h"

#include <stdio.h>
#include <string.h>

#include "Spider_L3.h"
#include "WebClient.h"
#include "WebServer.h"
#include "utility/twi.h"

static unsigned char PROGMEM index_data[923]=
{
    0x3C,0x21,0x44,0x4F,0x43,0x54,0x59,0x50,0x45,0x20,0x68,
    0x74,0x6D,0x6C,0x3E,0x0D,0x0A,0x3C,0x68,0x74,0x6D,0x6C,
    0x3E,0x0D,0x0A,0x09,0x3C,0x68,0x65,0x61,0x64,0x3E,0x0D,
    0x0A,0x09,0x09,0x3C,0x73,0x63,0x72,0x69,0x70,0x74,0x3E,
    0x0D,0x0A,0x09,0x09,0x09,0x66,0x75,0x6E,0x63,0x74,0x69,
    0x6F,0x6E,0x20,0x53,0x57,0x50,0x72,0x65,0x73,0x73,0x28,
    0x53,0x74,0x72,0x69,0x6E,0x67,0x29,0x7B,0x0D,0x0A,0x09,
    0x09,0x09,0x09,0x76,0x61,0x72,0x20,0x72,0x65,0x71,0x75,
    0x65,0x73,0x74,0x20,0x3D,0x20,0x6E,0x65,0x77,0x20,0x58,
    0x4D,0x4C,0x48,0x74,0x74,0x70,0x52,0x65,0x71,0x75,0x65,
    0x73,0x74,0x28,0x29,0x3B,0x0D,0x0A,0x09,0x09,0x09,0x09,
    0x72,0x65,0x71,0x75,0x65,0x73,0x74,0x2E,0x6F,0x6E,0x72,
    0x65,0x61,0x64,0x79,0x73,0x74,0x61,0x74,0x65,0x63,0x68,
    0x61,0x6E,0x67,0x65,0x3D,0x66,0x75,0x6E,0x63,0x74,0x69,
    0x6F,0x6E,0x28,0x29,0x0D,0x0A,0x09,0x09,0x09,0x09,0x7B,
    0x0D,0x0A,0x09,0x09,0x09,0x09,0x09,0x69,0x66,0x20,0x28,
    0x72,0x65,0x71,0x75,0x65,0x73,0x74,0x2E,0x72,0x65,0x61,
    0x64,0x79,0x53,0x74,0x61,0x74,0x65,0x3D,0x3D,0x34,0x20,
    0x26,0x26,0x20,0x72,0x65,0x71,0x75,0x65,0x73,0x74,0x2E,
    0x73,0x74,0x61,0x74,0x75,0x73,0x3D,0x3D,0x32,0x30,0x30,
    0x29,0x0D,0x0A,0x09,0x09,0x09,0x09,0x09,0x7B,0x0D,0x0A,
    0x09,0x09,0x09,0x09,0x09,0x09,0x64,0x6F,0x63,0x75,0x6D,
    0x65,0x6E,0x74,0x2E,0x67,0x65,0x74,0x45,0x6C,0x65,0x6D,
    0x65,0x6E,0x74,0x42,0x79,0x49,0x64,0x28,0x22,0x73,0x77,
    0x69,0x74,0x63,0x68,0x5F,0x74,0x65,0x78,0x74,0x22,0x29,
    0x2E,0x69,0x6E,0x6E,0x65,0x72,0x48,0x54,0x4D,0x4C,0x3D,
    0x72,0x65,0x71,0x75,0x65,0x73,0x74,0x2E,0x72,0x65,0x73,
    0x70,0x6F,0x6E,0x73,0x65,0x54,0x65,0x78,0x74,0x3B,0x0D,
    0x0A,0x09,0x09,0x09,0x09,0x09,0x09,0x72,0x65,0x71,0x75,
    0x65,0x73,0x74,0x2E,0x61,0x62,0x6F,0x72,0x74,0x28,0x29,
    0x3B,0x0D,0x0A,0x09,0x09,0x09,0x09,0x09,0x7D,0x0D,0x0A,
    0x09,0x09,0x09,0x09,0x7D,0x0D,0x0A,0x09,0x09,0x09,0x09,
    0x72,0x65,0x71,0x75,0x65,0x73,0x74,0x2E,0x6F,0x70,0x65,
    0x6E,0x28,0x22,0x50,0x4F,0x53,0x54,0x22,0x2C,0x22,0x3F,
    0x22,0x2B,0x53,0x74,0x72,0x69,0x6E,0x67,0x2C,0x66,0x61,
    0x6C,0x73,0x65,0x29,0x3B,0x0D,0x0A,0x09,0x09,0x09,0x09,
    0x72,0x65,0x71,0x75,0x65,0x73,0x74,0x2E,0x73,0x65,0x6E,
    0x64,0x28,0x6E,0x75,0x6C,0x6C,0x29,0x3B,0x0D,0x0A,0x09,
    0x09,0x09,0x7D,0x0D,0x0A,0x09,0x09,0x3C,0x2F,0x73,0x63,
    0x72,0x69,0x70,0x74,0x3E,0x0D,0x0A,0x09,0x3C,0x2F,0x68,
    0x65,0x61,0x64,0x3E,0x0D,0x0A,0x09,0x3C,0x62,0x6F,0x64,
    0x79,0x20,0x6F,0x6E,0x6C,0x6F,0x61,0x64,0x3D,0x22,0x53,
    0x57,0x50,0x72,0x65,0x73,0x73,0x28,0x27,0x4F,0x6E,0x6C,
    0x6F,0x61,0x64,0x27,0x29,0x22,0x3E,0x0D,0x0A,0x09,0x09,
    0x3C,0x68,0x32,0x3E,0x46,0x75,0x6E,0x4D,0x61,0x6B,0x65,
    0x72,0x20,0x57,0x69,0x46,0x69,0x20,0x73,0x77,0x69,0x74,
    0x63,0x68,0x2E,0x3C,0x2F,0x68,0x32,0x3E,0x0D,0x0A,0x09,
    0x09,0x3C,0x69,0x6D,0x67,0x20,0x73,0x72,0x63,0x3D,0x22,
    0x6C,0x6F,0x67,0x6F,0x2E,0x70,0x6E,0x67,0x22,0x3E,0x0D,
    0x0A,0x09,0x09,0x3C,0x70,0x3E,0x3C,0x2F,0x70,0x3E,0x0D,
    0x0A,0x09,0x09,0x3C,0x70,0x20,0x20,0x73,0x74,0x79,0x6C,
    0x65,0x3D,0x22,0x66,0x6F,0x6E,0x74,0x2D,0x73,0x69,0x7A,
    0x65,0x3A,0x32,0x34,0x70,0x74,0x3B,0x22,0x20,0x69,0x64,
    0x3D,0x22,0x73,0x77,0x69,0x74,0x63,0x68,0x5F,0x74,0x65,
    0x78,0x74,0x22,0x3E,0x53,0x77,0x69,0x74,0x63,0x68,0x20,
    0x73,0x74,0x61,0x74,0x65,0x3A,0x20,0x4E,0x6F,0x74,0x20,
    0x72,0x65,0x71,0x75,0x65,0x73,0x74,0x65,0x64,0x2E,0x2E,
    0x2E,0x2E,0x3C,0x2F,0x70,0x3E,0x0D,0x0A,0x09,0x09,0x3C,
    0x62,0x75,0x74,0x74,0x6F,0x6E,0x20,0x74,0x79,0x70,0x65,
    0x3D,0x22,0x62,0x75,0x74,0x74,0x6F,0x6E,0x22,0x20,0x73,
    0x74,0x79,0x6C,0x65,0x3D,0x22,0x66,0x6F,0x6E,0x74,0x2D,
    0x73,0x69,0x7A,0x65,0x3A,0x32,0x34,0x70,0x74,0x3B,0x20,
    0x68,0x65,0x69,0x67,0x68,0x74,0x3A,0x39,0x30,0x70,0x78,
    0x3B,0x20,0x77,0x69,0x64,0x74,0x68,0x3A,0x33,0x30,0x30,
    0x70,0x78,0x3B,0x20,0x63,0x6F,0x6C,0x6F,0x72,0x3A,0x72,
    0x65,0x64,0x3B,0x22,0x20,0x6F,0x6E,0x63,0x6C,0x69,0x63,
    0x6B,0x3D,0x22,0x53,0x57,0x50,0x72,0x65,0x73,0x73,0x28,
    0x27,0x53,0x57,0x3D,0x4F,0x46,0x46,0x27,0x29,0x22,0x3E,
    0x50,0x6F,0x77,0x65,0x72,0x20,0x4F,0x46,0x46,0x3C,0x2F,
    0x62,0x75,0x74,0x74,0x6F,0x6E,0x3E,0x3C,0x62,0x75,0x74,
    0x74,0x6F,0x6E,0x20,0x74,0x79,0x70,0x65,0x3D,0x22,0x62,
    0x75,0x74,0x74,0x6F,0x6E,0x22,0x20,0x73,0x74,0x79,0x6C,
    0x65,0x3D,0x22,0x66,0x6F,0x6E,0x74,0x2D,0x73,0x69,0x7A,
    0x65,0x3A,0x32,0x34,0x70,0x74,0x3B,0x20,0x68,0x65,0x69,
    0x67,0x68,0x74,0x3A,0x39,0x30,0x70,0x78,0x3B,0x20,0x77,
    0x69,0x64,0x74,0x68,0x3A,0x33,0x30,0x30,0x70,0x78,0x3B,
    0x63,0x6F,0x6C,0x6F,0x72,0x3A,0x67,0x72,0x65,0x65,0x6E,
    0x3B,0x22,0x20,0x6F,0x6E,0x63,0x6C,0x69,0x63,0x6B,0x3D,
    0x22,0x53,0x57,0x50,0x72,0x65,0x73,0x73,0x28,0x27,0x53,
    0x57,0x3D,0x4F,0x4E,0x27,0x29,0x22,0x3E,0x50,0x6F,0x77,
    0x65,0x72,0x20,0x4F,0x4E,0x3C,0x2F,0x62,0x75,0x74,0x74,
    0x6F,0x6E,0x3E,0x0D,0x0A,0x09,0x09,0x0D,0x0A,0x09,0x3C,
    0x2F,0x62,0x6F,0x64,0x79,0x3E,0x0D,0x0A,0x3C,0x2F,0x68,
    0x74,0x6D,0x6C,0x3E,0x0D,0x0A,0x0D,0x0A,0x0D,0x0A,
};

static unsigned char PROGMEM data_Logo_png[3134]=
{
    0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A,0x00,0x00,0x00,
    0x0D,0x49,0x48,0x44,0x52,0x00,0x00,0x00,0x80,0x00,0x00,
    0x00,0x86,0x08,0x00,0x00,0x00,0x00,0x30,0x0C,0xDD,0x0A,
    0x00,0x00,0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x15,
    0xD6,0x00,0x00,0x15,0xD6,0x01,0xD6,0x6B,0x59,0xFD,0x00,
    0x00,0x0B,0xF0,0x49,0x44,0x41,0x54,0x78,0x9C,0xED,0x9B,
    0x7B,0x74,0x55,0xD5,0x9D,0xC7,0xBF,0x7B,0x9F,0x7B,0xF3,
    0x0E,0x09,0x09,0x8F,0x24,0x8A,0x46,0x42,0x85,0x8A,0xB4,
    0x06,0x6B,0x6B,0x2B,0x75,0x18,0x26,0x62,0x51,0x5A,0x0A,
    0xD6,0x6A,0x15,0xDA,0x3A,0x75,0xDA,0x51,0xAA,0x63,0x2B,
    0xD4,0x65,0x3B,0x4A,0xD7,0xAC,0xE9,0x58,0x41,0x05,0x99,
    0xB2,0x96,0x33,0x76,0x49,0x4B,0xA9,0x62,0x5B,0x97,0x5A,
    0x64,0xD1,0x97,0x52,0x6C,0x95,0x57,0x0D,0x14,0x91,0x87,
    0x06,0x03,0x84,0x67,0x1E,0x24,0x70,0xF3,0xBA,0xE7,0xEC,
    0xFD,0xFB,0xCE,0x1F,0x37,0x86,0x20,0xF7,0x9E,0x73,0x6E,
    0xEE,0xCD,0x9A,0x7F,0xD8,0xFF,0x64,0xE5,0x9E,0x7D,0xEE,
    0xFE,0x9C,0xDF,0xEF,0xB7,0x7F,0xAF,0xB3,0xAF,0x22,0xC2,
    0x0C,0xAA,0x50,0xD3,0x06,0x31,0x74,0xB8,0x69,0x4A,0xC2,
    0x81,0x0E,0x19,0x40,0x6C,0xC8,0x44,0x10,0x06,0xC0,0xE2,
    0x8D,0x8B,0x17,0x62,0x88,0x44,0x10,0x06,0x40,0x63,0xDC,
    0xF4,0xA5,0x2F,0x2A,0x3B,0x24,0x00,0x2A,0xCC,0x83,0x51,
    0xF5,0x7C,0x22,0x6F,0x53,0xCE,0x90,0xA8,0xC1,0x5F,0x02,
    0x22,0x22,0x04,0x94,0xC9,0x5F,0x54,0xFF,0x0B,0xC8,0x10,
    0xAC,0xEF,0x2B,0x01,0x51,0x0A,0x00,0xC5,0x01,0xA8,0x66,
    0x6F,0x7B,0xA7,0x84,0x89,0xD9,0x59,0x15,0x84,0x8F,0x04,
    0xA8,0x79,0xA8,0x7E,0xCB,0x7B,0xC6,0x11,0x82,0xF8,0xEF,
    0x9E,0x65,0x10,0x28,0xA5,0x94,0x02,0xAD,0xCD,0xDE,0xB6,
    0x64,0xAA,0x21,0xDC,0x77,0x7D,0xAE,0x52,0x91,0x2B,0x7F,
    0x45,0xB1,0x34,0x7C,0xFE,0x1A,0x4B,0x1E,0x79,0xE7,0xFD,
    0xC3,0xCD,0x5D,0x89,0xEB,0x46,0x52,0xDE,0x9B,0xC6,0x48,
    0xA9,0x02,0xAA,0xEE,0x8F,0xB7,0x3C,0x50,0x13,0x39,0xFC,
    0xB3,0xED,0x5F,0x5F,0x32,0x82,0x8A,0xD2,0x36,0x12,0x6A,
    0xE7,0x5E,0x74,0x74,0xBB,0x2A,0x52,0x7C,0xC9,0xD8,0x6A,
    0x05,0xAB,0x33,0xD7,0x86,0x8F,0x0D,0xB8,0x4B,0xEA,0x3E,
    0x25,0x4A,0xB9,0x4F,0x3C,0x5C,0xB1,0xFA,0x5A,0xD1,0x67,
    0xF6,0x80,0xB4,0xB5,0xB6,0xEC,0x6B,0xEC,0x9D,0xF0,0xF9,
    0x4A,0x09,0xEB,0xC8,0x06,0x05,0x00,0xC0,0x02,0xD4,0x7A,
    0xDB,0xBC,0xDB,0x1F,0xB2,0x0E,0xA8,0x00,0x8A,0x82,0x4A,
    0x90,0x1C,0xFD,0xE3,0xEB,0x35,0x77,0x0D,0xB7,0xCE,0x10,
    0x02,0x58,0xA5,0x01,0x50,0x9C,0x9E,0x68,0xE4,0xAC,0x0B,
    0x04,0xA9,0x34,0xCC,0xEA,0x97,0xEE,0x9B,0x9A,0x31,0x41,
    0xB0,0x99,0x88,0x6B,0x93,0x7E,0x6C,0x2C,0x0F,0xCC,0x78,
    0x96,0x26,0x33,0x23,0x0C,0x06,0x10,0x61,0x2A,0x73,0xF7,
    0xD8,0x51,0xB7,0x81,0x49,0xF1,0xB2,0x07,0x20,0x6C,0x5D,
    0xDF,0x95,0x0A,0xC1,0x70,0xC7,0x75,0x29,0x2F,0x86,0x1B,
    0x41,0x56,0x4C,0xE5,0xFE,0xF3,0x8C,0xDD,0xA9,0x42,0xA1,
    0xE3,0x7D,0xFC,0xCA,0x95,0x19,0xBA,0xE8,0x00,0x40,0xC3,
    0x65,0x58,0x9A,0xFA,0x19,0x2D,0x77,0xDC,0x9E,0x99,0x04,
    0x02,0x00,0x84,0xA7,0x2E,0x9E,0xEE,0x3B,0xA3,0xF3,0x96,
    0xF6,0x8C,0x08,0x02,0x54,0x20,0x78,0xF1,0xE0,0x7D,0xF0,
    0xC9,0x04,0x58,0x98,0xD7,0x91,0x91,0x06,0x02,0x00,0x14,
    0x5E,0x98,0x50,0x97,0x72,0x92,0x10,0x84,0xEE,0xCC,0x28,
    0x59,0xF2,0x07,0xA0,0x3E,0xBD,0x75,0x56,0xD4,0xA6,0xF0,
    0xF8,0xD4,0x0A,0x0A,0x88,0x64,0x14,0x9F,0x03,0x00,0xF0,
    0xDE,0xA9,0xE9,0x29,0x2F,0xAA,0x9D,0x2D,0x00,0x4C,0x69,
    0x06,0xCB,0x07,0x03,0x1C,0x29,0xFA,0x68,0x8A,0x39,0x56,
    0xED,0xBE,0xF2,0x49,0xA8,0x13,0x43,0x0C,0x70,0xAC,0xAA,
    0x12,0x8A,0x49,0xAC,0x90,0x4E,0xFC,0xEE,0xBC,0x5B,0x81,
    0xF7,0x0B,0xF3,0x24,0x13,0x15,0x44,0x02,0xAE,0x77,0x54,
    0x41,0xB4,0x72,0xCE,0xC9,0x47,0x09,0x7B,0xE7,0xC6,0x35,
    0x97,0xBB,0x39,0x9B,0x2E,0xCB,0x2C,0x61,0xF7,0x97,0x80,
    0x42,0xEB,0x48,0x08,0xDE,0x7A,0xDA,0xFD,0xD0,0x2A,0xA4,
    0x9A,0xBF,0xFA,0xC9,0x5B,0x6C,0x14,0x5B,0xEB,0x32,0xCB,
    0x11,0x83,0x5C,0xB1,0x8E,0x42,0xB0,0xE6,0x9B,0xCF,0xC0,
    0x0C,0x74,0xB8,0x22,0x7A,0xE1,0xFF,0x2C,0xB9,0xD7,0x2A,
    0xF5,0x57,0x67,0x92,0x64,0x96,0x94,0xF8,0xBA,0x29,0x8F,
    0xCB,0x67,0xD2,0xB0,0xFB,0x7A,0x3C,0x45,0x31,0x7D,0x49,
    0xA0,0x58,0x43,0x2E,0xC0,0x33,0x34,0x34,0xFC,0xEA,0x2B,
    0x19,0xC6,0x63,0x7F,0x00,0xCB,0x0D,0xB5,0xBD,0xB4,0x6C,
    0x99,0x85,0x2F,0x37,0x50,0x48,0x6B,0x8C,0xA5,0xF0,0xC8,
    0x8C,0x61,0x7F,0xA2,0x47,0x8F,0xCF,0xDD,0x96,0x61,0x34,
    0x0E,0x00,0x10,0x7A,0x93,0xD6,0xD2,0x88,0xD8,0x27,0x8A,
    0xF2,0xBF,0xBD,0x2D,0x4E,0x92,0xF4,0xEA,0x17,0x14,0xD6,
    0x36,0xD0,0xA3,0xC7,0xBD,0x53,0x0F,0x67,0x16,0x8A,0x02,
    0x83,0x91,0xE1,0x8A,0x2B,0x2C,0xAD,0x58,0x36,0xDC,0x55,
    0xA4,0xC6,0x4E,0x9D,0x33,0xEF,0x8E,0x2F,0x8E,0xD7,0x25,
    0xFF,0xD1,0x43,0x63,0x3D,0x36,0x4C,0xFD,0x6B,0xA6,0x09,
    0x51,0xEA,0xB4,0xFC,0x03,0x13,0x51,0x73,0x9C,0x5F,0x53,
    0x34,0x95,0x3A,0xB4,0xFE,0xCD,0xF7,0x4F,0x19,0x46,0x3E,
    0x72,0xDD,0xEC,0x0A,0x21,0x94,0xDE,0xF0,0xC8,0xA2,0x6B,
    0x86,0x34,0x29,0x4D,0x00,0xC4,0x6F,0xB6,0xCF,0x96,0x08,
    0xA9,0x1C,0x00,0x14,0xA5,0x14,0xE0,0x3A,0x5A,0xB5,0x2C,
    0x6F,0xFC,0xCF,0xEA,0x8C,0xD7,0x0F,0x4E,0x4A,0x45,0xF8,
    0x70,0xED,0x2A,0x97,0x42,0x13,0x8F,0x1B,0xA1,0x58,0xCF,
    0x92,0x6C,0x7C,0xEC,0xB6,0xD5,0x22,0x29,0xB2,0xD5,0x74,
    0xD4,0x12,0x5C,0x9E,0x53,0x9C,0xB7,0x97,0xB6,0x7D,0x7A,
    0xDA,0x65,0x45,0x00,0xE4,0xF8,0x68,0x07,0xEE,0xFE,0xBF,
    0xEF,0xE8,0xFA,0xCC,0x17,0x0A,0x45,0x25,0x71,0x41,0x94,
    0xF4,0xCA,0xA5,0x30,0xFD,0x01,0x51,0xEA,0xC0,0xEF,0xFF,
    0xDE,0x3D,0xAC,0x62,0x74,0xF9,0xFB,0x4B,0xBE,0x79,0xC1,
    0x21,0x37,0xB7,0xE6,0xEA,0xF1,0x2A,0x69,0x32,0x43,0x2A,
    0xD5,0xFA,0xDB,0x0B,0xA6,0x45,0x43,0x03,0x04,0xC5,0x02,
    0x00,0xD0,0xC2,0xEA,0x6F,0xA1,0xEB,0xE0,0xC1,0x96,0xD3,
    0x6F,0x9E,0x2E,0xAB,0xB9,0x76,0x4C,0x21,0x40,0xAB,0x93,
    0xAC,0x6F,0xB5,0x8E,0xBD,0xBA,0x78,0x53,0xF5,0xA6,0x8A,
    0xF0,0xCD,0x8C,0x70,0x9A,0x12,0x63,0x49,0x1A,0x3E,0x59,
    0xD8,0x48,0x4B,0x7B,0x56,0x61,0x2C,0x22,0x62,0x13,0x83,
    0xBD,0xEB,0x3E,0xA1,0x47,0x14,0xCE,0xEE,0x0D,0xEF,0x1D,
    0xC2,0xB6,0xE9,0x1C,0x4D,0x8A,0x45,0x54,0x03,0xA4,0x76,
    0x14,0x48,0x11,0x6B,0xAD,0xB1,0x54,0x4A,0x29,0xAD,0xB5,
    0xD6,0x9A,0x2B,0xAF,0xBD,0x75,0xD7,0x94,0xFA,0x7B,0x5E,
    0xBC,0xB1,0x39,0x54,0xEB,0x07,0x08,0xA7,0x82,0x3E,0x06,
    0x28,0x07,0xC3,0x73,0xF2,0xC0,0x44,0x9F,0xE2,0x4C,0x10,
    0xF4,0xBC,0x9E,0xD6,0x1E,0x2A,0x53,0xB4,0x6E,0x41,0x49,
    0x91,0x3D,0xDC,0xF4,0x48,0xCD,0xBF,0xFC,0xE0,0xE9,0xB0,
    0x3A,0x08,0x0F,0x00,0xA1,0x71,0x0A,0xA3,0x85,0x70,0x14,
    0x00,0x7B,0xAA,0x27,0x76,0xF2,0xE4,0xE1,0x93,0xC7,0x5A,
    0x8E,0xC5,0x62,0xC6,0xEB,0x74,0x69,0x73,0xF3,0xBC,0x0A,
    0x58,0xE7,0xD0,0x8F,0xD6,0xDD,0xB9,0xEB,0x27,0x77,0xD7,
    0x86,0x0C,0x92,0x61,0x01,0x84,0x8E,0x06,0x10,0xE5,0x46,
    0x1E,0x6F,0xDF,0x7F,0xE2,0xF8,0xA9,0x18,0x75,0x5E,0x59,
    0x79,0x59,0xF9,0xD8,0x4F,0x96,0x15,0x14,0xE7,0xC3,0x98,
    0x9C,0xE7,0x7F,0x55,0x1E,0x53,0x88,0x96,0x74,0x03,0xF3,
    0x96,0xFF,0xAD,0x36,0xA4,0x0E,0xC2,0x01,0x90,0x1A,0xA6,
    0x61,0xF3,0xBB,0xEF,0xED,0x91,0xD9,0x56,0x17,0x5D,0x38,
    0x76,0xCA,0xB8,0x0B,0xCA,0x2A,0x2A,0x0A,0x07,0x12,0xEA,
    0xC8,0x2F,0x63,0xDA,0x29,0x54,0x4D,0xA3,0x01,0xED,0xE4,
    0x85,0x7C,0xB0,0x50,0x00,0x42,0xAD,0x8F,0xAC,0x5A,0xB3,
    0xDB,0x96,0x57,0x75,0xC5,0xE7,0xDF,0xE1,0x8C,0x18,0x11,
    0xE9,0xBF,0x42,0x28,0x24,0xBC,0x89,0x77,0xED,0xB2,0xFB,
    0x23,0x6E,0xBC,0xF0,0xCB,0x4F,0x02,0xBD,0xB6,0x20,0x2C,
    0x40,0x88,0xF2,0xDC,0x0A,0x1B,0xE7,0x17,0x17,0xDF,0xB8,
    0x62,0x6B,0x1B,0x37,0xD5,0xB4,0xF6,0xA5,0x05,0xC6,0xCA,
    0x87,0xB6,0x9A,0x70,0xD7,0x96,0xCD,0x6F,0xB5,0x90,0x96,
    0x3B,0x23,0x2B,0xC3,0x86,0xC9,0x40,0x09,0x08,0x74,0xE3,
    0x23,0x6B,0x26,0x3C,0x3E,0xB3,0x12,0x10,0x0C,0xCB,0x75,
    0x20,0x5A,0x25,0xB5,0x2F,0x25,0x13,0x01,0x40,0x14,0xD0,
    0x63,0x43,0x1B,0x77,0xD0,0x44,0xD1,0x1D,0x8F,0x2E,0x1D,
    0xFB,0xEC,0x4C,0x40,0xA8,0x08,0x58,0x41,0x4A,0x5F,0xAF,
    0x05,0x80,0xD2,0x20,0x04,0x39,0x59,0x02,0x10,0xBD,0xE3,
    0xB6,0x3D,0x3F,0x7C,0x30,0x47,0xA8,0x35,0x20,0x01,0xEF,
    0x0D,0xFA,0x04,0x43,0x58,0x86,0x4E,0xD5,0xFD,0x01,0x44,
    0xEF,0xFD,0x5C,0xEE,0x5F,0xA6,0xD8,0xFE,0xB0,0xAF,0xC3,
    0x75,0x23,0xDA,0x91,0xB5,0x5D,0xF0,0x70,0xCF,0x9F,0x27,
    0x18,0xA7,0x5F,0xE8,0x3A,0x8C,0x8B,0x55,0x18,0xA5,0xBB,
    0xC3,0x02,0xF8,0xBA,0x2B,0xEA,0x53,0x6F,0x7E,0x65,0x82,
    0x1B,0x39,0xA3,0x74,0x47,0x87,0x92,0x6D,0xAE,0xEA,0xCA,
    0x0A,0x00,0x90,0x5F,0x70,0x08,0x91,0x81,0x6B,0x86,0xF1,
    0xAF,0x0A,0xA3,0x4B,0x5E,0x1F,0x58,0x2F,0xD1,0xA7,0xB7,
    0xED,0xFB,0x85,0xCA,0xE6,0xDC,0xB7,0x7E,0x91,0xE6,0x19,
    0xBD,0x33,0x4C,0x21,0xAA,0x6C,0xC5,0x57,0xD7,0xEC,0xD1,
    0x67,0xEE,0x52,0x8E,0x56,0x36,0x65,0x53,0xDA,0xDF,0x0B,
    0xF1,0x01,0xFC,0x1B,0xA5,0xCF,0xA9,0x58,0xBE,0x3B,0xFE,
    0x44,0x88,0x50,0x6F,0xB9,0xCB,0xF9,0x71,0xBF,0x27,0x12,
    0x36,0xAF,0xDD,0xDC,0x4D,0x9B,0xBC,0x82,0x09,0xF2,0x84,
    0x96,0xCF,0x44,0x6E,0xE8,0xA0,0xB1,0x89,0x7F,0xF6,0xD6,
    0x1C,0x0D,0x95,0x6B,0x98,0xCB,0x67,0xF5,0x97,0x4C,0x86,
    0xF3,0x11,0x1D,0xBB,0xC2,0x4D,0x5E,0x43,0x05,0xE9,0x54,
    0xD9,0x3B,0xB6,0x1F,0x9F,0xB8,0xDE,0x51,0x92,0x90,0x68,
    0x4F,0x2C,0x4C,0x35,0x2E,0xCE,0xB8,0x03,0xFD,0xDA,0xD5,
    0xB8,0x67,0xC3,0x0B,0x93,0xE6,0xDF,0xD8,0xA4,0x93,0x6E,
    0xE1,0xE0,0x87,0xA1,0xF7,0xA3,0xE2,0x19,0xDB,0x29,0xC6,
    0x5A,0xBE,0x3B,0x7C,0x4B,0x98,0x62,0xD0,0xF0,0xDB,0xA3,
    0xDB,0x38,0xA0,0x96,0x15,0xBE,0x34,0xBC,0xFA,0xED,0x64,
    0xB7,0x06,0x5B,0xB5,0x23,0xEA,0xFB,0xF5,0xD1,0x4F,0xDD,
    0xBA,0xCF,0x51,0x02,0xE5,0x85,0xEC,0x89,0x55,0x77,0x76,
    0xF5,0x4D,0x54,0xDA,0xB1,0x9E,0xD8,0x59,0x6F,0xAA,0x59,
    0x47,0x93,0xC8,0x20,0xC4,0xB6,0xD2,0xDA,0xD6,0xBC,0xFC,
    0xCA,0xF6,0x49,0xF7,0xB7,0x44,0xA0,0xED,0xC9,0x30,0xCB,
    0x03,0xBA,0x6F,0xE3,0x11,0xA7,0xE7,0xFD,0x54,0xA2,0xD0,
    0x66,0xC2,0xDA,0x96,0x6F,0x25,0xEB,0x65,0x04,0xCB,0x93,
    0xA4,0x35,0x8C,0x2D,0xD0,0x55,0x2F,0x70,0x7F,0xFE,0xCF,
    0xE9,0x85,0x51,0xC1,0x43,0xC3,0x8E,0x50,0x48,0x0A,0xDB,
    0x3F,0x8B,0xAB,0x37,0xD3,0xD2,0xE3,0x2A,0x3C,0x75,0x6E,
    0x90,0x0E,0x99,0x96,0xD3,0x5A,0x6E,0x9B,0x8C,0xDF,0x76,
    0x0E,0x5F,0x16,0x26,0xD0,0x1B,0x2E,0x2E,0x6C,0xFA,0x40,
    0xE3,0xB2,0x6E,0x3C,0x16,0x59,0x11,0xCB,0x9B,0xCA,0x8F,
    0x9C,0xB3,0x87,0xC2,0x02,0x88,0x67,0xD9,0x3E,0xA5,0xEC,
    0xB5,0xCB,0xBE,0x17,0x0E,0xE0,0xEE,0xE1,0xC7,0x12,0x6B,
    0x89,0x67,0x18,0x5B,0x88,0xBB,0x29,0x96,0x6F,0x47,0x17,
    0x9D,0x73,0x77,0x48,0x00,0x2B,0xEC,0x58,0x7D,0xDB,0x18,
    0x4C,0x1A,0x77,0x6F,0x38,0x80,0x07,0x8B,0x9B,0xFA,0x1F,
    0xD6,0x78,0x7C,0x1C,0xAB,0x69,0x2C,0xE7,0x4C,0xB4,0x1F,
    0x16,0x41,0xB8,0xDC,0x99,0xBA,0xE3,0xD1,0x49,0x73,0x37,
    0x7D,0xEE,0x91,0x07,0x73,0xDD,0x50,0xF3,0x31,0x39,0xB6,
    0x1D,0x02,0x10,0xCD,0x5F,0xFB,0xB9,0x71,0xEC,0x77,0xA7,
    0xFE,0x97,0xE7,0x08,0xA6,0x1C,0x3D,0x31,0x28,0x23,0xB4,
    0x7C,0x7E,0x34,0x6E,0xF8,0x5D,0x2F,0xC9,0x49,0x73,0x83,
    0x25,0x20,0x22,0x3C,0x55,0x79,0x13,0x2D,0xAD,0xCB,0xA6,
    0x2B,0xF0,0xC9,0xBD,0xE4,0x6F,0x9C,0x4D,0x8C,0xF3,0xB1,
    0x68,0xD3,0x87,0x7D,0x41,0x28,0x09,0x10,0xEF,0x7C,0xE4,
    0x8D,0x75,0xD7,0xE7,0x5A,0x83,0x82,0xB6,0xC0,0xB6,0xA0,
    0x40,0x1D,0x5B,0xF6,0xC7,0x79,0xEB,0x0F,0x6A,0xA3,0xA3,
    0xAC,0x7A,0xEB,0x0F,0x4D,0x73,0xDA,0x70,0x85,0xDD,0x0A,
    0xA0,0x7A,0xDA,0x88,0x73,0x6E,0x0F,0x23,0x01,0x52,0x84,
    0xD6,0x08,0x2D,0xA7,0x4F,0x09,0x8A,0x05,0x56,0x7A,0x37,
    0x5E,0x84,0xF1,0x2B,0x23,0x7F,0x21,0xF7,0xBC,0x7C,0x88,
    0x2E,0xB7,0x44,0x1E,0xA4,0xB7,0xB8,0x9E,0x96,0x49,0x3A,
    0x1A,0xA1,0x01,0x2C,0x49,0x1A,0xCE,0xAD,0xE9,0xF1,0x27,
    0x10,0xAE,0xB8,0x30,0x7F,0xF2,0xE2,0xAA,0x1F,0xE7,0xBE,
    0xC8,0x07,0x72,0x31,0xEA,0x39,0xC6,0x39,0x77,0x64,0x77,
    0xDF,0x96,0x38,0xF7,0xD6,0xD0,0xDB,0x90,0x09,0x80,0x7F,
    0x2F,0x3D,0xEE,0x0B,0x60,0xB9,0x14,0x33,0xBF,0x94,0x7B,
    0x43,0xC5,0xCD,0xC5,0x6B,0x9F,0xC6,0xBF,0xAE,0xFB,0x47,
    0xAC,0x22,0x77,0x2C,0x15,0x26,0x2A,0xFA,0xC1,0x03,0xB0,
    0x0F,0x60,0x95,0xB3,0xDB,0x2F,0x1A,0x59,0xEE,0xC5,0x2D,
    0x1D,0x5C,0x11,0x19,0x71,0x51,0xF9,0x57,0xC6,0x5C,0x36,
    0xEF,0x0B,0x3B,0xEB,0xD0,0xE8,0x1B,0xBE,0xD2,0xED,0xF3,
    0x5E,0x60,0x0F,0xFA,0x45,0x23,0x85,0xB2,0x87,0xA6,0xD7,
    0x8E,0x2D,0xBD,0xDF,0xF4,0x3A,0xEB,0x7A,0x8F,0x6C,0xDE,
    0xF8,0x9D,0xA7,0xFE,0x77,0x14,0xFC,0x5E,0x3A,0xA5,0x27,
    0x01,0xCB,0x9D,0x51,0xFF,0xA2,0x4B,0x78,0xF4,0xC2,0x8F,
    0xCE,0x8C,0xD4,0x5D,0x5E,0x36,0xBA,0xB2,0xF4,0xE6,0xD7,
    0xBE,0x81,0x3B,0x03,0xC2,0x77,0x1A,0xFD,0x01,0x00,0x40,
    0x51,0xF4,0x84,0x6F,0x3C,0xA6,0xBA,0xF7,0xF8,0xAB,0xAD,
    0x8D,0x3B,0x25,0x4A,0x1B,0xDD,0xF6,0x6A,0x6F,0xF9,0x2B,
    0xFB,0x6B,0x7C,0x7B,0x15,0xE9,0xA9,0x40,0x61,0x58,0x5E,
    0x8F,0xDF,0x04,0xAB,0x5F,0xFA,0xCD,0xE2,0xF2,0x2F,0xED,
    0x29,0x72,0x08,0x44,0x5B,0x3F,0xFD,0xB7,0xE5,0xCD,0xAF,
    0xFB,0xD7,0x32,0xE9,0xDA,0x40,0x41,0x8E,0x5F,0xC6,0x4F,
    0xE7,0xE8,0x5D,0x1F,0xFB,0xCE,0xF7,0xBA,0x1E,0xF0,0xDC,
    0x68,0x24,0xD2,0x6D,0x9B,0x2F,0xBD,0xA9,0x72,0x43,0x80,
    0xE7,0x4A,0xCB,0x06,0x84,0x66,0xDC,0x5C,0x1F,0xA5,0x5A,
    0xD6,0x57,0xBF,0xB1,0x0F,0xDF,0xE5,0xA6,0xCA,0x68,0xAE,
    0x33,0xED,0x8B,0x95,0xC7,0xF8,0x75,0xBC,0xE7,0x6B,0x05,
    0xE9,0xDA,0x80,0x53,0x7A,0xDA,0xE7,0xAA,0x46,0xED,0x9E,
    0xBC,0x59,0x39,0xF7,0xE0,0xEA,0xD7,0xEA,0xBB,0xCB,0xE6,
    0x3C,0xB5,0xF6,0x58,0xC5,0xF7,0x2F,0x29,0xF7,0x15,0x41,
    0x7A,0x00,0x8A,0x2A,0xBF,0xDB,0x5F,0xA4,0x39,0x98,0xF2,
    0x4F,0xD5,0xA2,0x26,0x4C,0x80,0xE0,0x33,0xF6,0x67,0xB5,
    0x97,0x3C,0xEC,0x7F,0xF0,0x28,0x5D,0x09,0x20,0x28,0x27,
    0x55,0x58,0x08,0x6A,0x88,0x28,0xF0,0x63,0x0B,0x2A,0xA1,
    0x02,0x1A,0xEA,0x69,0x02,0x78,0x5B,0x4F,0x47,0xEC,0xB9,
    0x2F,0xF1,0xCE,0x02,0x10,0x68,0x20,0xD1,0xC8,0x5D,0x02,
    0x04,0xF5,0xF3,0xD3,0xDA,0x05,0x82,0x2D,0xD7,0xC5,0x0F,
    0xEC,0x0A,0x10,0xC2,0x80,0x26,0x72,0x88,0xF3,0x56,0x69,
    0x01,0x10,0x27,0x9D,0x1F,0xC8,0x6B,0xE1,0xDF,0x54,0x86,
    0xE8,0xDC,0xA7,0xE9,0x07,0x44,0xDF,0x30,0x6D,0x7D,0x56,
    0x4F,0xB3,0xA5,0x09,0x60,0x4B,0xCB,0xEE,0xDC,0x7D,0x32,
    0x5C,0x97,0x62,0x48,0x00,0xDC,0xD1,0xA8,0x1B,0xBE,0x39,
    0x9B,0x27,0x0B,0xD3,0x04,0x88,0x8D,0x41,0x64,0xC6,0xAB,
    0xD9,0x3C,0x5F,0x9A,0x26,0x40,0xD7,0x18,0x60,0xF6,0xBE,
    0x41,0x78,0x8F,0x6C,0x01,0xC4,0x2B,0x20,0x57,0x45,0x0F,
    0x65,0x51,0x04,0xE9,0xDA,0xC0,0x48,0x98,0x48,0x4D,0x7D,
    0x16,0x8D,0x20,0x4D,0x3F,0x20,0x23,0x00,0x4C,0xDE,0x9D,
    0xB5,0xE5,0xD3,0x04,0x50,0x70,0x8B,0xA0,0x51,0x7B,0x38,
    0x8B,0x9E,0x20,0x4D,0x15,0x74,0x0F,0x83,0xC6,0x45,0x6E,
    0x2C,0xE3,0x83,0x94,0x83,0x03,0x50,0xF0,0xF2,0x00,0x29,
    0x2C,0x3A,0x90,0x3D,0x23,0x48,0x13,0x80,0xF9,0x50,0xC4,
    0xA8,0xA6,0x6C,0x2D,0x9F,0x2E,0x80,0x61,0x09,0x00,0x54,
    0xF9,0xD6,0x06,0x43,0x07,0x40,0xC4,0x6D,0x01,0x94,0x42,
    0xD5,0x91,0x6C,0x2D,0x9F,0xAE,0x11,0x7A,0xD1,0x5C,0x00,
    0x18,0x15,0x5C,0xA3,0x0F,0x15,0x40,0x4E,0x94,0x50,0x28,
    0x69,0xB7,0x59,0xDB,0x06,0xE9,0x7D,0x51,0x3C,0xA1,0xFA,
    0x32,0xB7,0x2B,0x6B,0x46,0x30,0x08,0x00,0x85,0x02,0xB7,
    0x3D,0x4B,0xCB,0xA7,0x0B,0xE0,0x2A,0x10,0x40,0x14,0xA1,
    0x5F,0x88,0x64,0x15,0x80,0xE8,0x55,0x89,0xBF,0x39,0xA1,
    0x7A,0xE6,0x59,0x07,0x00,0x3A,0xFB,0x00,0x9C,0xFF,0x1F,
    0x09,0x00,0x5D,0x09,0x15,0x80,0xF1,0x81,0x9F,0x86,0x7F,
    0x49,0x98,0x64,0xA4,0x93,0xDB,0x10,0x6E,0xDF,0xF6,0xB7,
    0xF2,0x81,0x0A,0x48,0xD1,0x1A,0x49,0xCF,0xD3,0x64,0x1F,
    0x00,0xE8,0x35,0x00,0xA0,0x10,0x93,0xC4,0x11,0x7B,0x68,
    0xA5,0x34,0xC4,0x44,0x07,0xEF,0x97,0x52,0x03,0xF4,0x57,
    0x35,0x54,0x40,0xDF,0xAF,0x0A,0x3A,0x5C,0x00,0x50,0xD2,
    0x66,0x00,0x05,0x05,0xF0,0xD8,0x3B,0x1B,0xB7,0x35,0xC4,
    0xEA,0x96,0x97,0x0F,0xF6,0x37,0x40,0xA9,0x00,0xCE,0x6D,
    0x5F,0x09,0xAD,0xD3,0xD1,0x09,0x80,0xAA,0xED,0xB0,0x07,
    0x98,0xB6,0xC6,0x86,0x5D,0x07,0x5B,0x59,0x30,0x72,0xB2,
    0x7A,0xAC,0x74,0x85,0x0C,0xF2,0x50,0x57,0x2A,0x00,0xA5,
    0xF6,0xBF,0xB1,0xB7,0xB9,0x9B,0x5A,0xE7,0x94,0x16,0x17,
    0x54,0x54,0x8D,0xB9,0xB0,0x10,0xC0,0xE1,0x18,0x14,0xA8,
    0x9A,0xCC,0x33,0xBF,0x6B,0x68,0x6B,0x6E,0xB7,0x05,0x97,
    0x8E,0x29,0x2D,0x94,0x53,0x79,0x23,0x36,0x7A,0xD1,0x41,
    0x8A,0x20,0x39,0x00,0x95,0x77,0xFF,0xCA,0xE2,0x6B,0x26,
    0x5E,0x54,0x14,0x81,0xE9,0x39,0x7E,0x62,0x5B,0xE3,0x09,
    0x8E,0xFB,0xEC,0x3F,0xD4,0x9D,0xEC,0x01,0xA8,0xDD,0x6F,
    0x74,0x6E,0x6F,0x28,0xAF,0x1A,0x9F,0xE7,0x98,0xEE,0xAE,
    0xB6,0x5E,0xCF,0x48,0xC9,0x4D,0x83,0x5D,0x3F,0x45,0x8B,
    0xC6,0x70,0x1D,0x16,0x0E,0xE8,0x6B,0x0A,0x63,0x2F,0x7F,
    0x6D,0xDC,0xB0,0x89,0x95,0x57,0x91,0x42,0xCE,0x7F,0xBC,
    0x25,0x7E,0x56,0x67,0xC6,0x1D,0xFC,0xA9,0xC6,0xE4,0x6F,
    0xC3,0x45,0xEF,0x9A,0x56,0x70,0xD5,0x25,0x17,0x8F,0xCA,
    0x8F,0x44,0x5C,0x13,0xEF,0x70,0xB5,0x13,0xD1,0xCD,0x7F,
    0x30,0x47,0x6F,0x5F,0xD4,0x7F,0x3A,0x87,0x60,0x9F,0x25,
    0x22,0x93,0x1F,0x44,0xA6,0x78,0x1D,0x4F,0xF5,0xD6,0x4F,
    0xF6,0x76,0x32,0x92,0x17,0x8D,0x9A,0x9E,0x78,0x67,0x7B,
    0x1C,0x44,0x9E,0xBB,0x7A,0xB6,0x17,0x05,0x00,0x81,0x1A,
    0xD0,0x77,0x49,0x04,0xA8,0x2C,0x03,0x9C,0xF5,0x48,0xEC,
    0x3D,0xED,0x42,0x91,0xA6,0x2A,0x2F,0xC3,0x23,0xBC,0x69,
    0x00,0x40,0x98,0xA4,0xBB,0x30,0x04,0x3F,0xF8,0x0B,0x73,
    0x22,0x82,0x1F,0x74,0xA6,0xB2,0xFE,0xF8,0x21,0x01,0x86,
    0x74,0x0C,0xC5,0x43,0x9D,0x07,0x38,0x0F,0x70,0x1E,0xE0,
    0x3C,0xC0,0x79,0x80,0xF3,0x00,0xE7,0x01,0xD2,0x1A,0xFF,
    0x07,0x79,0xEE,0x90,0xB2,0x4F,0x70,0xB3,0x6E,0x00,0x00,
    0x00,0x00,0x49,0x45,0x4E,0x44,0xAE,0x42,0x60,0x82,
};

// Pin setting for arduino...
unsigned char WLAN_CS = 4;
unsigned char WLAN_EN = 7;
unsigned char WLAN_IRQ = 2;
unsigned char WLAN_IRQ_INTNUM = 0;


#define INDICATE_LED        13

#define RELAY_PIN           A0

#define SMART_CONFIG_PIN    8

unsigned char Relay_Status = 0;

char AP_Ssid[] = {"..."};
char AP_Pass[] = {"..."};

void Initial_Spider(void){
    int ret = 0;
    unsigned long init_timer = 0;

    while(1){
            /* Initial Spider L3 spi interface*/
        Serial.print(F("Starting Spider L3..."));

        Spider_begin();

        Serial.println(F("ok"));

        // Spider L3 SPI interface initial success, indicated LED signal.
        digitalWrite(INDICATE_LED, HIGH); 
        delay(100);
        digitalWrite(INDICATE_LED, LOW);

        /* Connect to WiFi AP */
        Serial.print(F("Connect to "));
        Serial.write((unsigned char*)AP_Ssid, strlen(AP_Ssid));
        Serial.print(F(" access point..."));

        Spider_Connect(3, (char*)AP_Ssid, (char*)AP_Pass);

        Serial.println(F("ok"));
        digitalWrite(INDICATE_LED, HIGH); 
        delay(100);
        digitalWrite(INDICATE_LED, LOW);

        Serial.print(F("Waiting DHCP..."));

        init_timer = millis() + 20000;
        while(init_timer > millis()){
            /* wait connection and Get DHCP address finished */
            if((Spider_CheckDHCP() == 0) && (Spider_CheckConnected() == 0)) break;
        }
        
        if((Spider_CheckDHCP() == 0) && (Spider_CheckConnected() == 0)){
            Serial.println(F("ok"));
            break;
        }
        else{
            Serial.println(F("Timeout."));
        }

        Spider_close();
        delay(1000);
    }

    // Spider L3 connect success, indicated LED signal.
    digitalWrite(INDICATE_LED, HIGH); 
    delay(100);
    digitalWrite(INDICATE_LED, LOW);

    /* show system IP address */
    tNetappIpconfigRetArgs inf;
    netapp_ipconfig(&inf);

    Serial.print(F("Device's IP address:"));
    Serial.print(inf.aucIP[3] ,DEC);
    Serial.print(F("."));
    Serial.print(inf.aucIP[2] ,DEC);
    Serial.print(F("."));
    Serial.print(inf.aucIP[1] ,DEC);
    Serial.print(F("."));
    Serial.print(inf.aucIP[0] ,DEC);
    Serial.print(F("\r\n"));
}

void setup() {          
    /* initial uart debug output interface. */
    Serial.begin(115200);
    //while(!Serial){}

    Serial.println(F("=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+"));
    Serial.println(F("        Spider L3 Practice 4 AJAX       "));
    Serial.println(F("=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+"));

    /* initial status LED pin */
    pinMode(INDICATE_LED, OUTPUT);
    digitalWrite(INDICATE_LED, LOW);

    // Initial Relay pin
    digitalWrite(RELAY_PIN, HIGH);
    pinMode(RELAY_PIN, OUTPUT);

    Relay_Status = LOW;

}

int http_server_socket = -1;

#define WAIT_CONNECT            0x02
#define CLOSE_CLIENT_CONN       0x05
unsigned char sw_app_st = WAIT_CONNECT;

int Init_SwitchServer(void){
    sw_app_st = WAIT_CONNECT;
    http_server_socket = -1;
    http_server_socket = WebServer_begin(80);
    if(http_server_socket >= 0) return 0;
    return -1;
}

int Switch_WebService(void){
    static unsigned long task_timer;

    int ret_st = 0;
    static int client_socket = -1;

    char method[7];
    char file[30];
    char content[100];
    char auth;

    unsigned long i;
    unsigned long j;

    if(millis() > task_timer){
        switch(sw_app_st){
            case WAIT_CONNECT:
            client_socket = -1;
            client_socket = WebServer_wait_connect(http_server_socket);
            if(client_socket < 0){
                task_timer = millis() + 250;
                return 0;
            }

            Serial.println(F("Incomming connection from client, Get client message."));
            memset(file, 0, sizeof(file));
            memset(content, 0, sizeof(content));
            memset(method, 0, sizeof(method));
            ret_st = WebServer_process_request(client_socket, method, sizeof(method), 
                                               file, sizeof(file), content, sizeof(content), &auth, 0);
            if(ret_st < 0) return -1;

            Serial.println("Method:");
            Serial.write((unsigned char*)method, strlen(method));
            Serial.println("");
            Serial.println("File:");
            Serial.write((unsigned char*)file, strlen(file));
            Serial.println("");
            Serial.println("Content:");
            Serial.write((unsigned char*)content, strlen(content));
            Serial.println("");

            // Check Get method.
            if(strstr_P(method, PSTR("GET")) != 0){

                // Check CGI parameter.
                if(strstr_P(file, PSTR("?")) == 0){

                    // Get files.
                    // If chrome send favicon request....
                    if((strstr_P(file, PSTR("logo")) != 0) || (strstr_P(file, PSTR("favicon")) != 0)){
                        
                        memset(content, 0, sizeof(content));
                        strncpy_P(content, PSTR("HTTP/1.1 200 OK\r\n"), sizeof(content));
                        ret_st = WebServer_put_response(client_socket, content, strlen(content));
                        if(ret_st < 0) return -1;

                        memset(content, 0, sizeof(content));
                        strncpy_P(content, PSTR("Content-type: image/png\r\n"), sizeof(content));
                        ret_st = WebServer_put_response(client_socket, content, strlen(content));
                        if(ret_st < 0) return -1;

                        memset(content, 0, sizeof(content));
                        strncpy_P(content, PSTR("Connection: close\r\n\r\n"), sizeof(content));
                        ret_st = WebServer_put_response(client_socket, content, strlen(content));
                        if(ret_st < 0) return -1;

                        i = 0;
                        j = sizeof(data_Logo_png);

                        while(1){
                            
                            if(j > i){
                                if(j > (i + sizeof(content))){
                                    memcpy_P(content, &data_Logo_png[i], sizeof(content));
                                    ret_st = WebServer_put_response(client_socket, content, sizeof(content));
                                    if(ret_st < 0) return -1;
                                    i += sizeof(content);
                                }
                                else{
                                    memcpy_P(content, &data_Logo_png[i], (j - i));
                                    ret_st = WebServer_put_response(client_socket, content, (j - i));
                                    if(ret_st < 0) return -1;
                                    i += sizeof(content);
                                }
                            }
                            else{
                                break;
                            }
                        }
                    }

                    // If send Index.html request.
                    if((strstr_P(file, PSTR("/")) != 0) || (strstr_P(file, PSTR("index.html")) != 0)){
                        
                        memset(content, 0, sizeof(content));
                        strncpy_P(content, PSTR("HTTP/1.1 200 OK\r\n"), sizeof(content));
                        ret_st = WebServer_put_response(client_socket, content, strlen(content));
                        if(ret_st < 0) return -1;

                        memset(content, 0, sizeof(content));
                        strncpy_P(content, PSTR("Content-Type: text/html\r\n"), sizeof(content));
                        ret_st = WebServer_put_response(client_socket, content, strlen(content));
                        if(ret_st < 0) return -1;

                        memset(content, 0, sizeof(content));
                        strncpy_P(content, PSTR("Connection: close\r\n\r\n"), sizeof(content));
                        ret_st = WebServer_put_response(client_socket, content, strlen(content));
                        if(ret_st < 0) return -1;

                        i = 0;
                        j = sizeof(index_data);

                        while(1){
                            
                            if(j > i){
                                if(j > (i + sizeof(content))){
                                    memcpy_P(content, &index_data[i], sizeof(content));
                                    ret_st = WebServer_put_response(client_socket, content, sizeof(content));
                                    if(ret_st < 0) return -1;
                                    i += sizeof(content);
                                }
                                else{
                                    memcpy_P(content, &index_data[i], (j - i));
                                    ret_st = WebServer_put_response(client_socket, content, (j - i));
                                    if(ret_st < 0) return -1;
                                    i += sizeof(content);
                                }
                            }
                            else{
                                break;
                            }
                        }
                    }
                }
                else{
                    // Get with CGI parameter
                }
            }

            // Check POST method.
            if(strstr_P(method, PSTR("POST")) != 0){

                memset(content, 0, sizeof(content));
                strncpy_P(content, PSTR("HTTP/1.1 200 OK\r\n"), sizeof(content));
                ret_st = WebServer_put_response(client_socket, content, strlen(content));
                if(ret_st < 0) return -1;

                memset(content, 0, sizeof(content));
                strncpy_P(content, PSTR("Content-Type: text/html\r\n"), sizeof(content));
                ret_st = WebServer_put_response(client_socket, content, strlen(content));
                if(ret_st < 0) return -1;

                memset(content, 0, sizeof(content));
                strncpy_P(content, PSTR("Connection: close\r\n\r\n"), sizeof(content));
                ret_st = WebServer_put_response(client_socket, content, strlen(content));
                if(ret_st < 0) return -1;

                if(strstr_P(file, PSTR("?SW=ON")) != 0){
                    Relay_Status = HIGH;
                    memset(content, 0, sizeof(content));
                    snprintf(content, sizeof(content), "Switch ON");
                    ret_st = WebServer_put_response(client_socket, content, strlen(content));
                    if(ret_st < 0) return -1;
                }

                if(strstr_P(file, PSTR("?SW=OFF")) != 0){
                    Relay_Status = LOW;
                    memset(content, 0, sizeof(content));
                    snprintf(content, sizeof(content), "Switch OFF");
                    ret_st = WebServer_put_response(client_socket, content, strlen(content));
                    if(ret_st < 0) return -1;
                }

                if(strstr_P(file, PSTR("?Onload")) != 0){
                    memset(content, 0, sizeof(content));
                    if(Relay_Status == HIGH){
                        snprintf(content, sizeof(content), "Switch ON");
                    }
                    else if(Relay_Status == LOW){
                        snprintf(content, sizeof(content), "Switch OFF");
                    }
                    ret_st = WebServer_put_response(client_socket, content, strlen(content));
                    if(ret_st < 0) return -1;
                }
            }

            task_timer = millis() + 250;
            sw_app_st = CLOSE_CLIENT_CONN;
            break;

            case CLOSE_CLIENT_CONN:
                ret_st = WebServer_close_connect(client_socket);
                if(ret_st < 0) return -1;
                client_socket = -1;
                sw_app_st = WAIT_CONNECT;
            break;
        }
    }
    return 0;
}

void Find_Me(void){
    /* Timer register */
    static unsigned long tmr = 0;

    /* Simple mdnsAdvisor function */
    char mdns_msg[10];
    memset(mdns_msg, 0, sizeof(mdns_msg));

    if(millis() > tmr){
        strncpy_P(mdns_msg, PSTR("WiFiSW"), sizeof(mdns_msg));
        mdnsAdvertiser(1, (char*)mdns_msg, strlen(mdns_msg));
        tmr = millis() + 30000;
    }
}

#define WIFI_INIT           1
#define WIFI_PROC           2
#define WIFI_FAIL           3
unsigned char WiFi_Status = WIFI_INIT;

void WiFi_Process(void){
    int ret_st = 0;
    unsigned long temp = 0;
    switch(WiFi_Status){
        case WIFI_INIT:
            //Serial.println("Initial WiFi");
            Initial_Spider();
            delay(1000);

            // ARP initial for 1.13 patch.
            gethostbyname("localhost", strlen("localhost"), &temp);
            gethostbyname("localhost", strlen("localhost"), &temp);
            delay(1000);

            // Fix UDP sendto issue for 1.13 patch.
            long sock;
            sockaddr addr;

            memset(&addr, 0, sizeof(addr));
            sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            // Setting BSD like socket setting.
            addr.sa_family = AF_INET;
            addr.sa_data[0] = 0;
            addr.sa_data[1] = 0;

            addr.sa_data[2] = 127;
            addr.sa_data[3] = 0;
            addr.sa_data[4] = 0;
            addr.sa_data[5] = 1;
            sendto(sock, &temp, sizeof(temp), 0, &addr, sizeof(sockaddr));
            delay(1000);

            // Initial network apps
            if(Init_SwitchServer() == 0){
                WiFi_Status = WIFI_PROC;
            }
            else{
                WiFi_Status = WIFI_FAIL;
            }
        break;
        case WIFI_PROC:

            Find_Me();

            ret_st = Switch_WebService();
            if(ret_st < 0){
                WiFi_Status = WIFI_FAIL;
                break;
            }
        break;
        case WIFI_FAIL:
            //Serial.println("Close WiFi");
            Spider_close();
            delay(1000);
            WiFi_Status = WIFI_INIT;
        break;
    }
}

void loop() {
    WiFi_Process();
    // Relay is low active
    if(Relay_Status == LOW){
        digitalWrite(RELAY_PIN, HIGH);
    }
    else{
        digitalWrite(RELAY_PIN, LOW);
    }
}