/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  NRF24L01 双机通信测试 (软件SPI版本)
 *  硬件接线: CE=PA31 CSN=PC12 SCK=PC13 MOSI=PC15 MISO=PC14
 *
 *  改 DEVICE_MODE 分别编译:
 *    MODE_TX → 发送端
 *    MODE_RX → 接收端
 */

#include "ti_msp_dl_config.h"
#include "OLED.h"
#include "Delay.h"
#include "nrf2401.h"

/* ★★★ 改这里切换角色 ★★★ */
#define MODE_TX  1
#define MODE_RX  2
#define DEVICE_MODE  MODE_TX

#if DEVICE_MODE == MODE_TX
uint8_t  tx_data[4];
uint8_t  send_flag;
uint16_t send_ok;
uint16_t send_fail;
#endif

#if DEVICE_MODE == MODE_RX
uint8_t  rx_data[4];
uint8_t  recv_flag;
uint16_t recv_ok;
uint16_t recv_fail;
#endif

int main(void)
{
    SYSCFG_DL_init();
    OLED_Init();

    /* ---- NRF24L01 自检 (读STATUS, 上电值应为0x0E) ---- */
    NRF24L01_Init();

    OLED_Clear();
    OLED_ShowString(0, 0, "NRF Check...", OLED_8X16);
    OLED_Update();

    if (!NRF24L01_Check())
    {
        uint8_t st = NRF24L01_ReadStatus();
        OLED_Clear();
        OLED_ShowString(0, 0,  "NRF ERR !", OLED_8X16);
        OLED_ShowString(0, 16, "STATUS=", OLED_8X16);
        OLED_ShowHexNum(6*8, 16, st, 2, OLED_8X16);
        OLED_ShowString(0, 32, "CE=PA31 CSN=PC12", OLED_8X16);
        OLED_ShowString(0, 48, "SCK=PC13 MO=PC15 MI=PC14", OLED_8X16);
        OLED_Update();
        while (1);
    }

    /* ---- OLED 界面 ---- */
    OLED_Clear();
#if DEVICE_MODE == MODE_TX
    OLED_ShowString(0, 0,  "== TX MODE ==", OLED_8X16);
    OLED_ShowString(0, 16, "S:0000 F:0000", OLED_8X16);
    OLED_ShowString(0, 32, "D:00 00 00 00", OLED_8X16);
    OLED_ShowString(0, 48, "Sending...", OLED_8X16);
    tx_data[0] = 0; tx_data[1] = 0;
    tx_data[2] = 0; tx_data[3] = 0;
    send_ok = 0; send_fail = 0;
#else
    OLED_ShowString(0, 0,  "== RX MODE ==", OLED_8X16);
    OLED_ShowString(0, 16, "R:0000 E:0000", OLED_8X16);
    OLED_ShowString(0, 32, "D:00 00 00 00", OLED_8X16);
    OLED_ShowString(0, 48, "Listening...", OLED_8X16);
    recv_ok = 0; recv_fail = 0;
#endif
    OLED_Update();

    /* ---- 主循环 ---- */
    while (1)
    {

#if DEVICE_MODE == MODE_TX
        Delay_ms(500);

        tx_data[0]++;
        if (tx_data[0] == 0) tx_data[1]++;
        if (tx_data[0] == 0 && tx_data[1] == 0) tx_data[2]++;
        if (tx_data[0] == 0 && tx_data[1] == 0 && tx_data[2] == 0) tx_data[3]++;

        NRF24L01_TxPacket[0] = tx_data[0];
        NRF24L01_TxPacket[1] = tx_data[1];
        NRF24L01_TxPacket[2] = tx_data[2];
        NRF24L01_TxPacket[3] = tx_data[3];
        send_flag = NRF24L01_Send();

        if (send_flag == 1) send_ok++;
        else               send_fail++;

        OLED_ShowString(0, 16, "S:", OLED_8X16);
        OLED_ShowNum(2*8, 16, send_ok, 4, OLED_8X16);
        OLED_ShowString(6*8, 16, "F:", OLED_8X16);
        OLED_ShowNum(8*8, 16, send_fail, 4, OLED_8X16);

        OLED_ShowString(0, 32, "D:", OLED_8X16);
        OLED_ShowHexNum(2*8, 32, tx_data[0], 2, OLED_8X16);
        OLED_ShowHexNum(5*8, 32, tx_data[1], 2, OLED_8X16);
        OLED_ShowHexNum(8*8, 32, tx_data[2], 2, OLED_8X16);
        OLED_ShowHexNum(11*8, 32, tx_data[3], 2, OLED_8X16);

        OLED_ShowString(0, 48, "TX", OLED_8X16);
        OLED_Update();
        OLED_ShowString(0, 48, "  ", OLED_8X16);
        OLED_UpdateArea(0, 48, 16, 16);
#endif

#if DEVICE_MODE == MODE_RX
        recv_flag = NRF24L01_Receive();
        if (recv_flag)
        {
            if (recv_flag == 1)
            {
                recv_ok++;
                rx_data[0] = NRF24L01_RxPacket[0];
                rx_data[1] = NRF24L01_RxPacket[1];
                rx_data[2] = NRF24L01_RxPacket[2];
                rx_data[3] = NRF24L01_RxPacket[3];
            }
            else
            {
                recv_fail++;
            }

            OLED_ShowString(0, 16, "R:", OLED_8X16);
            OLED_ShowNum(2*8, 16, recv_ok, 4, OLED_8X16);
            OLED_ShowString(6*8, 16, "E:", OLED_8X16);
            OLED_ShowNum(8*8, 16, recv_fail, 4, OLED_8X16);

            OLED_ShowString(0, 32, "D:", OLED_8X16);
            OLED_ShowHexNum(2*8, 32, rx_data[0], 2, OLED_8X16);
            OLED_ShowHexNum(5*8, 32, rx_data[1], 2, OLED_8X16);
            OLED_ShowHexNum(8*8, 32, rx_data[2], 2, OLED_8X16);
            OLED_ShowHexNum(11*8, 32, rx_data[3], 2, OLED_8X16);

            OLED_ShowString(0, 48, "RX", OLED_8X16);
            OLED_Update();
            OLED_ShowString(0, 48, "  ", OLED_8X16);
            OLED_UpdateArea(0, 48, 16, 16);
        }
#endif

    }
}
