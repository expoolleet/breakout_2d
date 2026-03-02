#include "audio_manager.hpp"

#include <fmod_errors.h>

#include <fmod_studio.hpp>
#include <string>

#include "custom_attributes.hpp"
#include "logging.hpp"
#include "path_manager.hpp"

#define ERRCHECK(result)                                                      \
    if (result != FMOD_OK) {                                                  \
        std::cerr << "FMOD Error: " << FMOD_ErrorString(result) << std::endl; \
    }

void AudioManager::_saveEvent(const std::string &eventPath) {
    if (m_loadedEventDesc.contains(eventPath)) return;
    FMOD::Studio::EventDescription *eventDesc = nullptr;
    ERRCHECK(m_system->getEvent(eventPath.c_str(), &eventDesc));
    m_loadedEventDesc[eventPath] = eventDesc;
}

AudioManager::~AudioManager() {
    ERRCHECK(m_system->release());
}

AudioManager &AudioManager::Get() {
    NO_DESTROY_ATTR static AudioManager manager;
    return manager;
}

void AudioManager::init() {
    ERRCHECK(FMOD::Studio::System::create(&m_system));
    ERRCHECK(m_system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));

    FMOD_3D_ATTRIBUTES listenerAttr;
    listenerAttr.position = {0.0f, 0.0f, 0.0f};
    listenerAttr.velocity = {0.0f, 0.0f, 0.0f};
    listenerAttr.forward = {0.0f, 0.0f, 1.0f};
    listenerAttr.up = {0.0f, 1.0f, 0.0f};
    ERRCHECK(m_system->setListenerAttributes(0, &listenerAttr));
}

void AudioManager::loadBank(const std::string &name) {
    FMOD::Studio::Bank *mainBank = nullptr;
    FMOD::Studio::Bank *stringsBank = nullptr;
    std::string bankPath = PathManager::getResourcePath("fmod/" + name);
    ERRCHECK(m_system->loadBankFile((bankPath + "/" + name + ".bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &mainBank));
    ERRCHECK(m_system->loadBankFile((bankPath + "/" + name + ".strings.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank));
}

FMOD_RESULT myCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE *instance, void *parameters) {
    if (type == FMOD_STUDIO_EVENT_CALLBACK_TIMELINE_MARKER) {
        auto *props = (FMOD_STUDIO_TIMELINE_MARKER_PROPERTIES *)parameters;
        std::cout << props->name << std::endl;
        std::cout << props->position << std::endl;
    }
    return FMOD_OK;
}

void AudioManager::playOnce(const std::string &eventPath) {
    _saveEvent(eventPath);
    FMOD::Studio::EventInstance *instance = nullptr;
    ERRCHECK(m_loadedEventDesc[eventPath]->createInstance(&instance));
    ERRCHECK(instance->start());
    ERRCHECK(instance->release());
}

void AudioManager::playOnce(const std::string &eventPath, glm::vec2 position, glm::vec2 velocity) {
    _saveEvent(eventPath);
    FMOD::Studio::EventInstance *instance = nullptr;
    ERRCHECK(m_loadedEventDesc[eventPath]->createInstance(&instance));

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
