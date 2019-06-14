#ifndef _MCP9808_H_
#define  _MCP9808_H_


#include <stdint.h>
#include "hw.h"


typedef enum {
    RFU     = 0x00,             // RFU, Reserved for Future Use (Read-Only register)
    M_CONFIG  = 0x01,             // Configuration registe
    TUPPER  = 0x02,             // Alert Temperature Upper Boundary Trip register
    TLOWER  = 0x03,             // Alert Temperature Lower Boundary Trip register
    TCRIT   = 0x04,             // Critical Temperature Trip register
    TEMP    = 0x05,             // Temperature register 
    MANID   = 0x06,             // Manufacturer ID registe
    DEVID   = 0x07,             // Device ID/Revision register
    RESREG  = 0x08              // Resolution register
}Mregister_t;

typedef enum{
    HYST_1  = 0x00,             // 0º Power-up default
    HYST_2  = 1 << 9,           // +1.5º
    HYST_3  = 2 << 9,           // +3.0º
    HYST_4  = 3 << 9,           // +6.0º

}hyst_t;

typedef enum{
    SHUTDOWN    = 1 << 8,           // Enter in low-power mdoe
    LOCKTCRIP   = 1 << 7,           // Tcrit register can't be written
    LOCKWIN     = 1 << 6,           // Tupper and Tlow can't be written
    CLEARINT    = 1 << 5,           // Clean interrupt
    ALERTOUT    = 1 << 3,           // Enable Interrupt output
    ALERT_1     = 1 << 2,           // TA > TCRIT only,
    ALERTPHIGH  = 1 << 1,           // Alert polarity Active-high

}ConfOpt_t;

typedef enum{
    RES_1,                          // +0.5°C (tCONV = 30 ms typical)
    RES_2,                          // +0.25°C (tCONV = 65 ms typical)
    RES_3,                          // +0.125°C (tCONV = 130 ms typical)
    RES_4                           // +0.0625°C (power-up default, tCONV = 250 ms typical)
}Resolution_t;

typedef enum{
    STATUS_0    = 0,
    STATUS_1    = 1 << 15,          // Ta > Tcrit
    STATUS_2    = 1 << 14,          // Ta > Tupper
    STATUS_3    = 1<< 13            // Ta < Tlower
}TempStatus_t;


typedef struct 
{
    uint8_t         Address;
    hyst_t          Hysteresis;
    Resolution_t    Resulolution;
    ConfOpt_t       options;

}MCP9808_config_t;




static union Data{
    uint32_t r1;
    float  r2;
}data;

//Config funtions


void MCP9808_DeInit();
void MCP9808_Init(MCP9808_config_t* config);
void MCP9808_setConfig(MCP9808_config_t* config);
void MCP9808_SetHysteresis(hyst_t hyst);
void MCP9808_LowPower(uint8_t set);
void MCP9808_LockWindow(uint8_t set);
void MCP9808_ClearInterrupt();
void MCP9808_EnableInterrupt(uint8_t set);
void MCP9808_SetConfigOptions(uint16_t options);



void MCP9808_SetUpperTemp(float temp);
void MCP9808_SetLowerTemp(float temp);
float MCP9808_GetUpperTemp();
float MCP9808_GetUpperTemp();

TempStatus_t MCP9808_GetTemp(float* temp);

uint16_t MCP9808_GetManId();
uint16_t MCP9808_GetDevId();

void MCP9808_SetResolution(Resolution_t resolution);


#endif