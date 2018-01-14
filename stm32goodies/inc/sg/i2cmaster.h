#ifndef _I2CMASTER_H_
#define _I2CMASTER_H_
#include <sg/stm32_hal.h>
#include <inttypes.h>
#include <string.h>

extern "C" {
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);
}

namespace sg {

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
class I2cCallbackDispatcher
{
public:
	static I2cCallbackDispatcher& Instance() { return m_instance; }

	class II2cCallback
	{
	public:
		enum CallbackType
		{
			None,
			MasterTxCpltCallback,
			MasterRxCpltCallback,
			SlaveTxCpltCallback,
			SlaveRxCpltCallback,
			MemTxCpltCallback,
			MemRxCpltCallback,
			ErrorCallback
		};

	private:
		virtual bool I2cCallback(I2C_HandleTypeDef *hi2c, CallbackType type) = 0;
		friend class I2cCallbackDispatcher;
	};

	bool Register(II2cCallback *handler);

private:
	I2cCallbackDispatcher() { memset(&m_handlers, 0, sizeof(m_handlers)); }
	I2cCallbackDispatcher(const I2cCallbackDispatcher &rhs) = delete;

	void Callback(I2C_HandleTypeDef *hi2c, II2cCallback::CallbackType type);

	static I2cCallbackDispatcher	m_instance;

	II2cCallback 					*m_handlers[2];

	friend void ::HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
	friend void ::HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);
	friend void ::HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);
	friend void ::HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
	friend void ::HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
	friend void ::HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
	friend void ::HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);

};

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
class I2cMaster : public I2cCallbackDispatcher::II2cCallback
{
public:
	enum Mode { Poll, It, Dma };

	typedef HAL_StatusTypeDef Status;

	I2cMaster(I2C_HandleTypeDef *hi2c, I2cCallbackDispatcher *disp = nullptr);

	Status Write(const uint16_t i2cAddress, uint8_t *data, uint8_t size, Mode mode = Poll);
	Status Read(const uint16_t i2cAddress, uint8_t *data, uint8_t size, Mode mode = Poll);
	Status WriteMem(const uint16_t i2cAddress, uint16_t memAddr, uint8_t memAddrSize, uint8_t *data, uint16_t size, Mode mode = Poll);
	Status ReadMem(const uint16_t i2cAddress, uint16_t memAddr, uint8_t memAddrSize, uint8_t *data, uint16_t size, Mode mode = Poll);

	inline uint32_t WaitCallback();
	inline uint32_t	GetCallbackError() { return m_callbackError; }

protected:
	virtual bool I2cCallback(I2C_HandleTypeDef *hi2c, CallbackType type);
	//virtual ~I2cMaster();	//will never be destructed

	I2C_HandleTypeDef			*m_hi2c;
	volatile CallbackType		m_expectedCallback = None;
	uint32_t 					m_callbackError = HAL_I2C_ERROR_NONE;
};

//////////////////////////////////////////////////////////////////////////////
inline uint32_t I2cMaster::WaitCallback()
{
	while(m_expectedCallback != None) {}
	return m_callbackError;
}

}	//	namespace sg

#endif	//	_I2CMASTER_H_