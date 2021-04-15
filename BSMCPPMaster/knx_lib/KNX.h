/*
 * KNX.h
 *
 * Created: 12/17/2020 12:22:04 PM
 *  Author: Betul Kaplan
 */ 


#ifndef KNX_H_
#define KNX_H_

class knx_base;
class knx_ObjectBase;
class knx_device;
class knxObjectContainer;

class indAdd{
	public:
	uint16_t Value;
	indAdd();
	uint8_t area();
	uint8_t line();
	uint8_t no();
		
	};
struct assocItem{
	
	uint8_t address, object;
	
	};
	
union UassocItem{
	uint16_t word;
	assocItem item;
};

#define RXBufferSize 40
#define RXDataSize 30
class knx_base{
	
	public:
	/*knx_ObjectBase* objects;*/
	uint8_t load_state[5];
	void load_control();
	uint8_t device_state[5];
	void device_control();
	bool isWaitingForTransmit;
	bool requestedToWaitForTransmit;
	knx_ObjectBase* writingObject=NULL;
	
	knx_base(); // constructor
	
	indAdd indAddress;
	uint16_t actualIndAddr;
	
	void connect();
	void disconnect();
	bool frameIsToMe();
	virtual int GroupAddIsExistent(uint16_t group_add);
	virtual uint16_t getFirstGroupAddressofObj(uint8_t object_index); // brings the first group address the object matched with

	virtual void writeObjects(uint16_t group_address, uint8_t* value,knx_ObjectBase*);
	virtual knx_ObjectBase* getObject(int index);
	
	uint8_t mask_version[3];
	uint8_t serial_no[10];
	uint8_t manufacturer_id[6];
	uint8_t hardware_type[10];
	
	uint16_t parameter_area;
	uint16_t groupAddress_area;
	uint16_t associationTable_area;	
	uint16_t objFlags_area;
	uint16_t dummy_add;
	void setParameters();
	void setGroupAddresses();
	void setAssociations();
	void setFlags();
	
	
	
	
	bool connected;
	bool progMode;
	uint8_t RXptr;
	uint8_t RXbuff[RXBufferSize];
	uint8_t RXtelType;
	uint8_t  RXpriority;
	uint16_t  RXsrcAdd;
	uint16_t  RXtargetAdd;
	uint8_t  RXtargetType;
	uint8_t  RXrountingCNT;
	uint8_t  RXdataSize;
	uint8_t  RXframeSize;
	bool isRXControlPocket; //Control or Data
	bool isRXNumbered; // Numbered or Unnumbered
	uint8_t RXsequence; // if numbered Sequence number
	uint8_t  RXdata[RXDataSize];
	uint8_t  RXdataPtr;
	uint8_t  RxApciNible1;
	uint8_t  RxApciNible2;
	uint8_t  ConnectionState;


	uint8_t  RXcrc;
	uint32_t  frameTimoutCNT;
	uint16_t  RXencodedApci;
	uint8_t  RXapciLength;

	//*****************************************************************//
	uint8_t*  TXcontrolField;
	uint16_t*  TXsource;
	uint16_t*  TXtarget;
	uint8_t  TXtargetType;
	uint8_t  TXrountingCNT;

	uint8_t TXptr;
	uint8_t TXsize;
	uint8_t TXdataSize;
	uint8_t TXpocketType; // UDP, NDP, UCP,NCD
	uint8_t TXbuff[100];
	uint8_t TXbusy;
	void setTXbusy(uint8_t);
	uint8_t getTXbusy();
	uint16_t TxApci;
	uint8_t TxApciNible1;
	uint8_t TxApciNible2;
	uint8_t TXcrc;
	uint8_t TXstate;
	uint8_t TXsequence;
	uint8_t TXmode;
	uint8_t TXwaitCnt;
	uint8_t TXended_status;
	void (*endTX)();
	
	uint8_t ack_buff[40];
	uint8_t ack_size;



	
	void init();
	virtual void timer_init();
	virtual void com_init();
	virtual void device_init();
	void TXbuff_init(uint8_t control_field, uint16_t target, uint8_t target_type, uint8_t data_size, uint16_t apci, uint8_t *data);
	
	void on_RXchar(char);
	virtual void TXchar(char);
	void on_TXchar();
	void startTX(uint8_t, uint8_t, uint8_t);
	void on_Timer();
	uint16_t RXapciDecode();
	void onFrame();
	
	//--APCI services--//
	void individualAddReadres();
	void individualAddWriteres();
	void memoryReadres();
	void memoryWriteres();
	void deviceDescriptorReadres();
	void propertyValueReadres();
	void propertyValueWriteres();
	void Restartres();
	void authorizeRequestres();
	void groupValueWriteres();
	void UnDocumented();

	
	virtual void setIndAddress(uint16_t);
	virtual uint16_t getIndAddress();
	virtual void wait(uint16_t);
	void setChipAddress(uint16_t newAdd);
	void ack();
	void t_ack();
	
	virtual void writeToMemory(uint16_t startAdd, void *buffer, uint16_t size);
	virtual void readFromMemory(uint16_t startAdd, void *buffer, uint16_t size);
	
	void groupValueWrite_init(knx_ObjectBase*, uint16_t target);
	

	
	
	};


class knx_ObjectBase
{
	public:
	
	knx_ObjectBase();
		
	uint8_t index;	
	char* data_buff;
	uint8_t data_size;
		
	uint8_t* getValue();
	virtual void setValue(uint8_t*);

	
	bool getBoolValue();
	virtual void setBoolValue(bool);
	
	uint8_t getByteValue();
	virtual void setByteValue(uint8_t);
	
	uint16_t getWordValue();
	void setWordValue(uint16_t);
	
	uint32_t getDwordValue();
	void setDwordValue(uint32_t);
	
	unsigned long getUlongValue();
	void setUlongValue(unsigned long);
	
	float getFloatValue();
	void setFloatValue(float);
	
	double getDoubleValue();
	void setDoubleValue(double);
	
	void (*onChange)(uint8_t );


	void valueChanged();
	
	uint16_t groupValueWrite_taskState;
	void groupValueWrite_task();
	virtual knx_base* get_knx();
	
};

class knx_boolObject: public knx_ObjectBase
{

};


#endif /* KNX_H_ */