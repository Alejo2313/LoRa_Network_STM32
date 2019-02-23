/*******************************************************************************
  Archivos con funciones para RADIO
 *******************************************************************************/

//! \file
#include "radio.h"

DEFINE_RADIO;								//Necesaria para definir la estructura



#if (defined CFG_1278_DRF1278F)  ||  (defined CFG_1278_NICERF1278)    //Selecciona el CHIP SX1278
//Calculo del tiempo por simbolo (Ts) util para establecer si se debe habilitar el Bit de LowDataRateOptimize (Ts>16ms)
u1_t tiempoPorSimbolo ()					//Ts(ms)= 2^SF / (BW {hz}) * 1000
{
  u8_t BW, Ts;
  u1_t Ts_Condicion;
  switch (RADIO.bw)
  {
    case BW7_8:
      BW  = SX1278_BW_7_8;	//7.8Khz
      break;
    case BW10_4:
      BW  = SX1278_BW_10_4;	//10.4Khz
      break;
    case BW15_6:
      BW  = SX1278_BW_15_6;	//15.6Khz
      break;
    case BW20_8:
      BW  = SX1278_BW_20_8;	//20.8Khz
      break;
    case BW31_25:
      BW  = SX1278_BW_31_25;	//31.25Khz
      break;
    case BW41_7:
      BW  = SX1278_BW_41_7;	//41.7Khz
      break;
    case BW62_5:
      BW  = SX1278_BW_62_5;	//62.5Khz
      break;
    case BW125:
      BW  = SX1278_BW_125;	//125Khz
      break;
    case BW250:
      BW  = SX1278_BW_250;	//250Khz
      break;
    case BW500:
      BW  = SX1278_BW_500;	//500Khz
      break;
  }
  Ts_Condicion  = 0;
  Ts = (((u8_t)1 << RADIO.sf));
  Ts = Ts	/ BW;				//Ts(ms)= 2^SF / (BW {hz}) * 1000
  //Ts = Ts * 1000 ;


  if(Ts > 16)													//La condicion establece que si el Tiempo/simbolo es superior 16ms, se debe habilitar BIT LowDataRateOptimize (BIT3 del registro RegModemConfig3)
  {
    Ts_Condicion = 1;
  }
  return Ts_Condicion;										//Envia 0/1  para la configuracion del BIT LowDataRateOptimize del registro RegModemConfig3
}																//Calculo del Ts
#endif





//Escribe en los registros del LORA via SPI
void writeReg (u1_t addr, u1_t data )
{
  hal_pin_nss(0);					//Inicia uso de SPI NSS->0
  hal_spi(addr | 0x80);			//Solicita escritura del registro addr (BIT7->1)
  hal_spi(data);					//Escribe sobre addr el valor de data
  hal_pin_nss(1);					//Finaliza el uso de SPI NSS->1
}

//Lee registros del LORA via SPI
u1_t readReg (u1_t addr)
{
  hal_pin_nss(0);					//Inicia uso de SPI NSS->0
  hal_spi(addr & 0x7F);			//Solicita lectura del addr. Se asegura que el BIT7 sea 0 (cuando es 1, es el modo escritura)
  u1_t val = hal_spi(0x00);		//Lee sobre el BUS SPI (posicion 0) el valor correpondiente a addr
  hal_pin_nss(1);					//Finaliza el uso de SPI NSS->1
  return val;
}


//Utilizada para escribir sobre la FIFO del LORA
void writeBuf (u1_t addr, xref2u1_t buf, u1_t len)
{
  u1_t  i, p1;

  hal_pin_nss(0);					//Inicia uso de SPI NSS->0
  hal_spi(addr | 0x80);			//Escribe la direcci�n del BUFFER FIFO del LORA (por def es 0X00)
  for (i = 0 ; i < len ; i++)		//Escribe los datos de LMIC.frame[..] hasta alcanzar la  cantidad de datos LMIC.dataLen
  {
    p1 = (buf[i]);
    hal_spi(buf[i]);
  }
  hal_pin_nss(1);					//EL SPI finaliza con NSS<- 1    Detiene el BUS SPI
}



//Utilizada para leer sobre la FIFO del LORA
void readBuf (u1_t addr, xref2u1_t buf, u1_t len)		//Utilizada para leer los datos del la FIFO. addr-> DIRECCION inicial a leer     buf->VECTOR de alamcenamiento  len-> longitud de los datos a leer
{
  u1_t i;
  i = 0;
  hal_pin_nss(0);					//EL SPI comieza con NSS<-0
  hal_spi(addr & 0x7F);			//Solicita escritura del registro addr (BIT7->1)
  for (i = 0; i < len ; i++)		//Lee los datos del FIFO y los transcribe a LMIC.frame[..] hasta alcanzar la  cantidad de datos LMIC.dataLen
  {
    buf[i] = hal_spi(0x00);
  }
  hal_pin_nss(1);					//EL SPI Finaliza con NSS<-1    Detiene el BUS SPI
}




//Escribe el modo de operacion del LORA
void opmode (u1_t mode)
{
  writeReg(RegOpMode, (readReg(RegOpMode) & ~OPMODE_MASK) | mode);		//Enmascara los bits 7:3 y escribi el mode sobre 2:0
}




//Escribe sobre el  RegOpMode{1} el  Modo Lora Bit7->1 y el modo SLEEP
void opmodeLora() 							//Se utiliza para escribir el modo LORA y modo SLEEP. Coloca el bit 7->1.
{													//Util para configuracion del LORA (configuracion inicial, configuracion para TX y RX)
  u1_t u = OPMODE_LORA;

#if (defined CFG_1278_DRF1278F)  ||  (defined CFG_1278_NICERF1278)    //Selecciona el CHIP SX1278
  u = OPMODE_LORA | 0x08;   									//ACTUALIZADO El bit3 LowFrequencyModeOn 1 ->Low Frequency Mode (access to LF test registers)
#endif

  writeReg(RegOpMode, u);
}




//Toma los valores de RADIO.sf, RADIO.bw, RADIO.cr y los escribe en los registros de configuracion
void configLoraModem ()
{
  u1_t regconf1, regconf2;
  regconf1 = 0;
  regconf2 = 0;
#if (defined CFG_1278_DRF1278F)  ||  (defined CFG_1278_NICERF1278)    //Selecciona el CHIP SX1278
  u1_t regconf3;
  regconf3 = 0;
#endif


#if defined(CFG_1272_SEMTECH)                          //Selecciona el CHIP SX1272
  //Configuracion LORARegModemConfig1
  regconf1 = RADIO.bw <<6;							//Define RADIO.bw entre los bits 7:6
  regconf1 |= (RADIO.cr << 3);						//Define RADIO.cr entre los bits 3:1
  regconf1 |= SX1272_MC1_RX_PAYLOAD_CRCON;			//Habilita ERROR por RX PAYLOAD CRC
  if ((RADIO.sf == SF_11 || RADIO.sf == SF_12) && RADIO.bw == BW125)
  {
    regconf1 |= SX1272_MC1_LOW_DATA_RATE_OPTIMIZE;			//Define el bit de Optimizacion a baja velocidad
  }
  //BW    CR     Explicit Header mode*-> Bit2    RxPayloadCrcOn->1 Bit1     LowDataRateOptimize->1 Si SF11 o SF12 y BW=125
  RADIO.regModemConfig1 = regconf1;

  regconf2 = RADIO.sf << 4;
  regconf2 |= SX1272_MC2_AGCAUTO_ON;
  // SF   AgcAutoOn=1    SymbTimeout=00
  RADIO.regModemConfig2 = regconf2;
#endif

#if (defined CFG_1278_DRF1278F)  ||  (defined CFG_1278_NICERF1278)    //Selecciona el CHIP SX1278
  //Configuracion LORARegModemConfig1
  regconf1 = RADIO.bw <<4;										//Define RADIO.bw entre los bits 7:4
  regconf1 |= (RADIO.cr << 1);						//Define RADIO.cr entre los bits 3:1
  //BW    CR     Explicit Header mode*-> Bit0-> !SX1278_MC1_IMPLICIT_HEADER_MODE_ON
  RADIO.regModemConfig1 = regconf1;							//Almacena los valores del registro de conf 1

  //Configuracion LORARegModemConfig2
  regconf2 = RADIO.sf <<4;									//Define RADIO.sf entre los bits 7:4
  //Se asume el bit3 ->0 Paquete Single
  regconf2 |= SX1278_MC2_RX_PAYLOAD_CRCON;											//Se asume que RxPayloadCrcOn (BIT2->1) Habilitado
  //Habilita ERROR por RX PAYLOAD CRC
  //SF    TX_SINGLE_PACKET     RxPayloadCrcOn  SymbTimeout(9:8)->0
  RADIO.regModemConfig2 = regconf2;							//Almacena los valores del registro de conf 2

  //Configuracion LORARegModemConfig3
  regconf3 |= SX1278_MC3_AGCAUTO;												//AgcAutoOn-> 1   ganacia del LNA por AGC loop  (CAMBIAR AQUI)

  if(tiempoPorSimbolo() == 1)								//Si el tiempo supera los 16ms
  {
    regconf3 |= SX1278_MC3_LOW_DATA_RATE_OPTIMIZE;		// Ubica el resultado en BIT3. Si el Ts>16ms ->1
  }
  RADIO.regModemConfig3 = regconf3;								//Almacena los valores del registro de conf 3
#endif

  writeReg(LORARegModemConfig1, regconf1);
  writeReg(LORARegModemConfig2, regconf2);

#if (defined CFG_1278_DRF1278F)  ||  (defined CFG_1278_NICERF1278)    //Selecciona el CHIP SX1278
  writeReg(LORARegModemConfig3, regconf3);						//Punto de comprobacion
#endif

}



//Configura el Canal de acuerdo con la frecuencia de RADIO.freq en 3 registros
void configChannel ()
{
  //->   FRF = (FQ *  2 ^ 19) / (32 Mhz)	<- Se despeja para introducir en los registro del LORA  RegFrfMsb RegFrfMid RegFrfLsb
  u8_t frf = ((u8_t)RADIO.freq << 19) / 32000000;
  writeReg(RegFrfMsb, (u1_t)(frf>>16));
  writeReg(RegFrfMid, (u1_t)(frf>> 8));
  writeReg(RegFrfLsb, (u1_t)(frf>> 0));
}



///Configura la potencia a Max potencia RADIO.txpow.RADIO.PaRamp. Potencia limitada hasta 20dBm.
//Se incorpora el registro de regulacion de corriente Reg
void configPower ()
{
  u1_t pw, maxpower, highPower, OcpTrim;

  writeReg(RegLna, LNA_RX_GAIN);									//bit 7:5->001-> Maxima ganancia LNA
  //bit4:3 LnaBoostLf-> corriente por defecto
  //bit1:0 LnaBoostHf-> Boost on, 150% corriente LNA
  //Configuracion de RegPaConfig
  pw = RADIO.txpow;
  if(pw > 17) 														//el modem unicamente permite potencia hasta 17 dBm usando PA_BOOST
  {
    pw = 17;
  }
  else if(pw < 2)
  {
    pw = 2;
  }
  pw = pw - 2;													//Realiza la conversion de 2-17-> 0-15
#if defined(CFG_1272_SEMTECH)                          //Selecciona el CHIP SX1272
  maxpower = 0;													  //Variable no habilitada para SX1272
#endif
#if (defined CFG_1278_DRF1278F)  ||  (defined CFG_1278_NICERF1278)    //Selecciona el CHIP SX1278
  maxpower = 0x07 << 4;											//Con maxpower = 7-> Se obtiene  Pmax=10.8+0.6*MaxPower [dBm]  -> Pmax = 19.8dBm. Ubivcacion Bits6:4															//Pout=17-(15-OutputPower)
#endif
  writeReg(RegPaConfig, ((0x80 | maxpower) | pw ));				//RegPaConfig{09} bit7 limita la potencia a 20dBm
  //Escribe 6:4 -> MAXPOWER->> 7
  //Escribe pw bits 3:0 a RegPaConfig
  //Configuracion RegOcp. Corriente maxima de radio Imax   0x0B. Aplica para SX1272/78
  if(RADIO.imax <= 120)
  {
    if(RADIO.imax > 45)											//Si       45mA <= Imax <=  120 mA
    {
      OcpTrim = (u1_t)((RADIO.imax - 45 )/5);			//Imax = 45+5*OcpTrim [mA] if OcpTrim <= 15 (120 mA)
    }
    else
    {
      OcpTrim = 0x00;
      RADIO.imax = 45;
    }
  }
  else if(RADIO.imax > 120  && RADIO.imax  <= 240)				//Si       120mA < Imax <=  240 mA
  {
    OcpTrim = (u1_t)((RADIO.imax + 30 )/10);				//Imax = -30+10*OcpTrim [mA] if 15 < OcpTrim <= 27 (130 to 240 mA)
  }
  else
  {
    RADIO.imax = 240;											//Se establece la maxima corriente.
    OcpTrim = 0x1B;												//0x1B-> 27
  }
  writeReg(RegOcp, (0x20 | OcpTrim));								//RegOcp{0B}  bit5-> habiita la proteccion contra sobrecorrientes.
  //bit4:0-> Establece la Imax con el parametro OcpTrim
  //Configuracion de RegPaDac
  highPower = 0x04;												//0x04->Para otros casos. Por defecto
  if (pw == 15)													//Si OutputPower= 15  entonces Habilita Alta Potencia +20dBm
  {
    highPower = 0x07;											//0x07 -> +20dBm on PA_BOOST when OutputPower=1111(pw)
  }
  writeReg(RegPaDac, readReg(RegPaDac) | highPower);				//Habilita +20dBm (PA_BOOST pin)Bit2:0. La direccion de RegPaDac es 0x5A-> SX1272    0x4D -> 1278

}




//Configura la TX del Loraa
//Para configurar el lora siempre debe cambiarse de modo SLEEP{00} a Modo STANDBY{01}
void txlora ()
{
  opmodeLora();										//Vuelve a colocar el MODO DESDE SLEEP{00}-> STANDBY{01}
  //El modo STANDBY es requerido para cargar datos al FIFO
  opmode(OPMODE_STANDBY);
  // Ajusta el mapeo DIO0=TxDone DIO1=NOP DIO2=NOP DIO3= MAP_DIO3_LORA_NOP
  writeReg(RegDioMapping1, MAP_DIO0_LORA_TXDONE|MAP_DIO1_LORA_NOP|MAP_DIO2_LORA_NOP|MAP_DIO3_LORA_NOP);
  // clear all radio IRQ flags
  writeReg(LORARegIrqFlags, 0xFF);					//Limpia todas las banderas. Las banderas se limpian colocandolas en '1'
  writeReg(LORARegIrqFlagsMask, ~(IRQ_LORA_TXDONE_MASK));				// | IRQ_LORA_CRCERR_MASK));		//Habilita las interrupciones con '0'. Habilita interrupciones TXDONE y CRC ERROR PAYLOAD

  //Ajusta la direcciones cargar el PAYLOAD de transmisiOn
  writeReg(LORARegFifoTxBaseAddr, 0x00);				//Escribe la direccion inicial del FIFO Buffer para el modulador TX {0E}
  writeReg(LORARegFifoAddrPtr, 0x00);					//Escribe el Pointer del FIFO Buffer
  writeReg(LORARegPayloadLength, RADIO.dataLenTX);		//Escribe la cantidad de datos a transmitir

  writeBuf(RegFifo, RADIO.frameTX, RADIO.dataLenTX);		//Descarga los datos de RADIO.frameTX a la FIFO. La direccion corresponde al registro RegFifo{00}

#if defined(CFG_1278_NICERF1278)                          //Selecciona el CHIP SX1278 de NICERF
  hal_TX_RX_en(TX_EN_OK);											//Selecciona el modo TX por hardware
#endif

  opmode(OPMODE_TX);									//Comienza formalmente la transmision al LORA (El pin RXTX se activa durante la TX, podria utilizarse como interrupcion o indicador de TX)
}




//Configura el modo a modo RX
//Para configurar el LORA siempre debe cambiarse de modo SLEEP{00} a Modo STANDBY{01}
void rxlora ()
{
  opmodeLora();										//Vuelve a colocar el MODO DESDE SLEEP{00}-> STANDBY{01}
  //El modo STANDBY es requerido para cargar datos al FIFO
  opmode(OPMODE_STANDBY);

#if (defined CFG_1278_DRF1278F)  ||  (defined CFG_1272_SEMTECH)                        //DEFINE la version/fabricante del SX1278 de DORJI o SX1272 de SEMTECH
  writeReg(RegDioMapping1, MAP_DIO0_LORA_RXDONE|MAP_DIO1_LORA_RXTOUT|MAP_DIO2_LORA_NOP|MAP_DIO3_PAYLOADCRCERROR);
#endif
#if defined(CFG_1278_NICERF1278)                          //Selecciona el CHIP SX1278 de NICERF
  writeReg(RegDioMapping1, MAP_DIO0_LORA_RXDONE|MAP_DIO1_LORA_RXTOUT|MAP_DIO2_LORA_NOP|MAP_DIO3_LORA_NOP);
#endif

  writeReg(LORARegIrqFlags, 0xFF);					//Limpia todas las banderas. Las banderas se limpian colocondolas en '1'

  writeReg(LORARegIrqFlagsMask, ~(IRQ_LORA_RXDONE_MASK | IRQ_LORA_CRCERR_MASK)); 	//Habilita las interrupciones RXDONE y CRC_ERRORPAYLOAD
  //Habilita las interrupciones con '0'.
#if defined(CFG_1278_NICERF1278)                          //Selecciona el CHIP SX1278 de NICERF
  hal_TX_RX_en(RX_EN_OK);											//Selecciona el modo RX por hardware
#endif
  opmode(OPMODE_RX);																//Cambia a modo RX-> SE DEBERIA HABILITAR UNA INT ANTES
}


//Corresponde a la funcion general que configura todos los parametros de inicio del MODEM LORA
//La primera parte gnera 16 bytes aleatorios para el "calentamiento - warm up" el modem a partir de ruido RSSI
void radio_init ()											//Esta funcion solo se realiza una vez
{
  radio_hal_init();										//Inicia formalmente por hardware el chip radio
  opmode(OPMODE_SLEEP);									//Coloca el modo en SLEEP antes de comenzar con la configuracion

  radio_Checkpoint_Version();								//Comprobacion de comunicacion entre MC y LoRa

  opmodeLora();																//Vuelve a colocar el MODO DESDE SLEEP-> STANDBY
  opmode(OPMODE_STANDBY);														//El modo STANDBY es requerido para configurar el modem
  configLoraModem();															//Configura el modem LORA en relacion con la velocidad de TX y recepcion de los datos con los valores de RADIO.sf RADIO.bw RADIO.cr
  configChannel();															//Configura el Canal de acuerdo con la frecuencia de RADIO.freq en 3 registros
  configPower();																//Configura la potencia a Max potencia RADIO.txpow.
  //Potencia limitada hasta 20dBm.
  writeReg(LORARegPayloadMaxLength, MAX_LEN_FRAME);										//Determina que el maximo payload sera de 64 Bytes para TX o RX
  writeReg(LORARegSymbTimeoutLsb, 0);		 									//Escribe que el tiempo Timeout de simbolos es 0
  writeReg(LORARegSyncWord, LORA_MAC_PREAMBLE);																			//Configura PA ramp-up time (RADIO.PaRamp)
  opmode(OPMODE_SLEEP);														//Vuelve a colocar el MODO a SLEEP
}


//Atencion por Software al ISR
//Continuacion de la funcion hal_io_check (IRQ a nivel de Hardware)
void radio_irq_handler (u1_t errorCRC) 				//Despues de una interrupcion por RX o TX -> El radio pasa de SLEEP/RXMODE/TXMODE-> STANDBY
{
  u1_t flags = readReg(LORARegIrqFlags);									//Almacena el valor del registro flags del LORA

  if( flags & IRQ_LORA_TXDONE_MASK )										//Detecta si la interrupcion fue por la comprobacion de una TX exitosa con el bit3 del registro flags
  {
    RADIO.flagTx = 1;													//Activa bandera de TX realizada
  }
  else if( flags & IRQ_LORA_RXDONE_MASK )									//Detecta si la interrupcion fue el LORA contar con una RX que requiere gestionar datos de la FIFO.
  {																		//bit6 del registro flags
#if (defined CFG_1278_DRF1278F)  ||  (defined CFG_1272_SEMTECH)                        //DEFINE la version/fabricante del SX1278 de DORJI o SX1272 de SEMTECH
    if( (flags & IRQ_LORA_CRCERR_MASK) || (errorCRC != 0) )						//Comprueba si existio error por CRC RX Payload COMPRUEBA SI EXISTE ERROR EN PAYLOADCRC
    {																		//Esto lo puede comprobar por Hardware con el pin DIO3 o por Software con el bit5 del registro flags
      RADIO.crc = 1;
    }
#endif

#if defined(CFG_1278_NICERF1278)                          //Selecciona el CHIP SX1278 de NICERF
    if( (flags & IRQ_LORA_CRCERR_MASK) )						//Comprueba si existio error por CRC RX Payload COMPRUEBA SI EXISTE ERROR EN PAYLOADCRC
    {																		//Software con el bit5 del registro flags
      RADIO.crc = 1;
    }
#endif


    RADIO.dataLenRX = readReg(LORARegRxNbBytes);                            //El modem asume el HEADER como EXPLICITO. Esto fue configurado en el moden con el registro LORARegModemConfig1{1D} bit2->0
    if(RADIO.dataLenRX <= MAX_LEN_FRAME)                                    //detecta si es menor que la cantidad de datos permitida
    {
      //Hace la lectura el COMIENZO de direccion del ULTIMO paquete recibido LORARegFifoRxCurrentAddr{10}
      //Escribe dicho valor en LORARegFifoAddrPtr{0D} que corresponder al pointer del buffer FIFO del LORA
      writeReg(LORARegFifoAddrPtr, readReg(LORARegFifoRxCurrentAddr));

      //Utilizada para leer sobre la FIFO del LORA
      //RegFiFO-> Direccion inicial FIFO
      //RADIO.frameRX-> Vector referencia donde se escribiron los datos
      //RADIO.dataLenRX -> Longitud de datos a escribir
      readBuf(RegFifo, RADIO.frameRX, RADIO.dataLenRX);
      //Lee los parametros de calidad del ultimo paquete recibido
      RADIO.snr  = readReg(LORARegPktSnrValue);                 //Estimacion SNR [dB] * 4
      RADIO.rssi = readReg(LORARegPktRssiValue);     // RSSI [dBm] (-196...+63)
      RADIO.flagRx = 1;                                        //Activa bandera de RX realizada
    }
    else                    //Datos llegaron con error por exceso en los datos permitidos
    {
      RADIO.crc = 1;
    }
  }

  writeReg(LORARegIrqFlagsMask, 0xFF);					//Deshabilita todas las interrupciones LORA
  writeReg(LORARegIrqFlags, 0xFF);						//Limpia todos los flags de las interrupciones


#if defined(CFG_1278_NICERF1278)                          //Selecciona el CHIP SX1278 de NICERF
  hal_TX_RX_en(TX_RX_EN_SLEEP);											//Selecciona el modo SLEEP por hardware
#endif
  opmode(OPMODE_SLEEP);									//Cambia de modo STANDBY a modo SLEEP
}



//Establece el modo de uso del radio para TX o RX
void radio_mode (u1_t mode)
{
  switch (mode)
  {
    case RADIO_TX:
      /*
         Ingresa
         RADIO.frameTX			(Buffer de datos a TX)
         RADIO.dataLenTX  		(cantidad de satos a TX)

         Modifica
         LORARegFifoTxBaseAddr{0E}
         LORARegFifoAddrPtr{0D}
         LORARegPayloadLength{22}
         */
      txlora();

      break;
    case RADIO_RX:
      /*
         Carga los datos de RADIO.frameRX al FIFO del LORA. La cantidad de datos es de RADIO.dataLenRX
         */
      rxlora();
      break;
    case RADIO_RST:
      opmode(OPMODE_SLEEP);
      break;
  }
  //hal_enableIRQ_RXTXLORA();					//Habuilita las interrupciones
}






// Inicia formalmente el uso del radio utilizando el pin de reset

void radio_hal_init ()										//Utiliza el protocolo de arranque del chip propuesto por Semtech
{
  hal_pin_rst(2);											//Coloca el pin de RESET del modem como FLOTANTE (pin que antes era salida, ahora es entrada)*/
  hal_delay(1);											//Hace un delay de 100us (exigido por el fabricante*/
#if defined(CFG_1278_NICERF1278)                          //Selecciona el CHIP SX1278 de NICERF*/
  hal_TX_RX_en(TX_RX_EN_SLEEP);											//Selecciona el modo SLEEP*/
#endif


#if defined(CFG_1272_SEMTECH)                          //Selecciona el CHIP SX1272*/
  hal_pin_rst(1); 										//Realiza un reset en el MODEM RST->1, hace el reset manual (SX1272)*/
  hal_delay(1);											//Hace un delay de 100us (exigido por el fabricante*/
  hal_pin_rst(0);											//Coloca el pin de RESET del modem A CERO**modificacion de la libreria
#endif


#if (defined CFG_1278_DRF1278F)  ||  (defined CFG_1278_NICERF1278)    //Selecciona el CHIP SX1278*/
  hal_pin_rst(0); 										//Realiza un reset en el MODEM RST->0, hace el reset manual (SX1278)*/
  hal_delay(1);											//Hace un delay de 100us (exigido por el fabricante*/
  hal_pin_rst(1);											//Coloca el pin de RESET del modem a UNO
#endif

  hal_delay(5);											//Hace un delay de 5ms recomendado por el fabricante*/
}


//Funcion util para controbar si la comunicacion o conexion con el LoRa han sido correctas. Modo debugger
void radio_Checkpoint_Version()
{
  u1_t versionCheckpoint;
  versionCheckpoint = readReg(RegVersion);
  //Punto de checkpoint
}



//Utilizada para trasnscribir los datos del buf  al vector utilizado por el Lora para TX --> En arduino NO se usa
void radio_buffer_to_frameTX (xref2u1_t buf, u1_t len)
{
  u1_t i;
  i = 0;
  RADIO.dataLenTX = len;
  for (i = 0 ; i < len ; i++)		//Escribe los datos de LMIC.frame[..] hasta alcanzar la  cantidad de datos LMIC.dataLen
  {
    RADIO.frameTX[i] = buf[i];
  }
}


