#define STM32_NAGA_LIB 1

#include "MD.hpp"

using namespace naga_libs;

quad_MD_info::quad_MD_info(quad_MD_info& cp) 
: port_A(cp.port_A), PIN_A(cp.PIN_A), 
  port_B(cp.port_B), PIN_B(cp.PIN_B),
  pwm_port(cp.pwm_port), pwm_channel(cp.pwm_channel) 
{
}

quad_MD_info::quad_MD_info(quad_MD_info&& cp) 
: port_A(cp.port_A), PIN_A(cp.PIN_A), 
  port_B(cp.port_B), PIN_B(cp.PIN_B),
  pwm_port(cp.pwm_port), pwm_channel(cp.pwm_channel) 
{
}

quad_MD_info::quad_MD_info(GPIO_TypeDef* _port_A, uint16_t _PIN_A, GPIO_TypeDef* _port_B, uint16_t _PIN_B, TIM_HandleTypeDef* _pwm_port, uint32_t _pwm_channel)
: port_A(_port_A),PIN_A(_PIN_A), port_B(_port_B),PIN_B(_PIN_B), pwm_port(_pwm_port), pwm_channel(_pwm_channel) 
{
}

explicit quad_MD::quad_MD(GPIO_TypeDef* port_A, uint16_t PIN_A, GPIO_TypeDef* port_B, uint16_t PIN_B, TIM_HandleTypeDef *pwm_port, uint32_t pwm_channel) 
: info(port_A, PIN_A, port_B, PIN_B, pwm_port, pwm_channel)
{
	HAL_TIM_PWM_Start(info.pwm_port, info.pwm_channel);
}

explicit quad_MD::quad_MD(quad_MD_info& _info)
: info(_info)
{
    HAL_TIM_PWM_Start(info.pwm_port, info.pwm_channel);
}

explicit quad_MD::quad_MD(quad_MD_info&& _info)
: info(_info)
{
    HAL_TIM_PWM_Start(info.pwm_port, info.pwm_channel);
}

void quad_MD::operator () (int32_t _pwm) override
{
    pwm = _pwm * ((not is_reverse) ? 1 : -1);
    HAL_GPIO_WritePin(port_A, PIN_A, pwm >= 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(port_B, PIN_B, pwm <  0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(pwm_port, pwm_channel, std::abs(pwm));
}

void quad_MD::set_reverse(bool reverse = false) noexcept override
{
    is_reverse = reverse;
}