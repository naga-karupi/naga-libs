#define STM32_NAGA_LIB 1

#include "RE.hpp"

using namespace naga_libs;

explicit RE_stm32::RE_stm32(TIM_HandleTypeDef *_htim)
: htim(_htim)
{
    HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
}

[[nodiscard]]
uint32_t RE_stm32::operator() () noexcept override
{
    uint32_t counter = __HAL_TIM_GET_COUNTER(htim);
    __HAL_TIM_SET_COUNTER(htim, 0);
    return counter;
}

void RE_stm32::set_zero() noexcept override
{
    _HAL_TIM_SET_COUNTER(htim, 0);
}

[[nodiscard]]
uint32_t RE_stm32::read() noexcept const override 
{
    return __HAL_TIM_GET_COUNTER(htim);
}
