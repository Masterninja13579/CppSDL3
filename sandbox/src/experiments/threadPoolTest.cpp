#include "threadPoolTest.h"

#include "core.h"
#include "threading/threading.h"
#include "window/window.h"

#include <sstream>
#include <unordered_map>

// DEBUG
#include <iostream>

using namespace Threading;

// Constants
namespace
{
    const std::string LABEL_CHARS = "ABCDEFGHIJ";
    const int MAX_THREADS = 10;
    const int THREAD_ID_ARRAY_SIZE = MAX_THREADS*2;

    const int COLOR_GRID_WIDTH = 15;
    const int COLOR_GRID_HEIGHT = 10;
    const int SLEEP_PER_FADE = 10; // milliseconds
    const float MAX_FADE_AMOUNT = 0.02f;
    const float COLOR_CELL_OUTLINE = 2.0f;
    const ImVec2 COLOR_CELL_SIZE = ImVec2(20, 20);
    const ImVec2 COLOR_CELL_SPACING = ImVec2(4, 4);
    const ImVec4 DEFAULT_GRID_COLOR = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    const ImVec4 DEFAULT_COLORS[MAX_THREADS] = 
    {
        ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
        ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
        ImVec4(0.0f, 0.0f, 1.0f, 1.0f),
        ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
        ImVec4(0.0f, 1.0f, 1.0f, 1.0f),
        ImVec4(1.0f, 0.0f, 1.0f, 1.0f),
        ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
        ImVec4(0.5f, 1.0f, 0.5f, 1.0f),
        ImVec4(0.5f, 0.5f, 1.0f, 1.0f),
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
    };
}

// Thread registration
namespace
{
    struct ThreadRegistration
    {
        std::thread::id id;
        int label = -1;
    };
    ThreadRegistration threadIdentification[THREAD_ID_ARRAY_SIZE];
    int threadIdentificationLookup[MAX_THREADS];
    std::mutex threadIdentificationMutex;
    int nextThreadLabel = 0;

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
}

// Fibonacci
namespace
{
    int fibonacciCalls[MAX_THREADS];
    int fibonacciResults[MAX_THREADS];
    int lastFibonacciIndex = -1;

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

// Color Grid
namespace
{
    ImVec4 colorGrid[COLOR_GRID_WIDTH][COLOR_GRID_HEIGHT];
    int colorGridActive[COLOR_GRID_WIDTH][COLOR_GRID_HEIGHT];
    ImVec4 gridClearColor = DEFAULT_GRID_COLOR;

    bool IncrementalFade(float target, float& current)
    {
        if (target == current)
            return true;
        //std::cout << "IncrementalFade(" << target << ", " << current << ")\n";
        float difference = target - current;
        float magnitude = std::abs(difference);
        //std::cout << "    difference: " << difference << "\n";
        //std::cout << "    magnitude: " << magnitude << "\n";
        if (magnitude < MAX_FADE_AMOUNT)
        {
            current = target;
            return true;
        }
        if (difference > 0)
            current += MAX_FADE_AMOUNT;
        else
            current -= MAX_FADE_AMOUNT;
        return false;
    }
    void Thread_ColorFade(int x, int y)
    {
        int index = GetThreadIndex(std::this_thread::get_id());
        colorGridActive[x][y] = index;
        const ImVec4& targetColor = DEFAULT_COLORS[index];
        ImVec4& currentColor = colorGrid[x][y];
        while (!IncrementalFade(targetColor.x, currentColor.x) ||
               !IncrementalFade(targetColor.y, currentColor.y) ||
               !IncrementalFade(targetColor.z, currentColor.z))
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_PER_FADE));
        colorGridActive[x][y] = -1;
    }

    void ResetActiveColorGrid()
    {
        for (int y = 0; y < COLOR_GRID_HEIGHT; y++)
            for (int x = 0; x < COLOR_GRID_WIDTH; x++)
                colorGridActive[x][y] = -1;
    }
    void RandomizeColorGrid()
    {
        for (int y = 0; y < COLOR_GRID_HEIGHT; y++)
        {
            for (int x = 0; x < COLOR_GRID_WIDTH; x++)
            {
                int r = rand() % 256;
                int g = rand() % 256;
                int b = rand() % 256;
                ImVec4 color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
                colorGrid[x][y] = color;
            }
        }
    }
    void SetColorGrid(const ImVec4& color)
    {
        for (int y = 0; y < COLOR_GRID_HEIGHT; y++)
            for (int x = 0; x < COLOR_GRID_WIDTH; x++)
                colorGrid[x][y] = color;
    }
}

int threadPoolTest()
{
    //Create window
    Application::Window window("threadPool tests", 1280, 720, PLATFORM_SDL_RENDER_FLAG);
    window.Create();

    ResetRegistration();
    ResetFibonacci();
    ResetActiveColorGrid();
    RandomizeColorGrid();
    
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
                    ResetActiveColorGrid();
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
                    ImGui::Indent();
                    ImGui::Text(ss.str().c_str());
                    ImGui::Unindent();
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
            if (ImGui::BeginTabItem("Colors"))
            {
                if (ImGui::Button("Start Fade"))
                {
                    for (int y = 0; y < COLOR_GRID_HEIGHT; ++y)
                        for (int x = 0; x < COLOR_GRID_WIDTH; ++x)
                            ThreadPool::Submit(std::bind(Thread_ColorFade, x, y));
                }
                ImGui::SameLine();
                if (ImGui::Button("Randomize Colors"))
                {
                    RandomizeColorGrid();
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear Grid"))
                {
                    SetColorGrid(gridClearColor);
                }
                ImGui::SameLine();
                ImGui::ColorEdit3(
                    "##GridClearColor",
                    (float*)&gridClearColor,
                    ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::SeparatorText("Active Threads");
                for (int i = 0; i < MAX_THREADS; ++i)
                {
                    if (threadIdentificationLookup[i] < 0)
                        break;
                    if (i % 5 != 0)
                        ImGui::SameLine();
                    std::stringstream childStream;
                    childStream << "ThreadColorLabel##" << i;
                    ImVec4 color = DEFAULT_COLORS[i];
                    std::stringstream textStream;
                    textStream << "Thread " << LABEL_CHARS[i];
                    std::string text = textStream.str();
                    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, color);
                    if (ImGui::BeginChild(
                        childStream.str().c_str(), 
                        ImVec2(textSize.x + 6, textSize.y + 2),
                        ImGuiChildFlags_None))
                    {
                        const float COLOR_THRESHOLD = 1.3f;
                        ImVec2 cursor = ImGui::GetCursorPos();
                        ImGui::SetCursorPos(ImVec2(cursor.x + 3, cursor.y + 1));
                        float total = 0.9f*color.x + 1.4f*color.y + 0.7f*color.z;
                        if (total > COLOR_THRESHOLD)
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
                        ImGui::Text(text.c_str());
                        if (total > COLOR_THRESHOLD)
                            ImGui::PopStyleColor();
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleColor();
                }
                ImGui::SeparatorText("Color Grid");
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                const ImVec2 gridStart = ImGui::GetCursorScreenPos();
                for (int y = 0; y < COLOR_GRID_HEIGHT; ++y)
                {
                    for (int x = 0; x < COLOR_GRID_WIDTH; ++x)
                    {
                        float px = gridStart.x + x*COLOR_CELL_SIZE.x + x*COLOR_CELL_SPACING.x;
                        float py = gridStart.y + y*COLOR_CELL_SIZE.y + y*COLOR_CELL_SPACING.y;
                        float pxe = px + COLOR_CELL_SIZE.x;
                        float pye = py + COLOR_CELL_SIZE.y;
                        const ImU32 color = ImColor(colorGrid[x][y]);
                        drawList->AddRectFilled(
                            ImVec2(px, py), 
                            ImVec2(pxe, pye),
                            color);
                        if (colorGridActive[x][y] >= 0)
                            drawList->AddRect(
                                ImVec2(px - COLOR_CELL_OUTLINE, py - COLOR_CELL_OUTLINE),
                                ImVec2(pxe + COLOR_CELL_OUTLINE, pye + COLOR_CELL_OUTLINE),
                                ImColor(DEFAULT_COLORS[colorGridActive[x][y]]),
                                0.0f,
                                ImDrawFlags_None,
                                COLOR_CELL_OUTLINE);
                    }
                }
                {
                    float width = COLOR_GRID_WIDTH * (COLOR_CELL_SIZE.x + COLOR_CELL_SPACING.x);
                    float height = COLOR_GRID_HEIGHT * (COLOR_CELL_SIZE.y + COLOR_CELL_SPACING.y);
                    ImGui::Dummy(ImVec2(width, height));
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
