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
- [ ] Updated State Manager
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

## Architectural Core:

### State-Based Task Scheduling

* Blocks of code - managers - can register their batches of tasks with the state manager, describing what states their task(s) are functional / should be functional in.
* The state manager enables/disables the appropriate tasks each time the state is changed
* This means that we can isolate functionality by assigning tasks to the states of the system.

#### To Implement:
* Adjust the state manager class to track tasks based upon a map of states to lists of tasks. Enable / disable tasks if they are present in the new list of tasks when the state changes.
* Create a simple way to register tasks and their functional states

<READY> {TaskReadyModuleUpdate}
<PAIRING> {TaskParingModuleUpdate}

## Hardware Managers:
All managers have two parts:
* Initialization / Setup
* Updates

Managers can register interrupts / tasks as well, during the initialization/setup phase, but they all must have these (at least)
### State Manager
### Display Manager (@Controller)
### Laser Manager (@Controller)
### Lighting Manager (@Target)
### Audio Manager
### Network Manager
### Input Manager

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
* 
@Target

## Data Structures:

### State of the Game
* Current Score
* Current Game mode
* Current Ammo Count
* Current Timer

### Game Configuration
* How many rounds
* What type of rounds
* What game is being played

### Target/Controller Information
@Target:
* Knows controller ID
@Controller:
* Needs to know each target, target ID
* Keep the list of targets in order

