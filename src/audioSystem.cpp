
#include <cassert>

#include "FMOD/fmod.h"

#include "audioSystem.h"
#include "arena.h"
#include "common.h"

AudioSystem* gAudioSystem;
const int NOT_FOUND = -1;

static const SoundDataEntry allSoundData[] = {
  { SFX_ID::FALLBACK, "assets/audio/sfx/fallback.wav" },
  { SFX_ID::JUMP, "assets/audio/sfx/fallback.wav" },
};

int GetAvailableChannelIndex(AudioSystem* audioSystem){
  for (int i = 0; i < AudioSystem::CHANNEL_COUNT; i++) {
    FMOD_CHANNEL* channel = audioSystem->channels[i];
    if(channel == nullptr){ // never used before
      return i;
    }
    FMOD_BOOL isPlaying = false;
    FMOD_Channel_IsPlaying(channel, &isPlaying);
    if(isPlaying == false){
      return i;
    }
  }
  return NOT_FOUND;
}

void PlaySFX(SFX_ID id, float volume) {
  
  assert(id != SFX_ID::COUNT);
  
  FMOD_SOUND* sfx = gAudioSystem->soundsEffects[(int)id];
  if (sfx == nullptr) {
    assert(id != SFX_ID::FALLBACK);
    PlaySFX(SFX_ID::FALLBACK, volume);
  }
  
  int channelIndex = GetAvailableChannelIndex(gAudioSystem);
  if (channelIndex == NOT_FOUND) {
    return;
  }
  
  FMOD_CHANNEL** channelSlot = &gAudioSystem->channels[channelIndex];
  FMOD_System_PlaySound(gAudioSystem->soundSystem, sfx, nullptr, false, channelSlot);
  FMOD_Channel_SetVolume(*channelSlot, volume);
}

void InitializeAudioSystem(AudioSystem* audioSystem, Memory::Arena* arenaMain) {
  assert(!audioSystem->initialized);
  
  size_t memorySize = AUDIO_MEMORY_ALLOWANCE;
  audioSystem->fmodMemory = Memory::CreateSubArena(arenaMain, memorySize);
  FMOD_RESULT memoryInitOk = FMOD_Memory_Initialize(audioSystem->fmodMemory, memorySize, nullptr, nullptr, nullptr, FMOD_MEMORY_ALL);
  assert(memoryInitOk == FMOD_OK);
  
  FMOD_RESULT systemCreationOk = FMOD_System_Create(&audioSystem->soundSystem, FMOD_VERSION);
  assert(systemCreationOk == FMOD_OK);
  
  FMOD_RESULT system_init_ok = FMOD_System_Init(audioSystem->soundSystem, AudioSystem::CHANNEL_COUNT, FMOD_INIT_NORMAL, nullptr);
  assert(system_init_ok == FMOD_OK);
  
  gAudioSystem = audioSystem;
  audioSystem->initialized = true;
}
void UpdateAudio(AudioSystem* audioSystem) {
  if (gAudioSystem == nullptr || gAudioSystem != audioSystem) {
    gAudioSystem = audioSystem;
  }
  
  assert(audioSystem->initialized);
  FMOD_System_Update(audioSystem->soundSystem);
}

namespace AssetManagement {
  void LoadAllSFX(AudioSystem* audioSystem) {
    for (const SoundDataEntry& soundData : allSoundData) {
      FMOD_RESULT soundCreatedOk = FMOD_System_CreateSound(audioSystem->soundSystem, soundData.path, FMOD_DEFAULT, nullptr,
        &audioSystem->soundsEffects[(int)soundData.id]);
      assert(soundCreatedOk == FMOD_OK);
    }
  }
}