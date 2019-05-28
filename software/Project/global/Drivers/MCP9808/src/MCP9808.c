#include "MCP9808.h"


#define SET(p, n)   ( (p) |= ((1) << (n) ))
#define CLEAR(p, n) ( (p) &= ~((1) << (n)) )
#define TEST(p, n)  ( (p)&((1) << (n)))


static MCP9808_config_t ;


void MCP9808_WriteConfReg();
void MCP9808_WriteReg(uint8_t reg, uint16_t val);

void MCP9808_Init(MCP9808_config_t* config){

    i2c_init();

    defConf.Address         = config->address;
    defConf.Hysteresis      = config->Hysteresis;
    defConf.Resulolution    = config->Resulolution;
    defConf.options         = config->options;


    MCP9808_WriteConfReg();

}



void MCP9808_SetHysteresis(hyst_t hyst){
    defConf.Hysteresis = hyst;
    MCP9808_WriteConfReg();
}


void MCP9808_LowPower(bool set){
    if(set){
        SET(defConf.options, SHUTDOWN);
    }
    else
    {
        CLEAR(defConf.options, SHUTDOWN);
    }

    MCP9808_WriteConfReg();
    
}


void MCP9808_LockWindow(bool set){
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

void MCP9808_EnableInterrupt(bool set){
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



void MCP9808_SetUpperTemp(int temp){

    static int8_t  exp = 0;
    static uint16_t val = 0:

    data.r2 = temp;
   
    exp = ((data.r1&0x7F800000) >> 23) - 127;
    val = ( ((data.r1&0x7FFFFF)|0x800000) >> (19 - exp)) & 0xFFC  ;
   
    if(temp < 0)
        val |= 0x1000;

    MCP9808_WriteReg(TUPPER, val);


}

void MCP9808_SetLowerTemp(int temp){
    static int8_t  exp = 0;
    static uint16_t val = 0:

    data.r2 = temp;
   
    exp = ((data.r1&0x7F800000) >> 23) - 127;
    val = ( ((data.r1&0x7FFFFF)|0x800000) >> (19 - exp)) & 0xFFC  ;
   
    if(temp < 0)
        val |= 0x1000;

    MCP9808_WriteReg(TUPPER, val);
}


int MCP9808_GetUpperTemp(){

}
int MCP9808_GetUpperTemp();

TempStatus_t MCP9808_GetTemp(int* temp);

uint_16 MCP9808_GetManId();
uint_16 MCP9808_GetDevId();

void MCP9808_SetResolution();