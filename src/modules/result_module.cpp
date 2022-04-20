#include "results_module.h"

Task ResultsModule::moduleTask;

String ResultsModule::gamemode = "";
String ResultsModule::scoreLabel = "";
int ResultsModule::score = 0;

void ResultsModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_SECOND, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

bool ResultsModule::onWake()
{
    Serial.println("[ResultsModule] I've been woken from sleep");

    // Register our callbacks
    InputManager::registerInputCallback(&handleLeftMenuInput, InputSource::BUTTON_LEFT);

    String messageString = "You played " + gamemode + "\n"
                            "with a final score of " + String(score) + " " +
                            scoreLabel;
    DisplayManager::drawBasicScreen("EXIT", "", "Congratulations!", messageString.c_str());

    // Play our game-over audio
    AudioManager::playAudio("/audio/game-over.wav");

    return true;
}

void ResultsModule::onSleep()
{
    Serial.println("[ResultsModule] I'm being sent to sleep");

    InputManager::deregisterInputCallback(&handleLeftMenuInput, InputSource::BUTTON_LEFT);
}


void ResultsModule::onUpdate()
{
    // Nothing to do
}

void ResultsModule::handleLeftMenuInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if(state) return;

    Serial.println("[ResultsModule] Left Menu Pressed!");

    StateManager::setSystemState(SystemState::Ready);
}

void ResultsModule::setResults(String newGamemodeLabel, int newScore, String newScoreLabel)
{
    gamemode = newGamemodeLabel;
    score = newScore;
    scoreLabel = newScoreLabel;
}