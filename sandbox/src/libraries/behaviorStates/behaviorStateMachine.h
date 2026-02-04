#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace BehaviorStateMachine
{
	struct State
	{
		State()
			: in(nullptr)
			, out(nullptr)
			, update(nullptr)
		{

		}

		State(std::function<void()> updateFunc)
			: in(nullptr)
			, out(nullptr)
			, update(updateFunc)
		{

		}

		State(std::function<void()> inFunc, std::function<void()> outFunc, std::function<void()> updateFunc)
			: in(inFunc)
			, out(outFunc)
			, update(updateFunc)
		{

		}

		std::function<void()> in;
		std::function<void()> out;
		std::function<void()> update;
	};



	template <typename T> 
	class Behavior
	{
	public:
		Behavior()
			: current(nullptr)
			, mStatesById()
		{

		}

		State* current;

		void Update()
		{
			if (current == nullptr)
			{
				return;
			}

			State& currentRef = *current;
			if (currentRef.update != nullptr)
			{
				currentRef.update();
			}
		}

		/// <summary>
		/// Transition to the state with the corresponding stateId.
		/// </summary>
		/// <param name="stateId"></param>
		/// <returns></returns>
		bool TransitionTo(T stateId)
		{
			if (Contains(stateId))
			{
				TransitionTo(mStatesById.at(stateId));
				return true;
			}
			else
			{
				return false;
			}
		}

		/// <summary>
		/// Force the behavior transition to the state.
		/// </summary>
		/// <param name="nextState"></param>
		void TransitionTo(State& nextState)
		{
			if (current != nullptr)
			{
				current->out();
			}
			current = &nextState;
			nextState.in();
		}

		/// <summary>
		/// Adds to state to the Behavior.
		/// </summary>
		/// <param name="stateId"></param>
		/// <param name="state"></param>
		/// <returns></returns>
		bool Add(T stateId, State& state)
		{
			return mStatesById.insert({ stateId, state }).second;
		}

		/// <summary>
		/// Removes the key/value pair where the key is equal to stateId.
		/// </summary>
		/// <param name="stateId"></param>
		/// <returns></returns>
		bool Remove(T stateId)
		{
			return mStatesById.erase(stateId) > 0;
		}

		/// <summary>
		/// Removes all key/value pairs where the value is equal to state.
		/// </summary>
		/// <param name="state"></param>
		/// <returns></returns>
		bool Remove(State& state)
		{
			for (auto it = mStatesById.begin(); it != mStatesById.end();)
			{
				if (it->second == state) { mStatesById.erase(it++); }
				else { ++it; }
			}
		}

		/// <summary>
		/// Returns true if the Behavior contains a state associated with the given stateId.
		/// </summary>
		/// <param name="stateId"></param>
		/// <returns></returns>
		bool Contains(T stateId)
		{
			return mStatesById.contains(stateId);
		}

		/// <summary>
		/// Returns true if the Behavior contains a state or states equal to the given state.
		/// </summary>
		/// <param name="state"></param>
		/// <returns></returns>
		bool Contains(const State& state)
		{
			//for (auto it = mStatesById.begin(); it != mStatesById.end(); ++it)
			//{
			//	if (it->second == state) { return true; }
			//}
			for (auto pair : mStatesById)
			{
				if (pair->second == state) { return true; }
			}
			return false;
		}

		/// <summary>
		/// Returns the state for the given stateId. Throws exception if it does not exist.
		/// </summary>
		/// <param name="stateId"></param>
		/// <returns></returns>
		State& GetStateAtId(T stateId)
		{
			return mStatesById.at(stateId);
		}

		/// <summary>
		/// Returns all states owned by the Behavior, not including the current state.
		/// </summary>
		/// <returns></returns>
		std::vector<State> GetAllStates()
		{
			std::vector<State> allStates;
			//for (auto it = mStatesById.begin(); it != mStatesById.end(); ++it)
			//{
			//	allStates.push_back(it->second);
			//}
			for (auto pair : mStatesById)
			{
				allStates.push_back(pair->second);
			}
			return allStates;
		}

	private:
		std::unordered_map<T, State> mStatesById;
	};
}

