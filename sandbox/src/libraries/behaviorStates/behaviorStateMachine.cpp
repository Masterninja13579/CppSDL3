
#include "behaviorStateMachine.h"

using namespace std;
using namespace BehaviorStateMachine;

State::State()
	: in(nullptr)
	, out(nullptr)
	, update(nullptr)
{

}

template <typename T>
Behavior<T>::Behavior()
	: current(nullptr)
	, mStatesById()
{
	
}

template <typename T>
void Behavior<T>::Update()
{

}

template <typename T>
bool Behavior<T>::TransitionTo(T)
{
	return false;
}

template <typename T>
bool Behavior<T>::TransitionTo(State&)
{
	return false;
}

template <typename T>
bool Behavior<T>::Add(T, State&)
{
	return false;
}

template <typename T>
bool Behavior<T>::Remove(T)
{
	return false;
}

template <typename T>
bool Behavior<T>::Remove(State&)
{
	return false;
}

template <typename T>
bool Behavior<T>::Contains(T)
{
	return false;
}

template <typename T>
bool Behavior<T>::Contains(State&)
{
	return false;
}

template<typename T>
State* Behavior<T>::GetStateById(T id)
{
	// TODO: insert return statement here
	return mStatesById[id];
}

template<typename T>
vector<State*> Behavior<T>::GetAllStates()
{
	return vector<State*>();
}

template class Behavior<int>;
