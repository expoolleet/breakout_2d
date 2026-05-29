#pragma once

#include <fmod_studio.hpp>
#include <unordered_map>

#include "path_manager.hpp"
#include "string_operators.hpp"

class AudioManager {
   private:
    FMOD::Studio::System *m_system = nullptr;
    std::unordered_map<std::string, FMOD::Studio::EventDescription *, string_hash, string_view_equal> m_loadedEventDesc;
    void _saveEvent(std::string_view eventPath);
    PathManagerPtr m_pathManager;

   public:
    AudioManager(PathManagerPtr pathManager);
    ~AudioManager() noexcept;

    AudioManager(const AudioManager &) = default;
    AudioManager &operator=(const AudioManager &) = default;
    AudioManager(AudioManager &&) = default;
    AudioManager &operator=(AudioManager &&) = default;

    void init();
    void update();
    void loadBank(const std::string &name);
    void playOnce(std::string_view eventPath);
    void playOnce(std::string_view eventPath, glm::vec2 position, glm::vec2 velocity = glm::vec2(0.0f));
    void changeGlobalParameter(const std::string &name, float value);
};

using AudioManagerPtr = std::unique_ptr<AudioManager>;
