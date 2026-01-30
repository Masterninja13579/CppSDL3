#pragma once

#include "libraries/behaviorStates/behaviorStateMachine.h"

#include <iostream>
#include <string>

using namespace BehaviorStateMachine;

class AudioPlayer
{
    const int STATE_STOP = 0;
    const int STATE_PLAY = 1;

    bool isPlaying;
    int songPosition;
    int songLength;
    std::string songName;

    Behavior<int> stateMachine;
    State stopState;
    State playState;

    void StopIn()
    {
        std::cout << this << " AudioPlayer::StopIn\n";
    }
    void StopUpdate()
    {
        std::cout << this << " AudioPlayer::StopUpdate\n";
    }
    void StopOut()
    {
        std::cout << this << " AudioPlayer::StopOut\n";
    }

    void PlayIn()
    {
        std::cout << this << " AudioPlayer::PlayIn\n";
        songPosition = 0;
        songLength = 0;
        songName = "";
        isPlaying = true;
    }
    void PlayUpdate()
    {
        std::cout << this << " AudioPlayer::PlayUpdate\n";
        songPosition++;
        if (songPosition >= songLength)
        {
            stateMachine.TransitionTo(STATE_STOP);
        }
    }
    void PlayOut()
    {
        std::cout << this << " AudioPlayer::PlayOut\n";
        isPlaying = false;
    }

public:
    AudioPlayer()
        : isPlaying(false)
        , songPosition(0)
        , songLength(0)
        , songName("")
        , stateMachine()
    {
        // Use std::bind to store a non-static function pointer in a std::function object
        // Basically this just connects the function pointer to the specific object instance it applies to
        stopState.in = std::bind(&AudioPlayer::StopIn, this);
        stopState.update = std::bind(&AudioPlayer::StopUpdate, this);
        stopState.out = std::bind(&AudioPlayer::StopOut, this);

        playState.in = std::bind(&AudioPlayer::PlayIn, this);
        playState.update = std::bind(&AudioPlayer::PlayUpdate, this);
        playState.out = std::bind(&AudioPlayer::PlayOut, this);

        stateMachine.Add(STATE_STOP, stopState);
        stateMachine.Add(STATE_PLAY, playState);
        Stop();
    }

    void Stop()
    {
        stateMachine.TransitionTo(STATE_STOP);
        songPosition = 0;
        songLength = 0;
        songName = "";
    }

    void Play(const std::string& name, int length)
    {
        songName = name;
        songLength = length;
        stateMachine.TransitionTo(STATE_PLAY);
    }

    bool IsPlaying()
    {
        return isPlaying;
    }

    void Update()
    {
        stateMachine.Update();
    }
};


int runTest()
{
    // stopIn
    AudioPlayer player;

    // stopUpdate
    player.Update();
    player.Update();
    // stopOut
    // playIn
    player.Play("Can't Describe", 5);
    // playUpdate
    player.Update();
    player.Update();
    player.Update();
    player.Update();
    player.Update();
    // playOut
    // stopIn
    // stopUpdate
    player.Update();
    player.Update();
    player.Update();
    // stopOut
    // playIn
    player.Play("Under the Sun", 7);
    // playUpdate
    player.Update();
    player.Update();
    // playOut
    // stopIn
    player.Stop();
    // stopUpdate
    player.Update();
    player.Update();

    return EXIT_SUCCESS;
}
