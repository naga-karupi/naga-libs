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
#include <concepts>

#if PC_TEST
#include <iostream>
#include <chrono>
#endif //TEST

namespace naga_libs::stm32
{
/**
 * @brief 処理を記述するクラスの親クラス。ここから派生したクラスを用いて制御する。
 * 
 */
class Process
{
protected:
	bool is_running;
	bool is_stop;

private:
	bool setuped;

public:
	Process(): is_running(true), is_stop(false), setuped(false)
	{

	}

	virtual ~Process()
	{

	}

	/**
	 * @brief 処理の一時停止。デフォルトではフラグのみ
	 * 
	 */
	virtual void Pause()
	{
		is_running = true;
	}

	/**
	 * @brief 一時停止からの復帰。デフォルトではフラグのみ
	 * 
	 */
	virtual void Resume()
	{
		is_running = true;
	}

	/**
	 * @brief 処理の停止。復帰は今の所ない(実装予定)
	 * 
	 */
	virtual void Stop()
	{
		is_stop = true;
	}

	/**
	 * @brief セットアップカンスの実行の必要性の有無
	 * 
	 * @return true セットアップの必要あり
	 * @return false セットアップの必要なし
	 */
	bool NeedSetup() const noexcept
	{
		return not setuped;
	}

	/**
	 * @brief setupが完了したことを表すコード
	 * 
	 */
	void DoneSetup() noexcept
	{
		setuped = true;
	}

	/**
	 * @brief Processクラスの状態を返す
	 * 
	 * @return std::tuple<bool　is_running, bool is_stop> 
	 */
	constexpr std::tuple<bool, bool> GetStatus() const noexcept
	{
		return {is_running, is_stop};
	}

	/**
	 * @brief 最初に一回だけ実行される。オーバーライドし、処理は各自で定義すること。
	 * 
	 */
	virtual void Setup() = 0;

	/**
	 * @brief 一定時間ごとに実行される。オーバーライドし、処理は各自で定義すること。
	 * 
	 */
	virtual void Loop() = 0;
};

/**
 * @brief Processの派生型だけを受け取れるようにしたコンセプト
 * 
 * @tparam ProcessDerivedClass 
 * @tparam ShareObjectType 
 */
template <class ProcessDerivedClass>
concept ProcessType = std::derived_from<ProcessDerivedClass, Process>;

/**
 * @brief 管理クラス
 * 
 * @tparam ShareObjectType 共有する型
 */
template <typename ShareObjectType = void*>
class System
{
	inline static bool once_f{false};

	/**
	 * @brief 触らないこと
	 * 
	 */
	struct impl
	{
		inline static std::vector<std::shared_ptr<Process>> Processes;
		
		class Timer
		{
			inline static uint64_t begin_tim;
			inline static float wait_ms;

		public:
			static void SetFrequency(float frequency)
			{
				wait_ms = 1000.0f / frequency;
			}

			static void Init() 
			{
				
			#if PC_TEST
				using namespace std::chrono;
				auto tp = system_clock::now();
				begin_tim  = duration_cast<microseconds>(tp.time_since_epoch()).count();
			#endif //PC_TEST
				
			}

			static void Sleep()
			{
			#if PC_TEST
				using namespace std::chrono;
				while(1)
				{
					auto tp = system_clock::now();
					auto now  = duration_cast<milliseconds>(tp.time_since_epoch()).count();
					if(now - begin_tim >= wait_ms)
					{
						begin_tim = now;
						break;
					}
					
				}
			#endif //PC_TEST
			}
			
		};
	};


public:
	/**
	 * @brief 最大1kHzまで対応だけど、読めるクロックの関係で1ms単位でしか見れないので下手な周波数選ぶといい精度が出ない可能性がある
	 * 
	 * @param frequency
	 */
	static void SetFrequency(float frequency)
	{
		impl::Timer::SetFrequency(frequency);
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
		[&]() 
		{
			if(once_f) 
				return;

			impl::Timer::Init();
			once_f = true;
		}();
		
		for (const auto& p : impl::Processes) 
		{
			const auto& [is_running, is_stop] = p->GetStatus();

			if(not is_running || is_stop) 
				continue;

			if (p->NeedSetup())
			{
				
				p->Setup();
				p->DoneSetup();
			}
			else
			{
				p->Loop();
			}
		}

		impl::Timer::Sleep();

		return true;
	}

	/**
	 * @brief 共有オブジェクトを提供するクラス。
	 * 
	 */
	class ShareObject
	{
		inline static ShareObjectType share_obj;

	public:
		/**
		 * @brief 共有オブジェクトの変更をこのメソッドの呼び出しから行うことができる
		 * 
		 * @return ShareObjectType& 共有オブジェクトの参照
		 */
		static ShareObjectType& GetShareObject()
		{
			static_assert(not std::is_same<void*, ShareObjectType>(), "You need set share ObjectType");
			return share_obj;
		}

	};

	/**
	 * @brief Processの追加
	 * 
	 * @tparam ProcessDerived Processの派生型
	 */
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
