#pragma once

#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "observer_ptr.hpp"

namespace fs = std::filesystem;

struct ShaderObserverInfo {
    fs::file_time_type lastWriteTime;
    std::string path;
};

struct ReloadTask {
    unsigned int *programID;
    std::vector<std::string> paths;
};

class ShaderObserver {
   private:
    std::unordered_map<unsigned int *, std::vector<ShaderObserverInfo>> m_shadersToObserve;
    std::vector<ReloadTask> m_reloadTasks;
    std::atomic<bool> m_running{false};
    unsigned int m_updateTimeMS = 500;
    std::condition_variable m_cv;
    std::thread m_workingThread;
    std::mutex m_mutex;

    void _checkFiles();

   public:
    ShaderObserver() = default;
    ~ShaderObserver() noexcept;

    ShaderObserver(const ShaderObserver &) = delete;
    ShaderObserver &operator=(const ShaderObserver &) = delete;
    ShaderObserver(ShaderObserver &&) = delete;
    ShaderObserver &operator=(ShaderObserver &&) = delete;

    void registerShader(unsigned int &programID, std::vector<std::string> shaderPaths);
    void update();
    void startObserving();
    void stopObserving();
};

using ShaderObserverPtr = observer_ptr<ShaderObserver>;
