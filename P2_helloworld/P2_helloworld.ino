/*****************************************************************************
*
*  P1_testhw.ino - Practice 1, test connected hardware and 
*  join wireless network.
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
#include <stdio.h>
#include <string.h>

#include "Spider_L3.h"

const int INDICATE_LED = 13;

// Configure your WiFi module pin connection.
unsigned char WLAN_CS = 4;
unsigned char WLAN_EN = 7;
unsigned char WLAN_IRQ = 2;
unsigned char WLAN_IRQ_INTNUM = 0;

// Don't forget set correct WiFi SSID and Password.
char AP_Ssid[] = {"..."};
char AP_Pass[] = {"..."};

int http_server_socket = -1;

void Initial_Spider(void) {          
    int ret = 0;
    /* initial uart message output interface. */
    Serial.begin(115200);

    Serial.println(F("=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+"));
    Serial.println(F("   Spider L3 Practice 2 Hello world   "));
    Serial.println(F("=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+"));

    /* initial status LED pin */
    pinMode(INDICATE_LED, OUTPUT);
    digitalWrite(INDICATE_LED, LOW);

    /* Initial Spider L3 */
    Serial.print(F("Starting Spider L3..."));
    ret = Spider_begin();
    if(ret != 0){
        while(1){};
    }
    Serial.println(F("ok"));

    // Spider L3 SPI interface initial success, indicated LED signal.
    digitalWrite(INDICATE_LED, HIGH); 
    delay(100);
    digitalWrite(INDICATE_LED, LOW);

    /* Connect to WiFi AP */
    Serial.print(F("Connect to "));
    Serial.write((unsigned char*)AP_Ssid, strlen(AP_Ssid));
    Serial.print(F(" access point..."));
    ret = Spider_Connect(3, (char*)AP_Ssid, (char*)AP_Pass);
    if(ret != 0){
        Serial.println(F("fail."));
        while(1) ;
    }
    Serial.println(F("ok"));

    digitalWrite(INDICATE_LED, HIGH); 
    delay(100);
    digitalWrite(INDICATE_LED, LOW);

    /* wait connection and Get DHCP address finished */
    Serial.print(F("Waiting DHCP..."));
    while((Spider_CheckConnected() != 0) || (Spider_CheckDHCP() != 0)) {};
    Serial.println(F("ok"));

    // Spider L3 connect success, indicated LED signal.
    digitalWrite(INDICATE_LED, HIGH); 
    delay(100);
    digitalWrite(INDICATE_LED, LOW);

    /* Print device's IP address */
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

int Initial_ServerSocekt(unsigned short port){
    int sock = 0;
    sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(socket < 0){
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(0);

    if (bind(sock, (sockaddr *)&addr, sizeof(addr)) < 0){
        closesocket(sock);
        return -2;
    }

    if (listen(sock, 0) < 0){
        closesocket(sock);
        return -3;
    }

    return(sock);
}

void WebService(int server_socket){
    int client_socket = -1;
    sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);
    char client_msg[100];

    client_socket = accept(server_socket, (sockaddr *)&client_name, &client_name_len);
    if(client_socket < 0) return;

    long timeout = 100;
    if(setsockopt(client_socket, SOL_SOCKET, SOCKOPT_RECV_TIMEOUT, &timeout, sizeof(timeout)) < 0){
        closesocket(client_socket);
        return;
    }

    memset(client_msg, 0, sizeof(client_msg));
    while(recv(client_socket, client_msg, sizeof(client_msg), 0) > 0){
        Serial.write((unsigned char*)client_msg, strlen(client_msg));
        memset(client_msg, 0, sizeof(client_msg));
    }

    memset(client_msg, 0, sizeof(client_msg));
    strncpy_P(client_msg, PSTR("HTTP/1.1 200 OK\r\n"), sizeof(client_msg));
    send(client_socket, client_msg, strlen(client_msg), 0);
    Serial.write((unsigned char*)client_msg, strlen(client_msg));

    memset(client_msg, 0, sizeof(client_msg));
    strncpy_P(client_msg, PSTR("Content-Type: text/html\r\n"), sizeof(client_msg));
    send(client_socket, client_msg, strlen(client_msg), 0);
    Serial.write((unsigned char*)client_msg, strlen(client_msg));

    memset(client_msg, 0, sizeof(client_msg));
    strncpy_P(client_msg, PSTR("Connection: close\r\n\r\n"), sizeof(client_msg));
    send(client_socket, client_msg, strlen(client_msg), 0);
    Serial.write((unsigned char*)client_msg, strlen(client_msg));

    memset(client_msg, 0, sizeof(client_msg));
    strncpy_P(client_msg, PSTR("<html>\r\n<body>\r\n\r\n"), sizeof(client_msg));
    send(client_socket, client_msg, strlen(client_msg), 0);
    Serial.write((unsigned char*)client_msg, strlen(client_msg));

    memset(client_msg, 0, sizeof(client_msg));
    strncpy_P(client_msg, PSTR("<p>Hello Maker!</p>"), sizeof(client_msg));
    send(client_socket, client_msg, strlen(client_msg), 0);
    Serial.write((unsigned char*)client_msg, strlen(client_msg));

    memset(client_msg, 0, sizeof(client_msg));
    strncpy_P(client_msg, PSTR("</body>\r\n</html>\r\n\r\n"), sizeof(client_msg));
    send(client_socket, client_msg, strlen(client_msg), 0);
    Serial.write((unsigned char*)client_msg, strlen(client_msg));

    delay(1000);

    closesocket(client_socket);
}

void setup() {
    int ret = 0;
    Initial_Spider();
    
    // Initial http listen service.
    ret = Initial_ServerSocekt(80);
    if(ret >= 0) http_server_socket = ret;
    else Serial.println(F("WebService intial fail."));

}

void loop(void){
    WebService(http_server_socket);
}