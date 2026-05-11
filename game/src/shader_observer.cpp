#include "shader_observer.hpp"

#include <chrono>
#include <filesystem>
#include <format>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "custom_attributes.hpp"
#include "logging.hpp"
#include "shader_manager.hpp"

namespace fs = std::filesystem;

void ShaderObserver::_checkFiles() {
    std::lock_guard<std::mutex> l(m_mutex);
    for (auto &[id, files] : m_shadersToObserve) {
        bool changed = false;
        for (auto &[lastWriteTime, file] : files) {
            try {
                fs::file_time_type writeTime = fs::last_write_time(file);
                if (lastWriteTime != writeTime) {
                    lastWriteTime = writeTime;
                    changed = true;
                }
            } catch (fs::filesystem_error &e) {
                logging::Warn("{}", e.what());
                continue;
            }
        }
        if (changed) {
            std::vector<std::string> paths;
            for (auto &f : files) {
                paths.push_back(f.path);
            }
            m_reloadTasks.push_back({id, paths});
        }
    }
}

ShaderObserver::~ShaderObserver() noexcept {
    if (m_running) m_running = false;
    m_cv.notify_all();
    m_workingThread.join();
}

void ShaderObserver::registerShader(unsigned int &programID, std::vector<std::string> shaderPaths) {
    if (m_shadersToObserve.contains(&programID)) {
        logging::Warn("Shader {} is already registered in ShaderObserver instance", programID);
        return;
    }
    try {
        for (auto &path : shaderPaths) {
            m_shadersToObserve[&programID].push_back({fs::last_write_time(path), path});
        }
    } catch (fs::filesystem_error &e) {
        logging::Error("{}", e.what());
    }
}

void ShaderObserver::update() {
    std::vector<ReloadTask> reloadTasks;
    {  // blocking only moving part
        std::lock_guard<std::mutex> lock{m_mutex};
        if (m_reloadTasks.empty()) return;
        reloadTasks = std::move(m_reloadTasks);  // m_reloadTasks is empty after "moving"
    }
    for (auto &task : reloadTasks) {
        ShaderManager::reloadShader(*task.programID, task.paths);
    }
}

void ShaderObserver::startObserving() {
    if (m_running) return;
    m_running = true;
    m_workingThread = std::thread([this]() {
        while (m_running) {
            _checkFiles();

            std::unique_lock<std::mutex> lock{m_mutex};
            m_cv.wait_for(lock, std::chrono::milliseconds(m_updateTimeMS),
                          [this] { return !m_running; });  // sleep while time is up or m_running is not false
        }
    });
}

void ShaderObserver::stopObserving() {
    m_running = false;
}
