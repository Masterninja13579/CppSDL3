
//#include "experiments/bgfxTriangle.h"
#include "libraries/behaviorStates/behaviorStateMachine.h"

#include <iostream>

int main(int argc, char** argv)
{
    BehaviorStateMachine::Behavior<int> myBehaviorStateMachine;


    //try
    //{
    //    int result = bgfxTest();
    //    std::cout << (result == EXIT_SUCCESS ? "Success\n" : "Failed\n");
    //    return result;
    //}
    //catch(const std::exception& e)
    //{
    //    std::cout << "Program crashed: " << e.what() << '\n';
    //    return EXIT_FAILURE;
    //}

    std::cout << "Success!\n";
}
