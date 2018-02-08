/*
 * dbgserial.cpp
 *
 *  Created on: Dec 6, 2016
 *      Author: compi
 */

#include <sg/usart.h>
#include <stdlib.h>
#include <sg/itlock.h>
#include <sg/strutil.h>

using namespace sg;

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	UsartCallbackDispatcher::Instance().Callback(huart, UsartCallbackDispatcher::IUsartCallback::TxCpltCallback);
}

void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	UsartCallbackDispatcher::Instance().Callback(huart, UsartCallbackDispatcher::IUsartCallback::TxHalfCpltCallback);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UsartCallbackDispatcher::Instance().Callback(huart, UsartCallbackDispatcher::IUsartCallback::RxCpltCallback);
}
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	UsartCallbackDispatcher::Instance().Callback(huart, UsartCallbackDispatcher::IUsartCallback::RxHalfCpltCallback);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	UsartCallbackDispatcher::Instance().Callback(huart, UsartCallbackDispatcher::IUsartCallback::ErrorCallback);
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
bool UsartCallbackDispatcher::Register(IUsartCallback &handler)
{
	IUsartCallback	**hp = nullptr;

	for( auto& h : m_handlers) {
		if( h == &handler )
			return true;
		if(!hp && !h)
			hp = &h;
	}
	if(!hp)
		return false;

	*hp = &handler;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
void UsartCallbackDispatcher::Callback(UART_HandleTypeDef *huart, IUsartCallback::CallbackType type)
{
	for(auto& handler : m_handlers) {
		if(handler && handler->UsartCallback(huart, type))
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
Usart::Usart( UART_HandleTypeDef *huart, UsartCallbackDispatcher &disp, char *outputBuffer, uint16_t outputBufferSize, bool block)
{
	m_huart = huart;
	m_outputBuffer = outputBuffer;
	m_outputBufferSize = outputBufferSize;
	m_block = block;

	disp.Register(*this);
}

////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef Usart::FillTxBuffer(const uint8_t *buffer, uint16_t &count)
{
	HAL_StatusTypeDef	st = HAL_OK;
	uint16_t   			free, freestart, tocopy, canCopy;

	do
	{
		{
			ItLock	lock;
			freestart = m_txStart + m_txCount;
			free = m_outputBufferSize - m_txCount;
		}
		freestart -= (freestart >= m_outputBufferSize) ? m_outputBufferSize : 0;
		canCopy = count > free ? free : count;
		tocopy = freestart + canCopy > m_outputBufferSize ? m_outputBufferSize - freestart : canCopy;

		memcpy(m_outputBuffer + freestart, buffer, tocopy);

		uint16_t tmp;
		{
			ItLock	lock;
			tmp = m_txCount;
			m_txCount += tocopy;
		}
		if(!tmp)
			st = HAL_UART_Transmit_IT(m_huart, reinterpret_cast<uint8_t*>(m_outputBuffer + freestart), tocopy);

		if(st == HAL_OK) {
			count -= tocopy;
			canCopy -= tocopy;
			buffer += tocopy;
		}
	}
	while(canCopy && st == HAL_OK);

	return st;
}

////////////////////////////////////////////////////////////////////
bool Usart::UsartCallback(UART_HandleTypeDef *huart, CallbackType type)
{
	HAL_StatusTypeDef	st;

	if(huart != m_huart)
		return false;
	if(m_inputBuffer && type == CallbackType::RxCpltCallback)
	{
		m_receiverCallback->LineReceived(m_inputBuffer, m_inputBufferSize - huart->RxXferCount);
	}
	else if(m_txCount && type == CallbackType::TxCpltCallback)
	{
		m_txCount -= huart->TxXferSize;
		m_txStart += huart->TxXferSize;
		if(m_txStart >= m_outputBufferSize)
			m_txStart -= m_outputBufferSize;
		auto chunkSize = m_txStart + m_txCount > m_outputBufferSize ? m_outputBufferSize - m_txStart : m_txCount;
		if(chunkSize)
			st = HAL_UART_Transmit_IT(m_huart, reinterpret_cast<uint8_t*>(m_outputBuffer + m_txStart), chunkSize);
	}

	(void)st;

	return true;
}

////////////////////////////////////////////////////////////////////
uint16_t  Usart::Send(const void *buffer, uint16_t count)
{
	uint16_t			requestedSize = count;
	HAL_StatusTypeDef	st;

	while(count) {
		while(m_txCount == m_outputBufferSize)
			if(!m_block)
				return requestedSize - count;

		if((st = FillTxBuffer((uint8_t*)buffer + requestedSize - count, count)) != HAL_OK)
				return requestedSize - count;
	}

	return requestedSize;
}

////////////////////////////////////////////////////////////////////
uint16_t  Usart::Send(char c)
{
	return Send(&c, 1);
}

////////////////////////////////////////////////////////////////////
uint16_t Usart::Send(bool b)
{
	return Send(b ? '1' : '0');
}

////////////////////////////////////////////////////////////////////
uint16_t Usart::Send(uint32_t u, bool hex, bool prefix, bool pad)
{
	if(hex) {
		uint16_t ret = 0;
		if(prefix)
			ret += Send("0x");
		uint8_t	*pb = ((uint8_t *)((&u) + 1) - 1);
		uint8_t b;
		for(uint8_t byte = 0; byte < sizeof(u); ++byte) {
			b = *pb >> 4;
			if(pad || b)
				ret += Send(tochr(b));
			if(b) pad = true;
			b = *pb & 0xF;
			if(pad || b)// || !byte)
				ret += Send(tochr(b));
			if(b) pad = true;
			--pb;
		}
		return ret;
	} else {
		char buffer[13];
		todec(buffer, u);
		return Send(buffer, strlen(buffer));
	}
}

////////////////////////////////////////////////////////////////////
uint16_t Usart::Send(uint16_t u, bool hex, bool prefix, bool pad)
{
	if(hex) {
		uint16_t  ret = 0;
		if(prefix)
			ret += Send("0x");
		uint8_t	*pb = ((uint8_t *)((&u) + 1) - 1);
		uint8_t b;
		for(uint8_t byte = 0; byte < sizeof(u); ++byte) {
			b = *pb >> 4;
			if(pad || b)
				ret += Send(tochr(b));
			if(b) pad = true;
			b = *pb & 0xF;
			if(pad || b || !byte)
				ret += Send(tochr(b));
			--pb;
		}
		return ret;
	} else {
		char    buffer[8];
		todec(buffer, u);
		return Send(buffer, strlen(buffer));
	}
}


////////////////////////////////////////////////////////////////////
uint16_t Usart::Send(uint8_t u, bool hex, bool prefix)
{
	if(hex) {
		uint16_t  ret = 0;
		if(prefix)
			ret += Send("0x");
		ret += Send(tochr(u >> 4));
		ret += Send(tochr(u & 0x0F));
		return ret;
	} else {
		char    buffer[4];
		todec(buffer, u);
		return Send(buffer, strlen(buffer));
	}
}

////////////////////////////////////////////////////////////////////
uint16_t Usart::Send(const char *str)
{
	return Send((void *)str, strlen(str));
}

////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef Usart::Receive(char *buffer, uint16_t bufferSize, IReceiverCallback &callback, void *callbackUserPtr)
{
	HAL_StatusTypeDef st;

	if( m_inputBuffer ) {
		HAL_UART_AbortReceive(m_huart);
		m_inputBuffer = nullptr;
	}

	m_inputBufferSize = bufferSize;
	m_receiverCallback = &callback;
	m_receivedCallbackUserPtr = callbackUserPtr;
	m_inputBuffer = buffer;

	if((st = HAL_UART_Receive_IT(m_huart, reinterpret_cast<uint8_t*>(m_inputBuffer), m_inputBufferSize)) != HAL_OK)
		m_inputBuffer = nullptr;

	return st;
}

