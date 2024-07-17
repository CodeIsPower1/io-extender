/******************************************************************************/
/** 模块名称：Modbus本地从站实例模块                                         **/
/** 文件名称：mcudsprocess.h                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：Modbus本地从站实例模块上位显示串行接口软件，实现对上位机的通   **/
/**           讯。基于USART1端口，采用RS232，实现Modbus RTU从站。            **/
/**           PA9       USART1_TX       USART1串行发送                       **/
/**           PA10      USART1_RX       USART1串行接收                       **/
/**           PD8       RS232_INT       RS232中断信号                        **/
/**           基于STM32F407ZGT6硬件平台，软件库采用HAL FW_F4 V1.26.0库       **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2021-03-29          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/

#include <stdint.h>
#include "uart-interface.h"
#include "mbrtuslave.h"


#define MCUDRECVLENGTH 256 // 接收数据的最大长度
#define MCUDSENDLENGTH 256 // 发送数据的最大长度



uint16_t localStation = 1;
uint16_t mcudRxLength = 0;
uint8_t mcudRxBuffer[MCUDRECVLENGTH];

uint16_t mcudTxIndex = 0;
uint16_t mcudTxLength = 0;
uint8_t mcudTxBuffer[MCUDSENDLENGTH];


/* 数据发送函数 */
static void McudSendData(uint8_t *sData, uint16_t sSize);

/*设备对外通讯数据处理*/
void LocalSlaveProcess(void)
{
    int r;
    int16_t respondLength = 0;
    r = uart_read(mcudRxBuffer + mcudRxLength, MCUDRECVLENGTH - mcudRxLength);
    if (r >= 0)
    {
        mcudRxLength += r;
    }
    if (mcudRxLength < 2)
    {
        return;
    }
    /* not for localStation data */
    if (mcudRxBuffer[0] != localStation)
    {
        mcudRxLength = 0;
        return;
    }

    if (mcudRxLength >= 8)
    {

        respondLength = ParsingMasterAccessCommand(mcudRxBuffer, mcudRxLength, mcudTxBuffer, &mcudTxLength, localStation);
        if (respondLength > 0)
        {
            mcudRxLength = 0;
            /* USART1数据发送函数 */
            McudSendData(mcudTxBuffer, mcudTxLength);
        } else if (respondLength < 0){
            /* unavalible data, discard */
            mcudRxLength = 0;
        }
    }
}

/* USART1数据发送函数 */
static void McudSendData(uint8_t *sData, uint16_t sSize)
{
    uart_write(sData, sSize);
}


/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/
