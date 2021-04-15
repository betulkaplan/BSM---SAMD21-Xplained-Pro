/*
 * knx_ObjectBase.cpp
 *
 * Created: 1/28/2021 12:15:26 AM
 *  Author: Betul Kaplan
 */ 

#include <stdio.h>
#include <string.h>
#include "KNX.h"
#include <stdlib.h>


 knx_ObjectBase:: knx_ObjectBase()
 {
	 data_size = 1;
	 data_buff =(char*)malloc(data_size);
	 groupValueWrite_taskState = 0;
 }
 
uint8_t* knx_ObjectBase::getValue()
{
	return (uint8_t*)&data_buff;
}

void knx_ObjectBase::setValue(uint8_t* value)
{
	for (uint8_t i=0; i < data_size; i++)
	{
		data_buff[i] = value[i];
	}
	
	valueChanged();

}
	
bool knx_ObjectBase::getBoolValue()
{
	return (bool)data_buff[0];
}
void knx_ObjectBase::setBoolValue(bool value)
{
	data_buff[0] = (uint8_t)value;
	valueChanged();
}

uint8_t knx_ObjectBase::getByteValue()
{
	return (uint8_t)data_buff[0];
}
void knx_ObjectBase::setByteValue(uint8_t value)
{
	data_buff[0] = value;
	valueChanged();
}

uint16_t knx_ObjectBase::getWordValue()
{
	uint16_t res;
	memcpy(&res, data_buff, 2);
	return res;
}
void knx_ObjectBase::setWordValue(uint16_t value)
{
	memcpy(data_buff, &value, 2);
	valueChanged();
}

uint32_t knx_ObjectBase::getDwordValue()
{
	uint32_t res;
	memcpy(&res, data_buff, 4);
	return res;
	
}
void knx_ObjectBase::setDwordValue(uint32_t value)
{
	memcpy(data_buff, &value, 4);
	valueChanged();
}

unsigned long knx_ObjectBase::getUlongValue()
{
	unsigned long res;
	memcpy(&res, data_buff, 8);
	return res;
	
}
void knx_ObjectBase::setUlongValue(unsigned long value)
{
	memcpy(data_buff, &value, 8);
	valueChanged();
}

float knx_ObjectBase::getFloatValue()
{
	float res;
	memcpy(&res, data_buff, 4);
	return res;
	
}
void knx_ObjectBase::setFloatValue(float value)
{
	memcpy(data_buff, &value, 4);
	valueChanged();
}
double knx_ObjectBase::getDoubleValue()
{
	double res;
	memcpy(&res, data_buff, 8);
	return res;
	
}
void knx_ObjectBase::setDoubleValue(double value)
{
	memcpy(data_buff, &value, 8);
	valueChanged();
}

void knx_ObjectBase::valueChanged()
{
	if(onChange!=0) onChange(index);
}
void knx_ObjectBase::groupValueWrite_task()
{
	uint16_t adr ;
	knx_base* knx = get_knx();
	if ((knx->writingObject != NULL) && (knx->writingObject!=this))
	{
		return;
	}
	switch(groupValueWrite_taskState)
	{
		case 0: return;
		break;
		
		case 1:
			knx->requestedToWaitForTransmit=true;
			knx->writingObject = this;
			groupValueWrite_taskState = 2;
		break;
		
		case 2:
		//groupValueWrite_taskState=3;
		
			if (knx->isWaitingForTransmit == true)
			{
				
				adr= knx->getFirstGroupAddressofObj(this->index);
				if (adr == 0)
				{
					knx->requestedToWaitForTransmit=false;
					knx->isWaitingForTransmit = false;
					groupValueWrite_taskState = 0;
					knx->writingObject = NULL;
					knx->setTXbusy(0);				
				}
				else
				{
					knx->TXbuff_init(0xbc, adr, 1, data_size, 128, (uint8_t*)data_buff);
					knx->startTX(knx->TXsize, 0, 0);
					groupValueWrite_taskState=3;
						
				}
				
			}
			
		break;
		
		case 3:
			if(knx->getTXbusy() == 10)
			{
				groupValueWrite_taskState = 4;
			}
		break;
		
		case 4:
			
			groupValueWrite_taskState = 1000;
			
			
		break;
	
		default:
			if (groupValueWrite_taskState>=1000){
				groupValueWrite_taskState++;
				if (groupValueWrite_taskState==1010)
				{
					knx->requestedToWaitForTransmit=false;
					knx->isWaitingForTransmit = false;
					knx->writingObject = NULL;
					groupValueWrite_taskState=0;
					knx->setTXbusy(0);
				}
			}
		break;
		
	}
	
}

knx_base* knx_ObjectBase::get_knx()
{
	return NULL;
}





