#include "MCP9808.h"



#define SET(p, n)   ( (p) |= ((1) << (n) ))
#define CLEAR(p, n) ( (p) &= ~((1) << (n)) )
#define TEST(p, n)  ( (p)&((1) << (n)))


static MCP9808_config_t defConf;
static I2C_HandleTypeDef hi2c1;

void MCP9808_WriteConfReg();
void MCP9808_WriteReg(uint8_t reg, uint16_t val);
uint16_t MCP9808_ReadReg(uint8_t reg);
void i2c_init();

void MCP9808_Init(MCP9808_config_t* config){

    i2c_init();

    defConf.Address         = config->Address;
    defConf.Hysteresis      = config->Hysteresis;
    defConf.Resulolution    = config->Resulolution;
    defConf.options         = config->options;


    MCP9808_WriteConfReg();
}



void MCP9808_SetHysteresis(hyst_t hyst){
    defConf.Hysteresis = hyst;
    MCP9808_WriteConfReg();
}


void MCP9808_LowPower(uint8_t set){
    if(set){
        SET(defConf.options, SHUTDOWN);
    }
    else
    {
        CLEAR(defConf.options, SHUTDOWN);
    }

    MCP9808_WriteConfReg();
    
}


void MCP9808_LockWindow(uint8_t set){
    if(set){
        SET(defConf.options, LOCKWIN);
    }
    else
    {
        CLEAR(defConf.options, LOCKWIN);
    }

    MCP9808_WriteConfReg();
}

void MCP9808_ClearInterrupt(){
    SET(defConf.options, CLEARINT);
    MCP9808_WriteConfReg();
    CLEAR(defConf.options, CLEARINT);

}

void MCP9808_EnableInterrupt(uint8_t set){
    if(set){
        SET(defConf.options, ALERTOUT);
    }
    else
    {
        CLEAR(defConf.options, ALERTOUT);
    }

    MCP9808_WriteConfReg();

}

void MCP9808_SetConfigOptions(uint16_t options){
    defConf.options |= options;
    MCP9808_WriteConfReg();
}



void MCP9808_SetUpperTemp(float temp){

    static int8_t  exp = 0;
    static uint16_t val = 0;

    data.r2 = temp;
   
    exp = ((data.r1&0x7F800000) >> 23) - 127;
    val = ( ((data.r1&0x7FFFFF)|0x800000) >> (19 - exp)) & 0xFFC  ;
   
    if(temp < 0)
        val |= 0x1000;

    MCP9808_WriteReg(TUPPER, val);


}

void MCP9808_SetLowerTemp(float temp){
    static int8_t  exp = 0;
    static uint16_t val = 0;

    data.r2 = temp;
   
    exp = ((data.r1&0x7F800000) >> 23) - 127;
    val = ( ((data.r1&0x7FFFFF)|0x800000) >> (19 - exp)) & 0xFFC  ;
   
    if(temp < 0)
        val |= 0x1000;

    MCP9808_WriteReg(TUPPER, val);
}


float MCP9808_GetUpperTemp(){
    static uint16_t val = 0;

    val = MCP9808_ReadReg(TUPPER);
    if(val&0x1000)
        return -(float)((val&0xFFF) >> 2)/4.0;
    else
        return (float)((val&0xFFF) >> 2)/4.0;



}

float MCP9808_GetLowerTemp(){
    static uint16_t val = 0;

    val = MCP9808_ReadReg(TUPPER);

    if(val&0x1000)
        return -(float)((val&0xFFF) >> 2)/4.0;
    else
        return (float)((val&0xFFF) >> 2)/4.0;
    

}

TempStatus_t MCP9808_GetTemp(float* temp){
    static uint16_t val = 0;

    //ToDo -> complete !
    val = MCP9808_ReadReg(TEMP);
    
    if(val&0x1000)
        *temp = -(float)(val&0xFFF)/16.0;
    else
        *temp = (float)(val&0xFFF)/16.0;

    return (val&0xE000);


}

uint16_t MCP9808_GetManId(){
    return MCP9808_ReadReg(MANID);
}
uint16_t MCP9808_GetDevId(){
    return MCP9808_ReadReg(DEVID);
}

void MCP9808_SetResolution(Resolution_t resolution){
    MCP9808_WriteReg(RESREG, resolution);
}

void i2c_init(){

    GPIO_InitTypeDef GPIO_InitStruct = {0};

  
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /**I2C1 GPIO Configuration    
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;


    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  

    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x00000103;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_ENABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        while (1)
        {
            /* code */
        }
        
    }


};




void MCP9808_WriteConfReg(){

    
    return;
}


void MCP9808_WriteReg(uint8_t reg, uint16_t val){
    static uint8_t outBff[3];


    if(reg == RESREG){

        HAL_I2C_Mem_Write(&hi2c1, defConf.Address, reg, 1, val, 1, HAL_MAX_DELAY);
        return;
    }

    outBff[0] = val&0xFF;
    outBff[1] = (val >> 8)&0xFF;
  

    HAL_I2C_Mem_Write(&hi2c1, defConf.Address, reg, 1, outBff, 2, HAL_MAX_DELAY);

        
}
uint16_t MCP9808_ReadReg(uint8_t reg){
    static uint8_t inBuff[2];


    if(reg == RESREG){
        HAL_I2C_Mem_Read(&hi2c1, defConf.Address,  reg, 1, inBuff, 1, HAL_MAX_DELAY);
        return inBuff[0];
    }

    HAL_I2C_Mem_Read(&hi2c1, defConf.Address,  reg, 1, inBuff, 2, HAL_MAX_DELAY);
    
    return (inBuff[0] << 8) | inBuff[1];

}