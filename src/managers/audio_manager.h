#pragma once

#include <Arduino.h>

#include <painlessMesh.h>
#include <HTTPClient.h>
#include <SD.h>
#include <AudioOutputI2S.h>
#include <AudioGeneratorWAV.h>
#include <AudioFileSOurceSPIFFS.h>

#include "config_manager.h"

class AudioManager
{
private:
    static AudioFileSourceSPIFFS *in;
    static AudioGeneratorWAV *wav;
    static AudioOutputI2S *out;

    static void audioUpdate();
    static Task taskAudioUpdate;
public:
    static void initialize(Scheduler *scheduler);
    static void playAudio(String fileName);
    static bool isAudioPlaying();
};