#pragma once

#include <functional>
#include <string>
#include <map>

namespace BehaviorStateMachine
{
	class Behavior;
	struct State
	{
		State();
		std::function<void(Behavior)>* in;
		std::function<void>* out;
		std::function<void>* update;
	};
	class Behavior
	{
	public:
		Behavior();
		State* current;
		void Update();
		bool TransitionTo(const std::string&);
		bool TransitionTo(int);
		bool TransitionTo(State&);
		bool Add(const std::string&, State&);
		bool Add(int, State&);
		bool Add(int, const std::string&, State&);
		bool Remove(const std::string&);
		bool Remove(int);
		bool Remove(State&);
		bool Contains(const std::string&);
		bool Contains(int);
		bool Contains(State&);
	private:
		std::map<int, State&> mStatesById;
		std::map<std::string, State&> mStatesByName;
	};

}

