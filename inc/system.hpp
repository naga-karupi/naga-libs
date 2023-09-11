/*
 * sleep.hpp
 *
 *  Created on: Jul 10, 2022
 *      Author: naga
 */

//#include "main.h"

#ifndef NAGA_LIBS_SLEEP_HPP_
#define NAGA_LIBS_SLEEP_HPP_

#include <vector>
#include <tuple>
#include <memory>
#include <mutex>
#include <concepts>

namespace naga_libs::stm32
{

template <typename ObjectType = void*>
class process
{
	static ObjectType share_obj;

	bool is_running;
	bool is_stop;

	static ObjectType getType()
	{
		return ObjectType();
	}

	friend class system;

public:
	process(): is_running(true), is_stop(false)
	{

	}


	static ObjectType& getShareObject()
	{
		static_assert(not std::is_same<void*, ObjectType>(), "You need set share ObjectType");
		return share_obj;
	}

	virtual void pause() noexcept
	{
		is_running = true;
	}

	virtual void resume() noexcept
	{
		is_running = true;
	}

	virtual void stop()
	{
		is_stop = true;
	}

	/**
	 * @brief Get the Status object
	 * 
	 * @return constexpr std::tuple<bool, bool> 
	 */
	constexpr std::tuple<bool, bool> getStatus() 
	{
		return {is_running, is_stop};
	}

	virtual void setup() = 0;
	virtual void loop() = 0;
};

/**
 * @brief processの派生型だけを受け取れるようにしたコンセプト
 * 
 * @tparam ProcessDerivedClass 
 * @tparam ShareObjectType 
 */
template <class ProcessDerivedClass, class ShareObjectType = void*>
concept processType = requires (ProcessDerivedClass t)
{
	t.loop();
	t.setup();
	ProcessDerivedClass::getShareObject();
}
&& std::derived_from<ProcessDerivedClass, process<ShareObjectType>>;

class system
{
public:
	static float freq;

	/**
	 * @brief 最大1kHzまで対応だけど、読めるクロックの関係で1ms単位でしか見れないので下手な周波数選ぶといい精度が出ない可能性がある
	 * 
	 * @param frequency 
	 */
	static void set_frequency(float frequency)
	{
		freq = frequency;
	}

	/// @brief 制御周期の制御や待機中の他の処理
	/// @return エラー検知でwhileから抜ける

	/**
	 * @brief whileの管理
	 * 
	 * @return true 異常なし
	 * @return false 異常あり
	 */
	static bool update()
	{
		return true;
	}

	/// @brief 処理の実装
	template <typename ShareObjectType>
	class impl{
		using SpProcess = process<ShareObjectType>;
	public:
		inline static std::vector<std::shared_ptr<SpProcess>> processes{0};
	};

	template <typename ProcessDerived>
	requires processType<ProcessDerived, decltype(ProcessDerived::getType())>
	static void Add() 
	{
		using shareObjectType = decltype(ProcessDerived::getType());
		using spProcess = process<shareObjectType>;

		auto p = std::make_shared<ProcessDerived>();
		
		impl<shareObjectType>::processes.push_back(std::dynamic_pointer_cast<spProcess>(p));
		
	}
};
} // namespace naga_libs::stm32

#endif /* NAGA_LIBS_SLEEP_HPP_ */
