#pragma once

#include "FMOD/fmod_common.h"

namespace Memory {
  struct Arena;
}

enum class SONG_ID {
  NONE,
  THEME
};

enum class SFX_ID {
  FALLBACK,
  JUMP,
  
  COUNT
};

struct SoundDataEntry {
  SFX_ID id;
  const char* path;
};

struct AudioSystem {
  bool initialized;
  void* fmodMemory;
  FMOD_SYSTEM* soundSystem;
  static const int CHANNEL_COUNT = 32;
  FMOD_CHANNEL* channels[CHANNEL_COUNT];
  FMOD_SOUND* soundsEffects[(int)SFX_ID::COUNT];
  
  SONG_ID songId;
  FMOD_SOUND* song;
  FMOD_CHANNEL* songChannel;
};

extern AudioSystem* gAudioSystem;

void PlaySong(SONG_ID id);
void PlaySFX(SFX_ID id, float volume = 1);
void InitializeAudioSystem(AudioSystem* audioSystem, Memory::Arena* arenaMain);
void UpdateAudio(AudioSystem* audioSystem);

namespace AssetManagement {
  void LoadAllSFX(AudioSystem* audioSystem);
}