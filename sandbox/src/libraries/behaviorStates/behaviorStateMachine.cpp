
#include "behaviorStateMachine.h"

using namespace std;
using namespace BehaviorStateMachine;

template <typename T>
State<T>::State()
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

}

template <typename T>
bool Behavior<T>::TransitionTo(State<T>&)
{

}

template <typename T>
bool Behavior<T>::Add(T, State<T>&)
{

}

template <typename T>
bool Behavior<T>::Remove(T)
{

}

template <typename T>
bool Behavior<T>::Remove(State<T>&)
{

}

template <typename T>
bool Behavior<T>::Contains(T)
{

}

template <typename T>
bool Behavior<T>::Contains(State<T>&)
{

}

template<typename T>
State<T>& Behavior<T>::GetStateById(T)
{
	// TODO: insert return statement here
}

template<typename T>
vector<State<T>&> BehaviorStateMachine::Behavior<T>::GetAllStates()
{
	return vector<State<T>&>();
}



