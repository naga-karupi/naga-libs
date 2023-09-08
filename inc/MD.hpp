#pragma once

#include<cmath>
#include<stdint.h>

#ifdef STM32_NAGA_LIB
#include<main.h>
#endif //STM32_NAGA_LIB

namespace naga_libs{
class MD {
public:
	virtual void operator () (int32_t) = 0;

	virtual void set_reverse(bool reverse_ = false) noexcept = 0;
};

#if ARDUINO_NAGA_LIB
//未実装(使わないし)
class MD_Arduino{
	const uint8_t PIN_A, PIN_B, pwm_pin;
	int pwm;
public:
	MD_Arduino(uint8_t _PIN_A, uint8_t _PIN_B, uint8_t _pwm_pin):PIN_A(_PIN_A), PIN_B(_PIN_B), pwm_pin(_pwm_pin){}
	~MD_Arduino() = default;
	void operator () (int re);
};
#endif //ARDUINO_NAGA_LIB

#if STM32_NAGA_LIB
struct quad_MD_info 
{
	GPIO_TypeDef* port_A; uint16_t PIN_A;
	GPIO_TypeDef* port_B; uint16_t PIN_B;

	TIM_HandleTypeDef* pwm_port;
	uint32_t pwm_channel;

	quad_MD_info(quad_MD_info&&);
	quad_MD_info(GPIO_TypeDef* _port_A, uint16_t _PIN_A, GPIO_TypeDef* _port_B, uint16_t _PIN_B, TIM_HandleTypeDef* _pwm_port, uint32_t _pwm_channel);
	quad_MD_info() = delete;

};

class quad_MD : public MD 
{
	bool is_reverse;
	int32_t pwm;

	quad_MD_info info;

public:
	explicit quad_MD(GPIO_TypeDef*, uint16_t, GPIO_TypeDef*, uint16_t, TIM_HandleTypeDef *, uint32_t);
	explicit quad_MD(quad_MD_info&);
	explicit quad_MD(quad_MD_info&&);

	void operator ()(int32_t) override;
	void set_reverse(bool reverse_ = false) noexcept override;

	quad_MD() = delete;

};
#endif //STM32_NAGA_LIB
};