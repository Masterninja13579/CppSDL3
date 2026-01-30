#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace BehaviorStateMachine
{
	template <typename T>
	class Behavior;

	template <typename T>
	struct State
	{
		State();
		std::function<void(Behavior<T>)> in;
		std::function<void> out;
		std::function<void> update;
	};

	template <typename T> 
	class Behavior
	{
	public:
		Behavior();
		State<T>* current;
		void Update();
		bool TransitionTo(T);
		bool TransitionTo(State<T>&);
		bool Add(T, State<T>&);
		bool Remove(T);
		bool Remove(State<T>&);
		bool Contains(T);
		bool Contains(State<T>&);
		State<T>& GetStateById(T);
		std::vector<State<T>&> GetAllStates();
	private:
		std::unordered_map<T, State<T>&> mStatesById;
	};

}

