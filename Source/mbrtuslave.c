/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：modbusrtuslave.c                                               **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于声明Modbus RTU从站相关属性及方法                           **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2016-04-17          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/

#include "mbrtuslave.h"
#include "mbpdu.h"

// #define StationAddress 0x01   /*定义本站地址*/
static uint16_t data_values[130];

/*处理读线圈状态命令*/
static uint16_t HandleReadCoilStatusCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
    uint16_t length = 0;

    GetCoilStatus(startAddress, quantity, data_values);

    length = SyntheticSlaveAccessRespond(receivedMessage, data_values, respondBytes);

    return length;
}

/*处理读输入状态命令*/
static uint16_t HandleReadInputStatusCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
    uint16_t length = 0;

    GetInputStatus(startAddress, quantity, data_values);

    length = SyntheticSlaveAccessRespond(receivedMessage, data_values, respondBytes);

    return length;
}

/*处理读保持寄存器命令*/
static uint16_t HandleReadHoldingRegisterCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
    uint16_t length = 0;

    GetHoldingRegister(startAddress, quantity, data_values);

    length = SyntheticSlaveAccessRespond(receivedMessage, data_values, respondBytes);

    return length;
}

/*处理读输入寄存器命令*/
static uint16_t HandleReadInputRegisterCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
    uint16_t length = 0;

    GetInputRegister(startAddress, quantity, data_values);

    length = SyntheticSlaveAccessRespond(receivedMessage, data_values, respondBytes);

    return length;
}

/*处理写单个线圈命令*/
static uint16_t HandleWriteSingleCoilCommand(uint16_t coilAddress, uint16_t coilValue, uint8_t *receivedMessage, uint8_t *respondBytes)
{
    uint16_t length = 0;
    uint16_t value;

    length = SyntheticSlaveAccessRespond(receivedMessage, NULL, respondBytes);

    if(CovertSingleCommandCoilToBoolStatus(coilValue, &value)==true)
    {
        SetSingleCoil(coilAddress, value);
    }
    return length;
}

/*处理写单个寄存器命令*/
static uint16_t HandleWriteSingleRegisterCommand(uint16_t registerAddress, uint16_t registerValue, uint8_t *receivedMessage, uint8_t *respondBytes)
{
    uint16_t length = 0;

    length = SyntheticSlaveAccessRespond(receivedMessage, NULL, respondBytes);

    SetSingleRegister(registerAddress, registerValue);

    return length;
}

/*处理写多个线圈状态*/
static uint16_t HandleWriteMultipleCoilCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
    uint16_t length = 0;

    length = SyntheticSlaveAccessRespond(receivedMessage, data_values, respondBytes);

    SetMultipleCoil(startAddress, quantity, data_values);

    return length;
}

/*处理写多个寄存器状态*/
static uint16_t HandleWriteMultipleRegisterCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
    uint16_t length = 0;

    length = SyntheticSlaveAccessRespond(receivedMessage, data_values, respondBytes);

    SetMultipleRegister(startAddress, quantity, data_values);
    return length;
}

/*处理未支持的命令*/
static uint16_t HandleUnsupportCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
    uint16_t length = 0;
    length = SyntheticSlaveAccessRespond(receivedMessage, NULL, respondBytes);

    return length;
}

typedef uint16_t (*HandleMasterCommandType) (uint16_t, uint16_t, uint8_t *, uint8_t *);

const HandleMasterCommandType HandleMasterCommand[] = {
    [UnsupportCode]         = HandleUnsupportCommand,
    [ReadCoilStatus]        = HandleReadCoilStatusCommand,
    [ReadInputStatus]       = HandleReadInputStatusCommand,
    [ReadHoldingRegister]   = HandleReadHoldingRegisterCommand,
    [ReadInputRegister]     = HandleReadInputRegisterCommand,
    [WriteSingleCoil]       = HandleWriteSingleCoilCommand,
    [WriteSingleRegister]   = HandleWriteSingleRegisterCommand,
    [WriteMultipleCoil]     = HandleWriteMultipleCoilCommand,
    [WriteMultipleRegister] = HandleWriteMultipleRegisterCommand,
};



/*解析接收到的信息，并返回本次处理字节数或失败返回负数，通过回调函数*/
int16_t ParsingMasterAccessCommand(uint8_t *receivedMessage, uint16_t rxLength, uint8_t *respondBytes, uint16_t *txLength, uint8_t StationAddress)
{
    uint16_t respondLength = 0;
    uint8_t index;

    uint8_t slaveAddress = *receivedMessage;
    if (slaveAddress != StationAddress)
    /*判断是否是本站，如不是则丢弃*/
    {
        return -2;
    }


    FunctionCode fc = (FunctionCode)((receivedMessage[1]));
    if (CheckFunctionCode(fc) != Modbus_OK)
    /*判断功能码是否支持*/
    {
        index = UnsupportCode;
        respondLength = HandleMasterCommand[index](0, 0, receivedMessage, respondBytes);
        *txLength = respondLength;
        return respondLength;
    }

    /*信息校验，如不正确则为错误信息不用处理*/
    uint16_t dataLength = 8;
    if ((fc == WriteMultipleCoil) || (fc == WriteMultipleRegister))
    {
        dataLength = (uint16_t)(receivedMessage[6]) + 9;
        if (rxLength < dataLength) // 尚未接收完整
        {
            return 0;
        }
    }
    bool chechMessageNoError = CheckRTUMessageIntegrity(receivedMessage, dataLength);
    if (!chechMessageNoError)
    /* crc校验 */
    {
        return -1;
    }

    uint16_t startAddress = ((uint16_t)(receivedMessage[2]))<<8 | (uint16_t)(receivedMessage[3]);
    uint16_t quantity     = ((uint16_t)(receivedMessage[4]))<<8 | (uint16_t)(receivedMessage[5]);

    index = (fc);

    respondLength = HandleMasterCommand[index](startAddress, quantity, receivedMessage, respondBytes);
    *txLength = respondLength;
    return dataLength;
}



/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/