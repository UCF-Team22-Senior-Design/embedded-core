- [Development Plans](#development-plans)
- [Priority / TODO](#priority--todo)
- [Architectural Core:](#architectural-core)
  - [State-Based Task Scheduling](#state-based-task-scheduling)
    - [To Implement:](#to-implement)
  - [Hardware Managers:](#hardware-managers)
    - [State Manager](#state-manager)
    - [Display Manager (@Controller)](#display-manager-controller)
    - [Laser Manager (@Controller)](#laser-manager-controller)
      - [Initialization](#initialization)
      - [Update](#update)
      - [Methods](#methods)
    - [Lighting Manager (@Target)](#lighting-manager-target)
      - [Initialization](#initialization-1)
      - [Update](#update-1)
      - [Methods](#methods-1)
    - [Audio Manager](#audio-manager)
      - [Initialization](#initialization-2)
      - [Update](#update-2)
      - [Methods](#methods-2)
    - [Network Manager](#network-manager)
      - [Initialization](#initialization-3)
      - [Update](#update-3)
      - [Methods](#methods-3)
    - [Input Manager](#input-manager)
      - [Initialization](#initialization-4)
      - [Update](#update-4)
      - [Methods](#methods-4)
  - [Functional Modules:](#functional-modules)
    - [Pairing Module](#pairing-module)
    - [Ready Module](#ready-module)
    - [Gameplay Module](#gameplay-module)
  - [Data Structures:](#data-structures)
    - [Volatile Game Information](#volatile-game-information)
    - [Game Configuration](#game-configuration)
    - [Target/Controller Information](#targetcontroller-information)

# Development Plans

Vocabulary Definition
| Word | Description |
| -: | :- |
| Manager | A class that controls a piece of low-level functionality, say, drawing to the display, doesn't vary from state to state |
| Module | A class that controls or defines system behavior depending on state - the ready module, pairing module, etc. are examples of this | 
| Controller | The device that serves as the primary controller for the system - both from the user experience and also in software. Physically, a laser gun |
| Target | Devices that serve as targets for the laser system. Less complicated in software, these devices act mostly by the commands of the controller's network packets |

**Notes on documentation:**

Things preceeded with an "@{Device}" are specific to each device. This is most frequently found in the descriptions for modules, who have different functionality depending on where they're used. Some managers have this tag, too, where it describes what device these managers are present on.

# Priority / TODO
- [x] Updated State Manager
- [ ] Prototype the Hardware Managers
  - [ ] Input Manager
  - [ ] Network Manager
  - [ ] Laser Manager
  - [ ] Audio Manager
  - [ ] Display Manager
  - [ ] Lighting Manager
- [ ] Module Development
  - [ ] Ready Module
  - [ ] Pairing Module
  - [ ] Play Module
  - [ ] Results Module

# Architectural Core:

## State-Based Task Scheduling

* Blocks of code - managers - can register their batches of tasks with the state manager, describing what states their task(s) are functional / should be functional in.
* The state manager enables/disables the appropriate tasks each time the state is changed
* This means that we can isolate functionality by assigning tasks to the states of the system.

### To Implement:
* Adjust the state manager class to track tasks based upon a map of states to lists of tasks. Enable / disable tasks if they are present in the new set of tasks when the state changes.
* Create a simple way to register tasks and their functional states

## Hardware Managers:
All managers have two parts:
* Initialization / Setup
* Updates
  * Update functions are actually TaskScheduler tasks, registered during the
    initialization phase. They are not called by the main function of the program.

Managers can register interrupts / tasks as well, during the initialization/setup 
phase, but they all should have these (at least)
### State Manager
### Display Manager (@Controller)
### Laser Manager (@Controller)
A simple hadware manager. This is our only "output" output, so I felt it was better
to put this in its own manager rather than sticking it into the input manager.
#### Initialization
* Register laser's pins as output pins
* Set the laser's state to off.
* Register a callaback for the input manager, listening to the trigger.
#### Update
* Nothing. It's a laser.
#### Methods
* None. Laser state is dependent on the trigger input only, and shouldn't be
  controlled by anything else.

### Lighting Manager (@Target)
This manager drives the WS2182 LED display on the target's face. It has some
internal lighting programs (just functions, really) that can be called to signal
certain patterns. The state of the lights can be queried, if you're into that
sort of thing.
#### Initialization
* Initialize the WS2182 light driver
* Begin displaying some basic lighting pattern
#### Update
* Update lighting pattern/program (based upon time)
#### Methods
* `void playPattern(PatternEnum pattern)`
* `void setLightColor(Color color)`
* `void setPlaybackScale(double timescale)`
* `void disableLights()`
* `PatternEnum getCurrentPattern()`

### Audio Manager
This manager handles the playback of audio data for the connected speakers on
both the target and controller. As all audio files are hard-coded on the device,
it's just a matter of triggering which one at the right time.
#### Initialization
* Create the audio source from the library
* Prepare lists of audio that can be called
* Load into memory critical / common audio files.
#### Update
* Update the audio buffer (if required)
#### Methods
* `void playAudio(SoundEnum sound)`
* `void stopPlayback()`
* `bool isAudioPlaying()`

### Network Manager
This manager handles sending and receiving network packets - a task (mostly) done
for us already by the PainlessMesh library, but it allows for callbacks to be
registered to it for certain packet messages.
#### Initialization
* Ready-up the painless mesh
* Register internal callbacks
#### Update
* No functionality
#### Methods
* `void sendPacket(String message, String recipient (optional, default=broadcast))`
* `void registerCallback(NetworkMessageCallback callback, String tagFilter (optional))`
* `void deregisterCallback(NetworkMessageCallback callback)`

### Input Manager
This manager handles the various inputs of the devices - listening for inputs, 
tracking their states, etc. Button / input states can have callbacks attached to 
them, or can be polled from the input manager itself.
#### Initialization
* Register ISRs for button/phototransistor inputs, which enable input tracking tasks
#### Update
* No functionality - all input management is done through ISRs
#### Methods
* `bool getInputState(InputEnum input)`
* `void registerCallback(InputCallback callback)`
* `void deregisterCallback(InputCallback callback)`

## Functional Modules:
### Pairing Module
@Controller
* Listens for trigger events, broadcasts network packet
* Listens for a pairing response from targets
* Stores target information into the appropriate data structure
* Asks display manager to update with menu changes
* Tracks button presses for menu things
* Broadcasts network message when pairing is finished
@Target
* Listens for hit events
* Messages controller with pairing response when hit
* Stores controller information into the appropriate data structure
* Listens for ending of pairing message
* Asks lighting manager to display different colors depending on pairing state

### Ready Module
@Controller
* Listens for button inputs (left/right/trigger)
* Asks display manager to update with menu changes
* Stores game/system configuration information into the appropriate data structure
@Target
* Listens for pairing message to send into pairing state
* Listens for game start message to send into game state
* Asks lighting manager to display idle light patterns

### Gameplay Module
@Controller
* Follows the rules of the game configuration data object
* Performs game tasks on fixed update interval (timers, spawning targets, 
  disabling targets, etc)
* Listens for target hit packets, tracks score changes in current game state
* Plays game audio on trigger click, alongside any game rules.
* When the game is over, emits game end packet to network and shifts into "results" mode.
@Target
* Listens for commands from the controller w.r.t enabling/disabling/lighting/audio
* Emits "hit" network packets when laser is detected.
* Listens for game end packet, where it sets the system into the "ready" mode.

## Data Structures:

### Volatile Game Information
* Current Score
* Current Game mode
* Current Ammo Count
* Current Timer

### Game Configuration
* How many rounds
* What type of rounds
* What game is being played
* Various game mode settings

### Target/Controller Information
@Target:
* Knows controller ID
@Controller:
* Needs to know each target, target ID
* Keep the list of targets in order

