#include "audio_manager.hpp"

#include <fmod_errors.h>

#include <fmod_studio.hpp>
#include <string>

#include "custom_attributes.hpp"
#include "logging.hpp"

#ifdef _DEBUG
#define ERRCHECK(result)                                                      \
    if (result != FMOD_OK) {                                                  \
        std::cerr << "FMOD Error: " << FMOD_ErrorString(result) << std::endl; \
    }

#else
#define ERRCHECK(result) (result)
#endif

void AudioManager::_saveEvent(std::string_view eventPath) {
    if (m_loadedEventDesc.contains(eventPath)) return;
    FMOD::Studio::EventDescription *eventDesc = nullptr;
    ERRCHECK(m_system->getEvent(eventPath.data(), &eventDesc));
    m_loadedEventDesc[eventPath.data()] = eventDesc;
}
AudioManager::AudioManager(PathManagerPtr pathManager) : m_pathManager(pathManager) {}

AudioManager::~AudioManager() noexcept {
    logging::Info("AudioManager is destroyed properly. FMOD is released");
    ERRCHECK(m_system->release());
}

void AudioManager::init() {
    ERRCHECK(FMOD::Studio::System::create(&m_system));
    ERRCHECK(m_system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));
}

void AudioManager::loadBank(const std::string &name) {
    FMOD::Studio::Bank *mainBank = nullptr;
    FMOD::Studio::Bank *stringsBank = nullptr;
    std::string bankPath = m_pathManager->getResourcePath("fmod/" + name);
    ERRCHECK(m_system->loadBankFile((bankPath + "/" + name + ".bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &mainBank));
    ERRCHECK(m_system->loadBankFile((bankPath + "/" + name + ".strings.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank));
}

void AudioManager::playOnce(std::string_view eventPath) {
    _saveEvent(eventPath);
    FMOD::Studio::EventInstance *instance = nullptr;
    auto it = m_loadedEventDesc.find(eventPath);
    ERRCHECK(it->second->createInstance(&instance));
    ERRCHECK(instance->start());
    ERRCHECK(instance->release());
}

void AudioManager::playOnce(std::string_view eventPath, glm::vec2 position, glm::vec2 velocity) {
    _saveEvent(eventPath);
    FMOD::Studio::EventInstance *instance = nullptr;
    auto it = m_loadedEventDesc.find(eventPath);
    ERRCHECK(it->second->createInstance(&instance));

    FMOD_3D_ATTRIBUTES attr;
    attr.position = {position.x, position.y, 0.0f};
    attr.velocity = {velocity.x, velocity.y, 0.0f};
    attr.forward = {0.0f, 0.0f, 1.0f};
    attr.up = {0.0f, 1.0f, 0.0f};
    ERRCHECK(instance->set3DAttributes(&attr));

    ERRCHECK(instance->start());
    ERRCHECK(instance->release());
}

void AudioManager::update() {
    ERRCHECK(m_system->update());
}

void AudioManager::changeGlobalParameter(const std::string &name, float value) {
    ERRCHECK(m_system->setParameterByName(name.c_str(), value));
}
