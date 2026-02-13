#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

struct ShaderObserveInfo {
    fs::file_time_type lastWriteTime;
    std::string path;
};

struct ReloadTask {
    unsigned int *programID;
    std::vector<std::string> paths;
};

class ShaderObserver {
  private:
    std::unordered_map<unsigned int *, std::vector<ShaderObserveInfo>> m_shadersToObserve;
    std::chrono::steady_clock::time_point m_lastCheck;
    std::vector<ReloadTask> m_reloadTasks;
    std::atomic<bool> m_running{false};
    unsigned int m_updateTimeMS = 500;
    std::condition_variable m_cv;
    std::thread m_workingThread;
    std::mutex m_mutex;
    ShaderObserver() = default;
    ~ShaderObserver();
    void _checkFiles();

  public:
    ShaderObserver(const ShaderObserver &) = delete;
    ShaderObserver operator=(const ShaderObserver &) = delete;
    static ShaderObserver &Get();
    void registerShader(unsigned int &programID, std::vector<std::string> shaderPaths);
    void update();
    void startObserving();
    void stopObserving();
};
