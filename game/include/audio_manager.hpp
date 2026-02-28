#pragma once

#include <fmod_studio.hpp>
#include <unordered_map>

class AudioManager {
   private:
    FMOD::Studio::System *m_system = nullptr;
    std::unordered_map<std::string, FMOD::Studio::EventDescription *> m_loadedEventDesc;

    void _saveEvent(const std::string &name);
    AudioManager() = default;
    ~AudioManager();

   public:
    AudioManager(const AudioManager &) = delete;
    AudioManager operator=(const AudioManager &) = delete;
    static AudioManager &Get();
    void init();
    void update();
    void loadBank(const std::string &name);
    void playOnce(const std::string &eventPath);
    void playOnce(const std::string &eventPath, glm::vec2 position);
    void changeGlobalParameter(const std::string &name, float value);
};
