#include "network_manager.h"

painlessMesh NetworkManager::mesh;
std::unordered_map<std::string, std::vector<NetworkMessageCallback>> NetworkManager::callbacks;

NetworkManager::NetworkManager()
{
    // We have nothing to do on the initial instantiation of this class - most
    // things that this class does is entirely static.
}

/**
 * @brief Initializes the network, including enabling the mesh network and
 *        connecting its tasks to the main scheduler. This should only run once,
 *        right at the start of the system execution.
 * 
 * @param scheduler The main scheduler, to be controlled by the mesh network.
 */
void NetworkManager::initialize(Scheduler* scheduler)
{
    // Ensure this only runs once.
    static bool hasBeenInitialized = false;
    if(hasBeenInitialized) return;
    hasBeenInitialized = true;

    // Configure our wireless mesh
    mesh.init(DEFAULT_MESH_PREFIX, DEFAULT_MESH_PASSWORD, scheduler, MESH_PORT);
    mesh.onReceive(&NetworkManager::receivedMessage);

    Serial.printf("<NetworkManager> Network initialized with prefix %s, password %s, and port %d\n", DEFAULT_MESH_PREFIX, DEFAULT_MESH_PASSWORD, MESH_PORT);
}

/**
 * @brief This updates our main scheduler through the painlessMesh object. This
 *        MUST be called in the system's `loop` function, and shouldn't be
 *        delayed.
 */
void NetworkManager::update()
{
    // Let the mesh update, while also updating the scheduler.
    mesh.update();
}

/**
 * @brief An internal callback used for handling incoming messages from the
 *        network. Takes the message, parses it as expected, and then
 *        distributes it as requested.
 * 
 * @param from 
 * @param message 
 */
void NetworkManager::receivedMessage(uint32_t from, String &message)
{
    // Parse the message into a NetworkMessage object. If we fail, print it
    // to serial and skip it.
    try
    {
        NetworkMessage parsedMessage = NetworkMessage(from, message);

        // Do some logic if it's something that just we care about

        // Distribute it to the various callbacks that are registered to that tag.
        // Step 1: Call callbacks under the "none" filter
        if(callbacks.count("NONE") > 0)
        {
            for(NetworkMessageCallback callback : callbacks["NONE"])
            {
                callback(parsedMessage);
            }
        }

        if(callbacks.count(parsedMessage.getTag().c_str()) > 0)
        {
            for(NetworkMessageCallback callback : callbacks[parsedMessage.getTag().c_str()])
            {
                callback(parsedMessage);
            }
        }
    }
    catch(const std::exception& e)
    {
        Serial.printf("Unknown Network Message Format: %s\n", message.c_str());
        return;
    }
}

void NetworkManager::sendMessage(NetworkMessage message)
{
    Serial.printf("[NetworkManager] broadcast message %s\n", message.toString().c_str());
    // Broadcast a message across the network
    mesh.sendBroadcast(message.toString());
}

void NetworkManager::sendMessage(NetworkMessage message, uint32_t destination)
{
    Serial.printf("[NetworkManager] broadcast message %s to %lu\n", message.toString().c_str(), destination);
    // Send a message to a specific target in the network
    if(mesh.sendSingle(destination, message.toString()))
    {
        Serial.println("Worked");
    }
    else
    {
        Serial.println("Did not");
    }
}

void NetworkManager::registerCallback(NetworkMessageCallback callback, String tagFilter)
{
    // Register a callback for a specific tag filter
    callbacks[tagFilter.c_str()].push_back(callback);
}

/**
 * @brief This function removes callback registration from the network manager - please specify the
 *        original tag filter when you do.
 * 
 * @param callback The callback function to be removed.
 * @param tagFilter The tag associated with this.
 */
void NetworkManager::deregisterCallback(NetworkMessageCallback callback, String tagFilter)
{
    // De-register a callback for a specific tag filter
    // First - find the callback in the provided filter
    std::vector<NetworkMessageCallback> callbackList = callbacks[tagFilter.c_str()];

    // Find the position of the callback in the list
    std::vector<NetworkMessageCallback>::iterator it;
    for(it = callbackList.begin(); it != callbackList.end(); it++)
    {
        if((*it) == callback)
            break;
    }

    // If we have found it, rease it. Otherwise, it didn't exist
    if(it != callbackList.end())
    {
        callbackList.erase(it);
        callbacks[tagFilter.c_str()] = callbackList;
    }
}

/**
 * @brief This returns the current time of the network
 * 
 * @return uint32_t 
 */
uint32_t NetworkManager::getNodeTime()
{
    return mesh.getNodeTime();
}

uint32_t NetworkManager::getNodeID()
{
    return mesh.getNodeId();
}