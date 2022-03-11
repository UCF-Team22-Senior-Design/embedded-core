#include "audio_manager.h"

// Initialize our static members
Task AudioManager::taskAudioUpdate(TASK_MILLISECOND * 5, TASK_FOREVER, &audioUpdate);
AudioFileSourceSPIFFS *AudioManager::in;
AudioGeneratorWAV *AudioManager::wav;
AudioOutputI2S *AudioManager::out;

/**
 * @brief Initialize our audio sources and outputs
 * 
 * @param scheduler The user scheduler
 */
void AudioManager::initialize(Scheduler *scheduler)
{
    // Prevent reduplicated initialization
    static bool hasBeenInitialized = false;
    if(hasBeenInitialized) return;
    hasBeenInitialized = true;

    

    // Create our audio sources & outputs
    in  = new AudioFileSourceSPIFFS("/audio/owin31.wav");
    wav = new AudioGeneratorWAV();
    out = new AudioOutputI2S(0, 0);
    out->SetPinout(26, 25, 19);

    // Load volume settings from the configuration manager
    out->SetGain(ConfigManager::configData.volume);
    wav->begin(in, out);

    // Set up our audio playback tasks
    (*scheduler).addTask(taskAudioUpdate);
    taskAudioUpdate.enable();

    Serial.println("<AudioManager> Initialization Complete");
}

void AudioManager::audioUpdate()
{
    // If our audio is currently running...
    if(wav->isRunning())
    {
        // Fill the buffer
        wav->loop();
    }
    else
    {
        // Disable our audio output when audio is done
        //in->close();
        //out->flush();
        wav->stop();
        // Disable this task
        taskAudioUpdate.disable();
    }
}

/**
 * @brief Return the status of the audio playback
 *
 */
bool AudioManager::isAudioPlaying()
{
    return wav->isRunning();
}

/**
 * @brief Begins the playback of a given audio file, pre-empting any currently playing audio
 * 
 * @param fileName 
 */
void AudioManager::playAudio(String fileName)
{
    // If there is currently audio playing, stop it.
    if(isAudioPlaying())
    {
        wav->stop();
        
        //in->close();
    }

    
    taskAudioUpdate.disable();

    // Load the thing as a source
    //out->flush();
    in = new AudioFileSourceSPIFFS(fileName.c_str());
    wav->begin(in, out);

    // Restart our audio task
    taskAudioUpdate.restart();

    Serial.printf("<AudioManager> Beginning playback of %s\n", fileName.c_str());
}

void AudioManager::setVolume(float volume)
{
    out->SetGain(volume);
    ConfigManager::configData.volume = volume;
    ConfigManager::saveData();

    Serial.printf("<AudioManager> Audio has been changed to %f\n", volume);
}