/**
 * @file RE.hpp
 * @author Nishinaga Rikuto
 * @brief 
 * @version 0.1
 * @date 2023-09-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include<stdint.h>
#include<cmath>

#ifndef NAGA_LIBRARIES_ROTARY_ENCODER_HPP__
#define NAGA_LIBRARIES_ROTARY_ENCODER_HPP__

#ifdef STM32_NAGA_LIB
#include<main.h>
#endif //STM32_NAGA_LIB

namespace naga_libs
{

class encoder 
{
	[[nodiscard]]
	virtual uint32_t operator() () noexcept = 0;

	virtual void set_zero() () noexcept = 0;

	[[nodiscard]]
	virtual uint32_t read() () noexcept const = 0;
};

#if ARDUINO_NAGA_LIB
//実装しないよ
class Arduino_Rotary_Encoder
{

};
#endif //ARDUINO_NAGA_LIB

#if STM32_NAGA_LIB

class RE_stm32 : encoder
{
	TIM_HandleTypeDef*htim;
public:
	explicit RE_stm32(TIM_HandleTypeDef *_htim);

	/**
	 * @brief その時のタイマの値を読み出し、その後タイマの値を0にセットします。0にしたくないならば別readメソッドを使ってください
	 * 
	 * @return uint32_t 
	 */
	[[nodiscard]]
	uint32_t operator() () noexcept override;

	/**
	 * @brief タイマの値を0にセットします。
	 * 
	 */
	void set_zero() noexcept override;

	/**
	 * @brief タイマの値を読み出します。0にはセットしません。
	 * 
	 * @return uint32_t 
	 */
	[[nodiscard]]
	uint32_t read() noexcept const override;

	RE_stm32() = delete;
};
#endif //STM32_NAGA_LIB

}
#endif //NAGA_LIBRARIES_ROTARY_ENCODER_HPP__
