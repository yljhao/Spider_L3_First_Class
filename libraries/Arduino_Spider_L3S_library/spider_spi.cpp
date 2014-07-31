/*****************************************************************************
*
*  spider_spi.c - Spider_L3 spi driver impliment.
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
#include <stdio.h>
#include "spider_spi.h"
#include "hci.h"
#include "evnt_handler.h"

#include <SPI.h>

#define         READ                                3
#define         WRITE                               1

#define         HI(value)                           (((value) & 0xFF00) >> 8)
#define         LO(value)                           ((value) & 0x00FF)


#define         HEADERS_SIZE_EVNT                   (SPI_HEADER_SIZE + 5)

#define         SPI_HEADER_SIZE                     (5)

#define         eSPI_STATE_POWERUP                  (0)
#define         eSPI_STATE_INITIALIZED              (1)
#define         eSPI_STATE_IDLE                     (2)
#define         eSPI_STATE_WRITE_IRQ                (3)
#define         eSPI_STATE_WRITE_FIRST_PORTION      (4)
#define         eSPI_STATE_WRITE_EOT                (5)
#define         eSPI_STATE_READ_IRQ                 (6)
#define         eSPI_STATE_READ_FIRST_PORTION       (7)
#define         eSPI_STATE_READ_EOT                 (8)

// The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
// for the purpose of detection of the overrun. The location of the memory where the magic number 
// resides shall never be written. In case it is written - the overrun occured and either recevie function
// or send function will stuck forever.
#define CC3000_BUFFER_MAGIC_NUMBER (0xDE)

typedef struct
{
    gcSpiHandleRx  SPIRxHandler;
    unsigned short usTxPacketLength;
    unsigned short usRxPacketLength;
    unsigned long  ulSpiState;
    unsigned char *pTxPacket;
    unsigned char *pRxPacket;

}tSpiInformation;


volatile tSpiInformation sSpiInformation;

//
// Static buffer for 5 bytes of SPI HEADER
//
unsigned char tSpiReadHeader[] = {READ, 0, 0, 0, 0};
unsigned char wlan_rx_buffer[CC3000_RX_BUFFER_SIZE];
unsigned char wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];

extern unsigned char WLAN_CS;
extern unsigned char WLAN_EN;
extern unsigned char WLAN_IRQ;
extern unsigned char WLAN_IRQ_INTNUM;

//
// Status flag for SPI pause and resume functions.
//
static volatile unsigned char   WLAN_INT_STATUE_REG = 0;
#define WLAN_INT_ENABLE         0b00000001
#define WLAN_INT_PROCESS        0b00000010
#define WLAN_INT_MISSED         0b00000100


/*
    GPIO register operate macro, for faster access time.
*/

#define CC3000_CS_DISABLE() do{                 \
    digitalWrite(WLAN_CS, HIGH);                \
}while(0)

#define CC3000_CS_ENABLE() do{                  \
    SPI.setDataMode(WLAN_SPI_MODE);             \
    SPI.setBitOrder(WLAN_SPI_BITORDER);         \
    SPI.setClockDivider(WLAN_SPI_CLOCK_DIV);    \
    digitalWrite(WLAN_CS, LOW);                 \
}while(0)

void SpiWriteDataSynchronous(unsigned char *data, unsigned short size);
void SpiWriteAsync(const unsigned char *data, unsigned short size);
void SpiPauseSpi(void);
void SpiResumeSpi(void);
void SSIContReadOperation(void);
void WLAN_IRQ_Handler(void);

//*****************************************************************************
//
//*****************************************************************************
long ReadWlanInterruptPin(void){
    return digitalRead(WLAN_IRQ);
}
//*****************************************************************************
//
//*****************************************************************************
void WlanInterruptEnable(void){
    WLAN_INT_STATUE_REG |= WLAN_INT_ENABLE;

    if(WLAN_INT_STATUE_REG & WLAN_INT_MISSED){
        if( (ReadWlanInterruptPin() == LOW) && (!(WLAN_INT_STATUE_REG & WLAN_INT_PROCESS)) ) {
            WLAN_IRQ_Handler();
        }
        WLAN_INT_STATUE_REG &= ~WLAN_INT_MISSED;
    }
}
//*****************************************************************************
//
//*****************************************************************************
void WlanInterruptDisable(void){
    WLAN_INT_STATUE_REG &= ~WLAN_INT_ENABLE;
}
//*****************************************************************************
//
//*****************************************************************************
void WriteWlanPin( unsigned char val ){
    if (val) {
        digitalWrite(WLAN_EN, HIGH);
    }
    else {
        digitalWrite(WLAN_EN, LOW);
    }
}
//*****************************************************************************
//
//!  SpiClose
//!
//!  \param  none
//!
//!  \return none
//!
//!  \brief  Cofigure the SSI
//
//*****************************************************************************
void
SpiClose(void)
{
    if (sSpiInformation.pRxPacket)
    {
        sSpiInformation.pRxPacket = 0;
    }

    //
    //  Disable Interrupt in GPIOA module...
    //
    tSLInformation.WlanInterruptDisable();
}

//*****************************************************************************
//
//! This function: init_spi
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief  initializes an SPI interface
//
//*****************************************************************************
int init_spi(void)
{
    SPI.begin();
    SPI.setDataMode(WLAN_SPI_MODE);
    SPI.setBitOrder(WLAN_SPI_BITORDER);
    SPI.setClockDivider(WLAN_SPI_CLOCK_DIV);
    return 0;
}
//*****************************************************************************
//
//! This function: init_spi
//!
//!  \param  none
//!
//!  \return none
//!
//!  \brief  initializes an gpio interface
//
//*****************************************************************************
void init_io(void){

    WLAN_INT_STATUE_REG |= WLAN_INT_ENABLE;
    pinMode(WLAN_IRQ, INPUT);
    attachInterrupt(WLAN_IRQ_INTNUM, WLAN_IRQ_Handler, FALLING);

    pinMode(WLAN_EN, OUTPUT);
    digitalWrite(WLAN_EN, LOW); // make sure it's off until we're ready
    
    pinMode(WLAN_CS, OUTPUT);
    CC3000_CS_DISABLE();
}

//*****************************************************************************
//
//*****************************************************************************
void CC3000_Init(void){
    init_io();
    init_spi();
}

//*****************************************************************************
//
//!  SpiClose
//!
//!  \param  none
//!
//!  \return none
//!
//!  \brief  Cofigure the SSI
//
//*****************************************************************************
void 
SpiOpen(gcSpiHandleRx pfRxHandler)
{
    
    sSpiInformation.ulSpiState = eSPI_STATE_POWERUP;
    
    memset(wlan_rx_buffer, 0, sizeof(wlan_rx_buffer));
    memset(wlan_tx_buffer, 0, sizeof(wlan_tx_buffer));
    
    sSpiInformation.SPIRxHandler = pfRxHandler;
    sSpiInformation.usTxPacketLength = 0;
    sSpiInformation.pTxPacket = NULL;
    sSpiInformation.pRxPacket = wlan_rx_buffer;
    sSpiInformation.usRxPacketLength = 0;
    wlan_rx_buffer[CC3000_RX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;
    wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;


    //
    // Enable interrupt of WLAN IRQ
    //
    tSLInformation.WlanInterruptEnable();
    
}

//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief  ...
//
//*****************************************************************************
long
SpiFirstWrite(unsigned char *ucBuf, unsigned short usLength)
{
    //
    // workaround for first transaction
    //
    CC3000_CS_ENABLE();
    
    // Assuming we are running on 24 MHz ~50 micro delay is 1200 cycles;
    delayMicroseconds(50);
    
    // SPI writes first 4 bytes of data
    SpiWriteDataSynchronous(ucBuf, 4);

    delayMicroseconds(50);
    
    SpiWriteDataSynchronous(ucBuf + 4, usLength - 4);

    // From this point on - operate in a regular way
    sSpiInformation.ulSpiState = eSPI_STATE_IDLE;
    
    CC3000_CS_DISABLE();

    return(0);
}



//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief  ...
//
//*****************************************************************************
long
SpiWrite(unsigned char *pUserBuffer, unsigned short usLength)
{
    unsigned char ucPad = 0;

    //
    // Figure out the total length of the packet in order to figure out if there is padding or not
    //
    if(!(usLength & 0x0001))
    {
        ucPad++;
    }


    pUserBuffer[0] = WRITE;
    pUserBuffer[1] = HI(usLength + ucPad);
    pUserBuffer[2] = LO(usLength + ucPad);
    pUserBuffer[3] = 0;
    pUserBuffer[4] = 0;

    usLength += (SPI_HEADER_SIZE + ucPad);
        
    // The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
    // for the purpose of detection of the overrun. If the magic number is overriten - buffer overrun 
    // occurred - and we will stuck here forever!
    if (wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER)
    {
        while (1)
            ;
    }

    if (sSpiInformation.ulSpiState == eSPI_STATE_POWERUP)
    {
        while (sSpiInformation.ulSpiState != eSPI_STATE_INITIALIZED)
            ;
    }

    if (sSpiInformation.ulSpiState == eSPI_STATE_INITIALIZED){
        //
        // This is time for first TX/RX transactions over SPI: the IRQ is down - so need to send read buffer size command
        //
        SpiFirstWrite(pUserBuffer, usLength);
    }
    else{
        //
        // We need to prevent here race that can occur in case 2 back to back packets are sent to the
        // device, so the state will move to IDLE and once again to not IDLE due to IRQ
        //
        tSLInformation.WlanInterruptDisable();

        while (sSpiInformation.ulSpiState != eSPI_STATE_IDLE)
        {
            ;
        }

        sSpiInformation.ulSpiState = eSPI_STATE_WRITE_IRQ;
        sSpiInformation.pTxPacket = pUserBuffer;
        sSpiInformation.usTxPacketLength = usLength;

        //
        // Assert the CS line and wait till SSI IRQ line is active and then initialize write operation
        //
        CC3000_CS_ENABLE();

        //
        // Re-enable IRQ - if it was not disabled - this is not a problem...
        //
        tSLInformation.WlanInterruptEnable();

         // check for a missing interrupt between the CS assertion and enabling back the interrupts
        if (tSLInformation.ReadWlanInterruptPin() == 0)
        {
            SpiWriteDataSynchronous(sSpiInformation.pTxPacket, sSpiInformation.usTxPacketLength);

            sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

            CC3000_CS_DISABLE();
        }
    }


    //
    // Due to the fact that we are currently implementing a blocking situation
    // here we will wait till end of transaction
    //

    while (eSPI_STATE_IDLE != sSpiInformation.ulSpiState)
        ;
    
    return(0);
}

 


//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief  ...
//
//*****************************************************************************
void
SpiWriteDataSynchronous(unsigned char *data, unsigned short size)
{
    unsigned short ptr;
    for(ptr = 0; ptr < size; ptr++){
        SPI.transfer(data[ptr]);
    }
}

//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief  ...
//
//*****************************************************************************
void
SpiReadDataSynchronous(unsigned char *data, unsigned short size)
{
    unsigned short i = 0;

    for (i = 0; i < size; i ++) {
        data[i] = SPI.transfer(0x03);
    }
}



//*****************************************************************************
//
//! This function enter point for read flow: first we read minimal 5 SPI header bytes and 5 Event
//! Data bytes
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief  ...
//
//*****************************************************************************
void
SpiReadHeader(void)
{
    SpiReadDataSynchronous(sSpiInformation.pRxPacket, HEADERS_SIZE_EVNT);
}


//*****************************************************************************
//
//! This function processes received SPI Header and in accordance with it - continues reading 
//! the packet
//!
//!  \param  None
//!
//!  \return None
//!
//!  \brief  ...
//
//*****************************************************************************
long
SpiReadDataCont(void)
{
    long data_to_recv = 0;
    unsigned char *evnt_buff = 0;
    unsigned char type = 0;

    
    //
    //determine what type of packet we have
    //
    evnt_buff =  sSpiInformation.pRxPacket;
    data_to_recv = 0;
    STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_PACKET_TYPE_OFFSET, type);
    
    switch(type)
    {
        case HCI_TYPE_DATA:
        {
            //
            // We need to read the rest of data..
            //
            STREAM_TO_UINT16((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_DATA_LENGTH_OFFSET, data_to_recv);
            if (!((HEADERS_SIZE_EVNT + data_to_recv) & 1))
            {   
                data_to_recv++;
            }

            if (data_to_recv)
            {
                SpiReadDataSynchronous(evnt_buff + HEADERS_SIZE_EVNT, data_to_recv);
            }
            break;
        }
        case HCI_TYPE_EVNT:
        {
            // 
            // Calculate the rest length of the data
            //
            STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE), HCI_EVENT_LENGTH_OFFSET, data_to_recv);
            data_to_recv -= 1;
            
            // 
            // Add padding byte if needed
            //
            if ((HEADERS_SIZE_EVNT + data_to_recv) & 1)
            {
                data_to_recv++;
            }
            
            if (data_to_recv)
            {
                SpiReadDataSynchronous(evnt_buff + HEADERS_SIZE_EVNT, data_to_recv);
            }

            sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;
            break;
        }
    }
    
    return (0);
}






//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  SpiTriggerRxProcessing
//!
//!  \return none
//!
//!  \brief  The function triggers a user provided callback for 
//
//*****************************************************************************
void 
SpiTriggerRxProcessing(void)
{
    //
    // Trigger Rx processing
    //
    SpiPauseSpi();
    CC3000_CS_DISABLE();
        
    // The magic number that resides at the end of the TX/RX buffer (1 byte after the allocated size)
    // for the purpose of detection of the overrun. If the magic number is overriten - buffer overrun 
    // occurred - and we will stuck here forever!
    if (sSpiInformation.pRxPacket[CC3000_RX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER)
    {
        while (1)
            ;
    }
    
    sSpiInformation.ulSpiState = eSPI_STATE_IDLE;
    sSpiInformation.SPIRxHandler(sSpiInformation.pRxPacket + SPI_HEADER_SIZE);

    
}

//*****************************************************************************
// 
//!  The IntSpiGPIOHandler interrupt handler
//! 
//!  \param  none
//! 
//!  \return none
//! 
//!  \brief  GPIO A interrupt handler. When the external SSI WLAN device is
//!          ready to interact with Host CPU it generates an interrupt signal.
//!          After that Host CPU has registrated this interrupt request
//!          it set the corresponding /CS in active state.
// 
//*****************************************************************************
void WLAN_IRQ_Handler(void)
{    
    if(!(WLAN_INT_STATUE_REG & WLAN_INT_ENABLE)){
        WLAN_INT_STATUE_REG |= WLAN_INT_MISSED;
        return;
    }

    WLAN_INT_STATUE_REG |= WLAN_INT_PROCESS;

    if (sSpiInformation.ulSpiState == eSPI_STATE_POWERUP){
        //
        // This means IRQ line was low call a callback of HCI Layer to inform on event
        //
        sSpiInformation.ulSpiState = eSPI_STATE_INITIALIZED;
    }
    else if (sSpiInformation.ulSpiState == eSPI_STATE_IDLE){
        sSpiInformation.ulSpiState = eSPI_STATE_READ_IRQ;
        
        //
        // IRQ line goes down - we are start reception
        //
        CC3000_CS_ENABLE();

        //
        // Wait for TX/RX Compete which will come as DMA interrupt
        // 
        SpiReadHeader();

        sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;
        
        //
        // Read rest of data.
        //
        SSIContReadOperation();
    }
    else if (sSpiInformation.ulSpiState == eSPI_STATE_WRITE_IRQ){
        SpiWriteDataSynchronous(sSpiInformation.pTxPacket, sSpiInformation.usTxPacketLength);

        sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

        CC3000_CS_DISABLE();
    }

    WLAN_INT_STATUE_REG &= ~WLAN_INT_PROCESS;
}

//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  SpiPauseSpi
//!
//!  \return none
//!
//!  \brief  The function triggers a user provided callback for 
//
//*****************************************************************************

void 
SpiPauseSpi(void)
{
    WlanInterruptDisable();
}


//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  SpiResumeSpi
//!
//!  \return none
//!
//!  \brief  The function triggers a user provided callback for 
//
//*****************************************************************************

void 
SpiResumeSpi(void)
{
    WlanInterruptEnable();
}

//*****************************************************************************
//
//! This function enter point for write flow
//!
//!  \param  SSIContReadOperation
//!
//!  \return none
//!
//!  \brief  The function triggers a user provided callback for 
//
//*****************************************************************************

void
SSIContReadOperation(void)
{
    //
    // The header was read - continue with  the payload read
    //
    if (!SpiReadDataCont())
    {
        //
        // All the data was read - finalize handling by switching to teh task
        // and calling from task Event Handler
        //
        SpiTriggerRxProcessing();
    }
}


