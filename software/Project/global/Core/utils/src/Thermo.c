

#include "Thermo.h"

float Therm_JunctionTempToVoltage(float junction_temp, uint8_t type);
float Therm_VoltageToTemp(float voltage, float junction_temp, uint8_t type);



void therm_init(){
    static MCP9808_config_t config2;
    uint8_t cnt = 0;

    config2.Address = 0x30;
	config2.Hysteresis = HYST_1;
	config2.options = 0;
	config2.Resulolution = RES_1;


	MCP9808_Init(&config2);
    ExtADC_Init();

    for(cnt=0; cnt < 16 ; cnt++)
        channelsType[cnt] = NONE;
    
};



void therm_config(therm_t type, ExtChannel_t channel_p, ExtChannel_t channel_n){
    extADC_t config;

    config.gain 	= EXTADC_GAIN_128;
	config.negative = channel_n;
	config.mode		= DIFFERENTIAL;
    config.ExtConfigOptions = 0;


    ExtADC_ConfigChannel(channel_p, &config);

    channelsType[channel_p] = type;
};



therm_t therm_getType(ExtChannel_t channel)
{
    return channelsType[channel];
}

uint16_t therm_getTemp(ExtChannel_t channel)
{
    static float junctionTemp = 0, voltage = 0, temperature = 0;

    MCP9808_GetTemp(&junctionTemp);
    voltage = ExtADC_ReadVoltageInput(channel);
    temperature = Therm_VoltageToTemp(voltage, junctionTemp, channelsType[channel]);

    return (uint16_t)(temperature * 10);
}


float Therm_VoltageToTemp(float voltage, float junction_temp, therm_t type)
{
    uint16_t index = 0;
    float fJunctionVoltage = 0;
    float fCorrectedVoltage = 0;

    float fM = 0;
    float fRoofVoltage = 0;
    float fFloorVoltage = 0;
    float fTemperature = 0;

    fJunctionVoltage = Therm_JunctionTempToVoltage(junction_temp, type);

    switch (type)
    {
        case TYPE_T:
            fCorrectedVoltage = voltage + fJunctionVoltage;

            /* If voltage out of lower bound, return lower temp */
            if (fCorrectedVoltage < fThermTableTypeT[0])
                return TYPE_T_MIN_TEMP;
            /* If voltage out of upper bound, return higher temp */
            else if (fCorrectedVoltage > fThermTableTypeT[TYPE_T_NUM_POINTS - 1])
                return TYPE_T_MAX_TEMP;

            /* if voltage in bounds */
            else
            {
                for (index = 0; index < TYPE_T_NUM_POINTS; index++)
                {
                    if (fCorrectedVoltage < fThermTableTypeT[index])
                        break;
                }

                /* y = [(y2-y1)/(x2-x1)]�x + y1 - [(y2-y1)/(x2-x1)]�x1 */
                /*y = m�x + y1 - m�x1*/
                /*Notice that x2-x1 is always 1*/

                fRoofVoltage = fThermTableTypeT[index];
                fFloorVoltage = fThermTableTypeT[index - 1];
                fM = 1.0 / (fRoofVoltage - fFloorVoltage);
                fTemperature = fM * fCorrectedVoltage + (float)(index - 1 + TYPE_T_MIN_TEMP) - fM * fFloorVoltage;
            }
            break;

        case TYPE_J:
            fCorrectedVoltage = voltage + fJunctionVoltage;

            /* If voltage out of lower bound, return lower temp */
            if (fCorrectedVoltage < fThermTableTypeJ[0])
                return TYPE_J_MIN_TEMP;
            /* If voltage out of upper bound, return higher temp */
            else if (fCorrectedVoltage > fThermTableTypeJ[TYPE_J_NUM_POINTS - 1])
                return TYPE_J_MAX_TEMP;

            /* if voltage in bounds */
            else
            {
                for (index = 0; index < TYPE_J_NUM_POINTS; index++)
                {
                    if (fCorrectedVoltage < fThermTableTypeJ[index])
                        break;
                }

                /* y = [(y2-y1)/(x2-x1)]�x + y1 - [(y2-y1)/(x2-x1)]�x1 */
                /* y = m�x + y1 - m�x1*/
                /* Notice that x2-x1 is always 1*/

                fRoofVoltage = fThermTableTypeJ[index];
                fFloorVoltage = fThermTableTypeJ[index - 1];
                fM = 1.0 / (fRoofVoltage - fFloorVoltage);
                fTemperature = fM * fCorrectedVoltage + (float)(index - 1 + TYPE_J_MIN_TEMP) - fM * fFloorVoltage;
            }
            break;

        case TYPE_K:
            fCorrectedVoltage = voltage + fJunctionVoltage;

            /* If voltage out of lower bound, return lower temp */
            if (fCorrectedVoltage < fThermTableTypeK[0])
                return TYPE_K_MIN_TEMP;
            /* If voltage out of upper bound, return higher temp */
            else if (fCorrectedVoltage > fThermTableTypeK[TYPE_K_NUM_POINTS - 1])
                return TYPE_K_MAX_TEMP;

            /* if voltage in bounds */
            else
            {
                for (index = 0; index < TYPE_K_NUM_POINTS; index++)
                {
                    if (fCorrectedVoltage < fThermTableTypeK[index])
                        break;
                }

                /* y = [(y2-y1)/(x2-x1)]�x + y1 - [(y2-y1)/(x2-x1)]�x1 */
                /*y = m�x + y1 - m�x1*/
                /*Notice that x2-x1 is always 1*/

                fRoofVoltage = fThermTableTypeK[index];
                fFloorVoltage = fThermTableTypeK[index - 1];
                fM = 1.0 / (fRoofVoltage - fFloorVoltage);
                fTemperature = fM * fCorrectedVoltage + (float)(index - 1 + TYPE_K_MIN_TEMP) - fM * fFloorVoltage;
            }
            break;
        
        default:
            fTemperature = 0;
            break;
    
    }

    return fTemperature;
}




float Therm_JunctionTempToVoltage(float junction_temp, uint8_t type)
{
    int16_t sFloorTemp = 0;
    float fFloorVoltage = 0;
    float fRoofVoltage = 0;
    float fM = 0;
    float fVoltage = 0;

    switch (type)
    {
    case TYPE_T:
        /* If junction_temp out of lower bound, return lower voltage */
        if (junction_temp <= TYPE_T_MIN_TEMP)
            return fThermTableTypeT[0];

        /* If junction_temp out of upper bound, return higher voltage */
        else if (junction_temp >= TYPE_T_MAX_TEMP)
            return fThermTableTypeT[TYPE_T_NUM_POINTS - 1];

        /* If junction_temp in bounds */
        else
        {
            sFloorTemp = (int16_t)junction_temp;
            fFloorVoltage = fThermTableTypeT[sFloorTemp - TYPE_T_MIN_TEMP];
            fRoofVoltage = fThermTableTypeT[sFloorTemp + 1 - TYPE_T_MIN_TEMP];

            /* y = [(y2-y1)/(x2-x1)]�x + y1 - [(y2-y1)/(x2-x1)]�x1 */
            /*y = m�x + y1 - m�x1*/
            /*Notice that x2-x1 is always 1*/
            fM = (fRoofVoltage - fFloorVoltage);
            fVoltage = fM * junction_temp + fFloorVoltage - fM * (float)sFloorTemp;
        }
        break;

    case TYPE_J:
        /* If junction_temp out of lower bound, return lower voltage */
        if (junction_temp <= TYPE_J_MIN_TEMP)
            return fThermTableTypeJ[0];

        /* If junction_temp out of upper bound, return higher voltage */
        else if (junction_temp >= TYPE_J_MAX_TEMP)
            return fThermTableTypeJ[TYPE_J_NUM_POINTS - 1];

        /* If junction_temp in bounds */
        else
        {
            sFloorTemp = (int16_t)junction_temp;
            fFloorVoltage = fThermTableTypeJ[sFloorTemp - TYPE_J_MIN_TEMP];
            fRoofVoltage = fThermTableTypeJ[sFloorTemp + 1 - TYPE_J_MIN_TEMP];

            /* y = [(y2-y1)/(x2-x1)]�x + y1 - [(y2-y1)/(x2-x1)]�x1 */
            /* y = m�x + y1 - m�x1*/
            /* Notice that x2-x1 is always 1*/
            fM = (fRoofVoltage - fFloorVoltage);
            fVoltage = fM * junction_temp + fFloorVoltage - fM * (float)sFloorTemp;
        }
        break;

    case TYPE_K:
        /* If junction_temp out of lower bound, return lower voltage */
        if (junction_temp <= TYPE_K_MIN_TEMP)
            return fThermTableTypeK[0];

        /* If junction_temp out of upper bound, return higher voltage */
        else if (junction_temp >= TYPE_K_MAX_TEMP)
            return fThermTableTypeK[TYPE_K_NUM_POINTS - 1];

        /* If junction_temp in bounds */
        else
        {
            sFloorTemp = (int16_t)junction_temp;
            fFloorVoltage = fThermTableTypeK[sFloorTemp - TYPE_K_MIN_TEMP];
            fRoofVoltage = fThermTableTypeK[sFloorTemp + 1 - TYPE_K_MIN_TEMP];

            /* y = [(y2-y1)/(x2-x1)]�x + y1 - [(y2-y1)/(x2-x1)]�x1 */
            /* y = m�x + y1 - m�x1*/
            /* Notice that x2-x1 is always 1*/
            fM = (fRoofVoltage - fFloorVoltage);
            fVoltage = fM * junction_temp + fFloorVoltage - fM * (float)sFloorTemp;
        }
        break;
    
    default:
        fVoltage = 0;
        break;
    }
    return fVoltage;
}