/*
 *   lib.cpp
 *
 * Created: 12/17/2020 12:21:09 PM
 * Author : Betul Kaplan
 */ 

#include <stdio.h>
#include <string.h>
#include "KNX.h"

uint32_t transmitTime;

 knx_base:: knx_base(){
	
	
}

void knx_base::init()
{
	int i=0;
	for (i=0;i<RXBufferSize;i++) RXbuff[i]=0;	
	TXcontrolField = (uint8_t*)&TXbuff[0];;
	TXsource = (uint16_t*)&TXbuff[1];
	TXtarget = (uint16_t*)&TXbuff[3];
	
		
	actualIndAddr = getIndAddress();
	timer_init();
	com_init();
	device_init();
	setChipAddress(0xffff);
	TXchar(0x01);

}


void knx_base::wait(uint16_t time){}	
void knx_base::timer_init(){}
void knx_base::com_init(){}
void knx_base::device_init(){}
void knx_base::setChipAddress(uint16_t newAdd)
{
	TXchar(0x28); // U_Set_Address +PhysAddressHigh +PhysAddressLow
	uint8_t h = newAdd>>8;
	uint8_t l = newAdd;
	TXchar(h);
	TXchar(l);	
}

void knx_base::ack()
{
	TXchar(0x11);
}

uint8_t x;	
void knx_base::on_Timer(){
	
	frameTimoutCNT++;
	if (frameTimoutCNT>5)
	{
		if (requestedToWaitForTransmit==true)
		{
			isWaitingForTransmit=true;
		}
	}
	if (TXwaitCnt > 1) TXwaitCnt--;
	else 
	{
		if (TXwaitCnt == 1)
		{
			TXwaitCnt = 0;
			switch (TXmode)
			{
				case 0:
					TXsequence = 0x80;// U_L_DataStart
					TXstate = 0;
					TXcrc = 0;
					TXchar(TXsequence); 
					break;	
				
/*
				case 1:
					TXstate=0;
					TXchar(TXbuff[0]);
					TXbusy = 1;*/
				case 3:
					TXsequence = 0x80;// T-ACK start sequence
					TXstate = 0;
					TXcrc = 0;
					TXchar(TXsequence);
					break;				
				case 4:
					TXsequence = 0x80;// T-ACK start sequence
					TXstate = 0;
					TXcrc = 0;
					TXchar(TXsequence);
					break;	
			}
			
		}
	}
	
}
void knx_base::on_RXchar(char RX_byte) // This is called in receive interrupt and collection of the whole telegram frame is completed here.
{
	if(isWaitingForTransmit)
	{
		RXptr = 0;
		RXcrc = 0;
		return;
	}
	
	if (getTXbusy() == 10)
	{
		if (RX_byte == 0x8b || RX_byte == 0x0b)
		{
			setTXbusy(20);
			
		}
		return;
		
	}
	
	if (  frameTimoutCNT > 5)
	{
		  RXptr = 0;
		  RXcrc = 0;
	}
	if (getTXbusy()==1)
	{
		return;
	}
		
	if ((RXdataPtr>=29)||(RXptr>=39)) {
		  RXptr = 0;
		  RXcrc = 0;
		  RXdataPtr=0;
	}
			
	  frameTimoutCNT = 0;
	  RXbuff[  RXptr] = RX_byte;
	  RXcrc =   RXcrc ^ RX_byte;

	switch(   RXptr )
	{
		case 0:   RXtelType = (RX_byte & 0xF0) >> 4;  // Normal/Retry
		  RXpriority = RX_byte & 0x0F;  // System/Alarm/High/Low
		break;
		case 2: 
		  RXsrcAdd = (  RXbuff[1] << 8) +   RXbuff[2];
		break;
		case 4:   RXtargetAdd = (  RXbuff[3] << 8) +   RXbuff[4];
		break;
		case 5:   RXtargetType =   RXbuff[5] >> 7;
			ack();		
			RXrountingCNT = (  RXbuff[5] >> 4) & 0x07;
			RXdataSize =    RXbuff[5] & 0x0F;
		break;
		case 6:
			isRXControlPocket = RXbuff[6] >> 7; // if the msb is 1 frame is Control Pocket   
			isRXNumbered =   RXbuff[6] >> 6 & 0x01;
			RXsequence = RXbuff[6]>>2 & 0x0f;

		// There is a part we do not control yet:
		// For NDP or NCP case, 4 bit data sequence number knowledge for divided long data.
		  RxApciNible1 =   RXbuff[6] & 0x03;
		  RxApciNible1 =   RxApciNible1 << 2;
		break;
		case 7:
		  RxApciNible1 +=   RXbuff[7] >> 6; // BURAYA 2 defa  giriyor ???
		  RXdataPtr = 0;
		  RXdata[  RXdataPtr] =   RXbuff[7] & 0x3F;
		  RxApciNible2 =   RXdata[  RXdataPtr];
		break;
		
	}
	
	if ((  RXptr > 7) && (  RXptr <   RXdataSize + 7)) // runs when data longer than 1 byte is received
	{
		  RXdataPtr++;
		  RXdata[RXdataPtr] = RXbuff[RXptr];
	}
	
	if ((  RXptr == (  RXdataSize + 7)) /*&& (  RXcrc == 0)*/) // CRC byte received
	{
		
		// frame is successfully received
		  RXencodedApci =   RXapciDecode();

		  onFrame();
	}
	  RXptr++;
}	

void knx_base::onFrame() // Enters here once the telegram receive is completed. How to act upon received telegram is decided here.
{

	RXcrc = 0;
	RXptr = 0;
	if (!frameIsToMe()) return;

	if (RXdataSize == 0)
	{
		uint8_t x = RXbuff[6]&0x01;

		if (x==0)
		{
			connect();
		}		
		else if(x==1) 
			disconnect();
		
		
	}
	
	switch(RXencodedApci)
	{
		case 256: // A_IndividualAddress_Read-PDU - Broadcast
			individualAddReadres();
			break;
			
		case 320: // A_IndividualAddress_Write-PDU
			break;
			
		case 192: // A_IndividualAddress_Reponse-PDU
			individualAddWriteres();
			break;
			
		case 8: // A_Memory_Read-PDU - Point to Point
			memoryReadres();
			break;
			
		case 10:  // A_Memory_Write-PDU - Point to Point
			memoryWriteres();
			break;
			
		case 12: // A_DeviceDescriptor_Read-PDU - Point to Point
			deviceDescriptorReadres();
			break;
			
		case 14: // A_Restart-PDU - Point to Point
			Restartres(); // deactivate programming mode
			break;
			
		case 981: // A_PropertyValue_Read-PDU - Point to Point
			propertyValueReadres();
			break;
			
		case 983: // A_PropertyValue_Write-PDU - Point to Point
			propertyValueWriteres();
			break;
			
		case 977: // A_Authorize_Request-PDU - Point to Point
			authorizeRequestres();
			break;
		case 1: // Connect
			break;
		case 286: // Disconnect
			break;
		case 2: //A_GroupValue_Write-PDU 
			groupValueWriteres();
			break;
			/*		uint8_t dum_data[2]={0x77, 0x33};
			knx.TXbuff_init();
			knx.startTX(0xbc, 0x3030, 1, knx.TXsize, 1, 2, 2, dum_data);
			_delay_ms(5000);*/
		

		default:
			UnDocumented();
	}
}

uint16_t knx_base::RXapciDecode() // Here the Bibles are interpreted for the final evalution
{ 
	
	if ( RxApciNible1 == 0x01 ||  RxApciNible1 == 0x02 ||  RxApciNible1 == 0x06 ||  RxApciNible1 == 0x08 ||  RxApciNible1 == 0x09 ||  RxApciNible1 == 0x0A || RxApciNible1 == 0x0C || RxApciNible1 == 0x0E)
	{
		return ( RxApciNible1);
	}
	else return(( RxApciNible1 << 6) +  RxApciNible2);
	
}

/*
	If the recevied telegram is a Numbered one, the response to this telegram must be numbered and have the same sequence numbered with the one received.
*/

/* 
	Telegram gönderirken 3 tane fonksiyon kullanýyoruz.
	transmit iþlemi interrupt-driven çalýþýr. Bu yüzden ilk byte i yazarýz ve sonra interrupt geldikçe 
	onTXchar devamýný yazmaya devam eder. 
 
	TXchar(): data register a byte yazar
	
	bu üç fonksiyon knxbase classes ininin TXbuff ýný gönderir. 
	
	bir telegram göndereceðinizde önce TXbuff a göndermek istediðiniz telegramý yazarsýnýz fakat burada önemli konu, 
	TPUART transciever yapýya uyumlu olmayan telegramý bus a göndermiyor. O yüzden random telegram gönderemeyeceðimizi bilelim.
	daha sonra da TPUART ýn dahasheet i incelendiðinde göreleceði gibi transicever a telegram gönderirken uyulmasý gereken bir yapý vardýr. 
	Önce bunlar data_start, data_continue ... - data_end dir.  
	
	startTX in size parametresi telegramýný CRC(checksum) hariç boyudur. CRC on_TXchar da hesaplanýr. 

*/

void knx_base::on_TXchar()
{
	
	if (getTXbusy() != 1) //transmit yapýlýyor
	{
		return;
	}

	switch (TXmode){

		case 0:
			switch (TXstate){ // 0 for sending data 1 for sequence
				case 0:
				
				if (TXptr == TXsize)
				{
					TXstate = 2;
					TXchar(~TXcrc);
				}
				else
				{
					TXchar(TXbuff[TXptr]);
					TXcrc ^= TXbuff[TXptr];
					TXptr++;
					TXstate = 1;
	
				}
				break;
				
				case 1: //continuing sequence
				
					if (TXptr == TXsize){
						TXsequence = 0x40 | (TXsequence-0x7f);
					}
					else{
						TXsequence++;
					}
					
					TXchar(TXsequence);
					TXstate=0;
					break;
				case 2: //end sequence
					setTXbusy(10); //trasmit bitti
					break;
				
			}

			break;
			
		case 1:
			if (TXptr == TXsize)
			{
				setTXbusy(10); //trasmit bitti
				return;
			}
			TXptr++;
			TXchar(TXbuff[TXptr]);
			break;	
		case 3:
					switch(TXstate)
			{
				case 0:
					if (TXptr == ack_size)
					{
						TXstate = 2;
						TXchar(~TXcrc);
					}
					else
					{
						TXchar(ack_buff[TXptr]);
						TXcrc ^= ack_buff[TXptr];
						TXptr++;
						TXstate = 1;					
					}
					break;
				case 1:
					if (TXptr == ack_size){
						TXsequence = 0x40 | (TXsequence-0x7f);
					}
					else{
						TXsequence++;
					}
					
					TXchar(TXsequence);
					TXstate=0;
					break;
				case 2:
					setTXbusy(10); //trasmit bitti
					break;
				
			}
			
		case 4:
			switch(TXstate)
			{
				case 0:
					if (TXptr == ack_size)
					{
						TXstate = 2;
						TXchar(~TXcrc);
					}
					else
					{
						TXchar(ack_buff[TXptr]);
						TXcrc ^= ack_buff[TXptr];
						TXptr++;
						TXstate = 1;					
					}
					break;
				case 1:
					if (TXptr == ack_size){
						TXsequence = 0x40 | (TXsequence-0x7f);
					}
					else{
						TXsequence++;
					}
					
					TXchar(TXsequence);
					TXstate=0;
					break;
				case 2:
					startTX(TXsize, 0, 0);
					break;
				
			}
			break;	
	}
}

void knx_base::startTX(uint8_t size, uint8_t tx_mode, uint8_t a_size)
{
	TXended_status = 0;
	TXmode = tx_mode;
	TXptr = 0;
	TXsize = size;
	ack_size=a_size;
	setTXbusy(1); //trasmit baþlýyor
	TXwaitCnt = 20;
	
}

void knx_base::TXchar(char c)
{
	
}

void knx_base::connect()
{
	connected = true;
	
}
void knx_base::disconnect()
{
	connected = false;
	t_ack();
	startTX(0, 3, 7);
}

bool knx_base::frameIsToMe()
{
	switch(RXtargetType)
	{
		case 0: 
			if (RXtargetAdd == actualIndAddr) return true; // Point to point connection
			break;
		case 1:
			if (RXtargetAdd == 0) 
			return true; // Broadcast 	
			if (GroupAddIsExistent(RXtargetAdd)!=-1) 
			return true; // Multicast
			break;	
	}
	
	return false;
	
	
}



void knx_base::TXbuff_init(uint8_t control_field, uint16_t target, uint8_t target_type, uint8_t data_size, uint16_t apci, uint8_t *data)
{ 
	TxApciNible1 = (apci >> 6);
	bool shortApci=(TxApciNible1 == 0x01 ||  TxApciNible1 == 0x02 ||  TxApciNible1 == 0x06 ||  TxApciNible1 == 0x08 ||  TxApciNible1 == 0x09 ||  TxApciNible1 == 0x0A || TxApciNible1 == 0x0C || TxApciNible1 == 0x0E);
	if (!shortApci) // Long Apci condition
	{
		data_size++;
	}
	
	*TXcontrolField = control_field;
	
	uint8_t h = actualIndAddr>>8;
	uint8_t l = actualIndAddr&0x00ff;
	*TXsource = l*0x100+h;
	h = target>>8;

	l = target&0x00ff;
	*TXtarget =  l*0x100+h;
	TXbuff[5] = 0x60; // Default routing counter
	TXbuff[5] |= (target_type == 1)? 128:0;
	TXbuff[5] |= data_size;
	TXbuff[6] = isRXControlPocket<<7;
	TXbuff[6] += isRXNumbered<<6;
	TXbuff[6] += RXsequence<<2;	
	
	TXbuff[6] |= apci>>8;
	TXbuff[7] = apci & 0x00ff;
	TXsize = 7;
	//if(data == NULL) return;
	
	
	
	
	uint8_t shift = 8;
	uint8_t data_ptr = 0;
	if(shortApci)
	{
		if (data!=NULL)
		{
			TXbuff[7] |= data[0]&0x3f;//data[0] & 0xc0;
		}
		else
		{
			TXbuff[7] |= 0&0x3f;//data[0] & 0xc0;
		}
		
		data_ptr = 1;
		shift = 7;
		
	} 
	for (int i=data_ptr; i<data_size; i++)
	{
		if (data!=NULL)
		{
			TXbuff[data_ptr+shift] =  data[i];
		}
		else
		{
			TXbuff[data_ptr+shift] =  0;
			
		}
		
		data_ptr++;
		
	}
		
	if (shortApci)
	{
		TXsize = data_size+7;
	}
	else TXsize = data_size+7;
	
}

//-------START apci services-----------//
void knx_base::individualAddReadres() // When address read received
{
	if (progMode == true)
	{
		TXbuff_init(0xbc, 0, 1, 0, 320, NULL); // Broadcast communication
		startTX(8,0, 0);
	}

}

void knx_base::individualAddWriteres() // Gets the new individual address and writes it into the EEPROM address reserved for it
{
	if (progMode == true)
	{
		indAddress.Value = RXbuff[8];
		indAddress.Value = indAddress.Value << 8;
		indAddress.Value += RXbuff[9];
		setIndAddress(indAddress.Value);
		actualIndAddr = getIndAddress();
		setChipAddress(actualIndAddr);	// to write address to the transceiver
	}
	

}

void knx_base::memoryReadres() // Response to memRead and probably memWrite
{
	//we look at the receive read address and response with information for that address.	
}

void knx_base::memoryWriteres() // Common function is to retreive the tables (Parameter, Address, Association, Flags)
{
	t_ack();

	uint8_t number = RXbuff[7]&0x3F; // extracts number of data bytes after address info
	uint16_t address = RXbuff[8];
	address = address<<8;
	address +=RXbuff[9];
	uint8_t data[20]; //Temporary data buff to write on eeprom
	
	for (int i = 0; i<(number); i++)
	{
		data[i] = RXbuff[10+i];
	}
	
	writeToMemory(address, data, number);
	
	TXbuff_init(0xbc, RXsrcAdd, 0, (number+3), 576, NULL);
	TXbuff[7] += number;
	TXbuff[8] = RXbuff[8]; 
	TXbuff[9] = RXbuff[9];
	
	readFromMemory(address, &TXbuff[10], number);
	
	TXbuff[0]=0xb0;
	startTX(TXsize, 4, 7);

}

void knx_base::deviceDescriptorReadres() // ??
{
	if (connected)
	{

		TXbuff_init(0xbc, RXsrcAdd, 0, 2, 832, mask_version); // Apci is for DeviceDescriptorResponse
		t_ack();
		startTX(TXsize, 4, 7);
	}
	
		/*TXbuff_init//(uint8_t control_field, uint16_t target, uint8_t target_type, uint8_t data_size, uint8_t pocket_type, uint16_t apci, void *data)*/
	
}

void knx_base::Restartres() // Deactivates programming mode
{
	progMode=false;
}

void knx_base::propertyValueReadres() // Load state machine and device related info flow during download
{
	if (connected)
	{
		switch(RXbuff[9]) //Deciding for the PID (serialno, manufacturer id, HWtype, Loadcontrol)
		{
			case 5: //Load control
				load_control();
				TXbuff_init(0xbc, RXsrcAdd, 0, 5, 982, load_state); 
				t_ack();
				startTX(TXsize, 4, 7);
				break;
			case 11: //Asking for serial number
				TXbuff_init(0xbc, RXsrcAdd, 0, 10, 982, serial_no);
				t_ack();
				startTX(TXsize, 4, 7);			
				break;
			case 12: //Manufacturer ID
				TXbuff_init(0xbc, RXsrcAdd, 0, 6, 982, manufacturer_id); 
				t_ack();
				startTX(TXsize, 4, 7);
				break;
			case 78: //HW type
				TXbuff_init(0xbc, RXsrcAdd, 0, 10, 982, hardware_type); 
				t_ack();
				startTX(TXsize, 4, 7);
				break;
			case 14: //Device Control
				device_control();
				TXbuff_init(0xbc, RXsrcAdd, 0, 5, 982, device_state); 
				t_ack();
				startTX(TXsize, 4, 7);
				break;				
		
			default:
				break;	
		}	
	}
}
	
void knx_base::propertyValueWriteres() // ??
{
	if (connected)
	{
		switch(RXbuff[9]) //Deciding for the PID (serialno, manufacturer id, HWtype, Loadcontrol)
		{
			case 5: //Load control
				load_control();
				TXbuff_init(0xbc, RXsrcAdd, 0, 5, 982, load_state); 
				t_ack();
				startTX(TXsize, 4, 7);
				break;			
			case 14: //Device control
				device_control();
				device_state[4] = RXbuff[12];
				TXbuff_init(0xbc, RXsrcAdd, 0, 5, 982, device_state); 
				t_ack();
				startTX(TXsize, 4, 7);
				break;
			default:
				break;	
		}	
	}
	
}

void knx_base::authorizeRequestres() // ??
{
	if (connected)
	{
		t_ack();
		uint8_t data_ser[1] = {0x00};
		TXbuff_init(0xbc, RXsrcAdd, 0, 1, 978, data_ser);
		startTX(TXsize, 4, 7);
	}
}

void knx_base::groupValueWriteres() // 
{
	writeObjects(RXtargetAdd, RXdata,NULL);
}

void knx_base::UnDocumented() // ??
{

}

void knx_base::groupValueWrite_init(knx_ObjectBase* o, uint16_t target)
{
	TXbuff_init(0xbc, target, 1, o->data_size, 2, (uint8_t*)o->data_buff);
	//void knx_base::TXbuff_init(uint8_t control_field, uint16_t target, uint8_t target_type, uint8_t data_size, uint16_t apci, uint8_t *data)
}

//------------END apci service------------//

//-----------Transport Layer Services----------//
void knx_base::t_ack()
{
	ack_buff[0]= 0xbc;
	ack_buff[1]= RXbuff[3];
	ack_buff[2]= RXbuff[4];
	ack_buff[3]= RXbuff[1];
	ack_buff[4]= RXbuff[2];
	ack_buff[5]=0x60;	
	ack_buff[6] = 1<<7;
	ack_buff[6] += 1<<6;
	ack_buff[6] += RXsequence<<2;
	ack_buff[6] += 2;
}

void knx_base::load_control()
{
		load_state[0] = RXbuff[8];
		load_state[1] = 5;
		load_state[2] = 0x10;
		load_state[3] = 0x01;	
		switch(RXbuff[12])
		{
			case 4: // Unload
				load_state[4] = 0x00;
				break;
			case 1: // Start Loading
				load_state[4] = 0x02;
				break;
			case 3: // Additional Load Control
				load_state[4] = 0x02;
			break;
			case 2: // Load Complete
				load_state[4] = 0x01;
			break;						
		}
	
}


void knx_base::device_control()
{
		device_state[0] = RXbuff[8];
		device_state[1] = 14;
		device_state[2] = 0x10;
		device_state[3] = 0x01;		
}

void knx_base::setTXbusy(uint8_t value)
{
	TXended_status = value;
}

uint8_t knx_base::getTXbusy()
{
	return TXended_status;
}

void knx_base::setParameters()
{
	

}

void knx_base::setGroupAddresses()
{

}

void knx_base::setAssociations()
{

}

void knx_base::setFlags()
{

}

//----------- END Transport Layer Services----------//

void knx_base::setIndAddress(uint16_t addr)
{
	
}
	
uint16_t knx_base::getIndAddress(){
	return 0;
}
	

indAdd::indAdd(){

}
uint8_t indAdd::area(){
	return(Value&0xF000)>>12;
}
uint8_t indAdd::line(){
	return(Value&0x0F00)>>8;
}
uint8_t indAdd::no(){
	return(Value&0x00FF);
}

void knx_base::writeToMemory(uint16_t startAdd, void *buffer, uint16_t size)
{
	
}
void knx_base::readFromMemory(uint16_t startAdd, void *buffer, uint16_t size)
{
	
}
int knx_base::GroupAddIsExistent(uint16_t group_add){
	return false;
	
}

void knx_base::writeObjects(uint16_t group_address, uint8_t* value,knx_ObjectBase* source)
{
	
	
}

knx_ObjectBase* knx_base::getObject(int index){
	return NULL;
}


uint16_t knx_base::getFirstGroupAddressofObj(uint8_t object_index)
{
	return 0;
	
}

