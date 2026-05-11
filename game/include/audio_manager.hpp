#pragma once

#include <fmod_studio.hpp>
#include <unordered_map>

class AudioManager {
   private:
    FMOD::Studio::System *m_system = nullptr;
    std::unordered_map<std::string, FMOD::Studio::EventDescription *> m_loadedEventDesc;
    void _saveEvent(const std::string &name);

   public:
    AudioManager() = default;
    ~AudioManager() noexcept;
    void init();
    void update();
    void loadBank(const std::string &name);
    void playOnce(const std::string &eventPath);
    void playOnce(const std::string &eventPath, glm::vec2 position, glm::vec2 velocity = glm::vec2(0.0f));
    void changeGlobalParameter(const std::string &name, float value);
};
