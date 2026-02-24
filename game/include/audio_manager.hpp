#pragma once

#include <fmod_studio.hpp>
#include <unordered_map>

class AudioManager {
  private:
    FMOD::Studio::System *m_system = nullptr;
    std::unordered_map<std::string, FMOD::Studio::EventDescription *> m_loadedEventDesc;

    AudioManager() = default;
    ~AudioManager();

  public:
    AudioManager(const AudioManager &) = delete;
    AudioManager operator=(const AudioManager &) = delete;
    static AudioManager &Get();
    void init();
    void update();
    void loadBank(const std::string &name);
    void playOneShot(const std::string &eventPath);
    void changeGlobalParameter(const std::string &name, float value);
};
