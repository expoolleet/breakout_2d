#pragma once

#include "fmod.hpp"

class AudioManager {
  private:
    FMOD::System *m_system = nullptr;

    AudioManager() = default;
    ~AudioManager();

  public:
    AudioManager(const AudioManager &) = delete;
    AudioManager operator=(const AudioManager &) = delete;
    static AudioManager &Get();
    bool init();
    void update();
};
