#include "audio_manager.hpp"

#include "custom_attributes.hpp"

#include <fmod.hpp>

AudioManager::~AudioManager() {
    if (m_system) {
        m_system->release();
    }
}

AudioManager &AudioManager::Get() {
    NO_DESTROY_ATTR static AudioManager manager;
    return manager;
}

bool AudioManager::init() {
    FMOD_RESULT result;

    result = FMOD::System_Create(&m_system, FMOD_VERSION);
    if (result != FMOD_OK) {
        return false;
    }

    result = m_system->init(512, FMOD_INIT_NORMAL, nullptr);

    if (result != FMOD_OK) {
        return false;
    }

    return true;
}

void AudioManager::update() {
    if (m_system) {
        m_system->update();
    }
}
