#include "threadPoolTest.h"

#include "core.h"
#include "threading/threading.h"
#include "window/window.h"

#include <sstream>
#include <unordered_map>

using namespace Threading;

namespace
{
    const std::string LABEL_CHARS = "ABCDEFGHIJ";
    const int MAX_THREADS = 10;
    const int THREAD_ID_ARRAY_SIZE = MAX_THREADS*2;

    struct ThreadRegistration
    {
        std::thread::id id;
        int label = -1;
    };
    ThreadRegistration threadIdentification[THREAD_ID_ARRAY_SIZE];
    int threadIdentificationLookup[MAX_THREADS];
    std::mutex threadIdentificationMutex;
    int nextThreadLabel = 0;

    int fibonacciCalls[MAX_THREADS];
    int fibonacciResults[MAX_THREADS];
    int lastFibonacciIndex = -1;

    void Thread_Register()
    {
        {
            std::lock_guard<std::mutex> lock(threadIdentificationMutex);
            std::thread::id id = std::this_thread::get_id();
            int index = std::hash<std::thread::id>{}(id) % THREAD_ID_ARRAY_SIZE;
            for (int i = 0; i < THREAD_ID_ARRAY_SIZE; ++i)
            {
                if (threadIdentification[index].id == id)
                    break;
                if (threadIdentification[index].label == -1)
                {
                    threadIdentification[index].label = nextThreadLabel;
                    threadIdentification[index].id = id;
                    threadIdentificationLookup[nextThreadLabel] = index;
                    nextThreadLabel++;
                    return;
                }
                index = (index + 1) % THREAD_ID_ARRAY_SIZE;
            }
        }
        ThreadPool::Submit(Thread_Register);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    int GetThreadIndex(std::thread::id id)
    {
        int index = std::hash<std::thread::id>{}(id) % THREAD_ID_ARRAY_SIZE;
        for (int i = 0; i < THREAD_ID_ARRAY_SIZE; ++i)
        {
            if (threadIdentification[index].id == id)
                return threadIdentification[index].label;
            index = (index + 1) % THREAD_ID_ARRAY_SIZE;
        }
        return -1;
    }
    char GetThreadLabel(std::thread::id id)
    {
        return LABEL_CHARS[GetThreadIndex(id)];
    }
    void ResetRegistration()
    {
        for (int i = 0; i < THREAD_ID_ARRAY_SIZE; ++i)
            threadIdentification[i] = ThreadRegistration();
        for (int i = 0; i < MAX_THREADS; ++i)
            threadIdentificationLookup[i] = -1;
        nextThreadLabel = 0;
    }

    void Thread_Fibonacci(int index)
    {
        int i = GetThreadIndex(std::this_thread::get_id());
        fibonacciCalls[i]++;

        if (index <= 0)
            return;
        else if (index == 1)
            fibonacciResults[i]++;
        else
        {
            ThreadPool::Submit(std::bind(Thread_Fibonacci, index - 1));
            ThreadPool::Submit(std::bind(Thread_Fibonacci, index - 2));
        }
    }
    void ResetFibonacci()
    {
        for (int i = 0; i < MAX_THREADS; ++i)
        {
            fibonacciCalls[i] = 0;
            fibonacciResults[i] = 0;
        }
        lastFibonacciIndex = -1;
    }
}

int threadPoolTest()
{
    //Create window
    Application::Window window("threadPool tests", 1280, 720, PLATFORM_SDL_RENDER_FLAG);
    window.Create();

    ResetRegistration();
    ResetFibonacci();
    
    bool showDemoWindow = true;
    bool initialized = false;
    int threadCount = 1;
    int fibonacciIndex = 0;

    //Create loop
    bool doStuff = true;
    while (doStuff)
    {
        // Sleep if window is not visible
        if (window.IsMinimized())
        {
            SDL_Delay(10);
            continue;
        }

        bgfx::touch(0);

        ImGui_Implbgfx_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("ThreadPoolTest", nullptr, 
            ImGuiWindowFlags_AlwaysAutoResize);
        if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Config"))
            {
                ImGui::SliderInt("Thread Count", &threadCount, 1, MAX_THREADS);
                ImGui::BeginDisabled(initialized);
                if (ImGui::Button("Initialize"))
                {
                    ThreadPool::Initialize(threadCount);
                    for (int i = 0; i < threadCount; ++i)
                        ThreadPool::Submit(Thread_Register);
                    initialized = true;
                }
                ImGui::EndDisabled();
                ImGui::SameLine();
                ImGui::BeginDisabled(!initialized);
                if (ImGui::Button("Shutdown"))
                {
                    ThreadPool::Shutdown();
                    ResetRegistration();
                    ResetFibonacci();
                    initialized = false;
                }
                ImGui::EndDisabled();
                for (int i = 0; i < MAX_THREADS; ++i)
                {
                    bool registered = threadIdentificationLookup[i] >= 0;
                    std::stringstream ss;
                    ss << "Thread " << LABEL_CHARS[i];
                    if (registered)
                        ss << " - Running";
                    ImGui::Text(ss.str().c_str());
                }
                ImGui::EndTabItem();
            }
            ImGui::BeginDisabled(!initialized);
            if (ImGui::BeginTabItem("Fibonacci"))
            {
                ImGui::SliderInt("Index", &fibonacciIndex, 0, 30);
                if (ImGui::Button("Calculate"))
                {
                    ResetFibonacci();
                    lastFibonacciIndex = fibonacciIndex;
                    ThreadPool::Submit(std::bind(Thread_Fibonacci, fibonacciIndex));
                }
                int result = 0;
                for (int i = 0; i < MAX_THREADS; ++i)
                    result += fibonacciResults[i];
                std::stringstream ss;
                ss << "fibonacci(" << lastFibonacciIndex << ") = " << result;
                ImGui::Text(ss.str().c_str());
                ImGui::SeparatorText("Number of calls");
                for (int i = 0; i < MAX_THREADS; ++i)
                {
                    std::stringstream ss;
                    ss << "Thread " << LABEL_CHARS[i] << " - " << fibonacciCalls[i];
                    ImGui::Text(ss.str().c_str());
                }
                ImGui::EndTabItem();
            }
            ImGui::EndDisabled();
            ImGui::EndTabBar();
        }
        ImGui::End();

        ImGui::ShowDemoWindow(&showDemoWindow);
        ImGui::Render();
        ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

        bgfx::frame();

        // Events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    doStuff = false;
                    break;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    doStuff = false;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                {
                    window.Refresh();
                    break;
                }
                default: break;
            }
        }
    }

    ThreadPool::Shutdown();
    window.Destroy();

    return EXIT_SUCCESS;
}
