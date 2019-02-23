
int main(){


}

/**
 * 	@brief	Tx done callback
 * 	@param	none
 * 	@retval	none
 */

void onTxDone(){
    state_flags |= TX_OK;       //Set flag
	Radio.Rx(0);

#ifdef debug
	Trace_send("-->TX done! \r \n");
#endif

}

/**
 *	@brief 		RX timeout callback
 * 	@param		none
 * 	@pretval	none
 * 
 */
void onTxTimeout(){

    state_flags |= TX_TIMEOUT;

#ifdef debug
    Trace_send("-->TX error: Timeout \t \n");
#endif
}

/**
 * 	@brief	TimeOut Callback 
 * 	@param	none
 * 	@retval	none
 */

void onRxTimeout(){


    state_flags |= RX_TIMEOUT;
	Radio.Sleep( );

#ifdef debug
    Trace_send("Error: Timeout \r \n");
#endif

}


/**
 * 	@brief	Rx Error handle
 * 	@param 	none
 * 	@retval	none
 */

void onRxError(){

    state_flags |= RX_ERROR;

#ifdef debug
	Trace_send("RX ERROR \r \n");	
#endif
}

void onFhssChangeChannel(uint8_t currentChannel){};
void onCadDone(bool channelActivityDetected){}