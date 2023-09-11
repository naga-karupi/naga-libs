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
class Process
{
protected:
	bool is_running;
	bool is_stop;

public:
	Process(): is_running(true), is_stop(false), setuped(false)
	{

	}

	virtual ~Process()
	{

	}

	virtual void Pause()
	{
		is_running = true;
	}

	virtual void Resume()
	{
		is_running = true;
	}

	virtual void Stop()
	{
		is_stop = true;
	}

	/**
	 * @brief Get the Status object
	 * 
	 * @return constexpr std::tuple<bool, bool> 
	 */
	constexpr std::tuple<bool, bool> GetStatus() const noexcept
	{
		return {is_running, is_stop};
	}

	virtual void Setup() = 0;
	virtual void Loop() = 0;
};

/**
 * @brief processの派生型だけを受け取れるようにしたコンセプト
 * 
 * @tparam ProcessDerivedClass 
 * @tparam ShareObjectType 
 */
template <class ProcessDerivedClass>
concept ProcessType = std::derived_from<ProcessDerivedClass, Process>;

template <typename ShareObjectType = void*>
class System
{
	static uint64_t start_tim;

	void Sleep();

public:
	static float freq;

	/**
	 * @brief 最大1kHzまで対応だけど、読めるクロックの関係で1ms単位でしか見れないので下手な周波数選ぶといい精度が出ない可能性がある
	 * 
	 * @param frequency 
	 */
	static void SetFrequency(float frequency)
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
	static bool Update()
	{
		return true;
	}

	class ShareObject
	{
		inline static ShareObjectType share_obj;

		static ShareObjectType GetType()
		{
			return ShareObjectType();
		}

	public:

		static ShareObjectType& GetShareObject()
		{
			static_assert(not std::is_same<void*, ShareObjectType>(), "You need set share ObjectType");
			return share_obj;
		}

	};

	/// @brief 処理の実装
	class impl{
	public:
		inline static std::vector<std::shared_ptr<Process>> Processes{0};
	};

	template <typename ProcessDerived>
	requires ProcessType<ProcessDerived>
	static void Add() 
	{
		auto p = std::make_shared<ProcessDerived>();
		impl::Processes.push_back(std::dynamic_pointer_cast<Process>(p));
		
	}
};
} // namespace naga_libs::stm32

#endif /* NAGA_LIBS_SLEEP_HPP_ */
