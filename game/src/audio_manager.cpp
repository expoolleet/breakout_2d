#include "audio_manager.hpp"

#include "custom_attributes.hpp"
#include "logging.hpp"
#include "path_manager.hpp"

#include <fmod_errors.h>
#include <fmod_studio.hpp>
#include <string>

#define ERRCHECK(result)                                                                                                                   \
    if (result != FMOD_OK) {                                                                                                               \
        std::cerr << "FMOD Error: " << FMOD_ErrorString(result) << std::endl;                                                              \
    }

AudioManager::~AudioManager() {
    ERRCHECK(m_system->release());
}

AudioManager &AudioManager::Get() {
    NO_DESTROY_ATTR static AudioManager manager;
    return manager;
}

FMOD_RESULT myCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE *instance, void *parameters) {
    if (type == FMOD_STUDIO_EVENT_CALLBACK_TIMELINE_MARKER) {
        auto *props = (FMOD_STUDIO_TIMELINE_MARKER_PROPERTIES *)parameters;
        std::cout << props->name << std::endl;
        std::cout << props->position << std::endl;
    }
    return FMOD_OK;
}

void AudioManager::playOneShot(const std::string &eventPath) {
    if (!m_loadedEventDesc.contains(eventPath)) {
        FMOD::Studio::EventDescription *eventDesc = nullptr;
        ERRCHECK(m_system->getEvent(("event:/" + eventPath).c_str(), &eventDesc));
        m_loadedEventDesc[eventPath] = eventDesc;
    }
    FMOD::Studio::EventInstance *instance = nullptr;
    ERRCHECK(m_loadedEventDesc[eventPath]->createInstance(&instance));
    ERRCHECK(instance->start());
    ERRCHECK(instance->release());
}

void AudioManager::init() {
    ERRCHECK(FMOD::Studio::System::create(&m_system));
    ERRCHECK(m_system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));
}

void AudioManager::loadBank(const std::string &name) {
    FMOD::Studio::Bank *mainBank = nullptr;
    FMOD::Studio::Bank *stringsBank = nullptr;
    std::string bankPath = PathManager::getResourcePath("fmod/" + name);
    ERRCHECK(m_system->loadBankFile((bankPath + "/" + name + ".bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &mainBank));
    ERRCHECK(m_system->loadBankFile((bankPath + "/" + name + ".strings.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank));
}

void AudioManager::update() {
    ERRCHECK(m_system->update());
}

void AudioManager::changeGlobalParameter(const std::string &name, float value) {
    ERRCHECK(m_system->setParameterByName(name.c_str(), value));
}
