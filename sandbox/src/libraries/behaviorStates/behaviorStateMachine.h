#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace BehaviorStateMachine
{
	template <typename T>
	class Behavior;

	struct State
	{
		State();
		std::function<void()> in;
		std::function<void()> out;
		std::function<void()> update;
	};

	template <typename T> 
	class Behavior
	{
	public:
		Behavior();
		State* current;
		void Update();
		bool TransitionTo(T);
		bool TransitionTo(State&);
		bool Add(T, State&);
		bool Remove(T);
		bool Remove(State&);
		bool Contains(T);
		bool Contains(State&);
		State* GetStateById(T);
		std::vector<State*> GetAllStates();
	private:
		std::unordered_map<T, State*> mStatesById;
	};
}

