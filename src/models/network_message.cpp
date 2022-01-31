#include "network_message.h"

/**
 * @brief Construct a new Network Message object from a mesh ID and a payload
 * 
 * @param from The mesh ID of the node this message came from. Optional for
 *             outgoing messages, as this information is not sent as part of the
 *             payload.
 * @param payload The incoming string from the network message. Should conform
 *                to our networking format.
 */
NetworkMessage::NetworkMessage(uint32_t from, String payload)
{
    // Store the sender's information
    sender = from;

    // Parse the payload

    // 1298518932412;pairing_tag|some data
    
    // Payloads are composed of the timestamp, a semicolon, a tag, 
    // then a vertical bar character "|", then the data associated with that payload.
    int semicolonLocation = payload.indexOf(';');
    int barLocation = payload.indexOf('|');

    if(barLocation < 0 || semicolonLocation < 0 || semicolonLocation >= barLocation)
        throw std::runtime_error("Malformed Payload Recieved");

    // Separate the payload into its parts - the timestamp, tag, and data.
    timestamp = static_cast<uint32_t>(strtoul(
        payload.substring(0, semicolonLocation).c_str(), NULL, 0));
    tag  = String(payload.substring(semicolonLocation + 1, barLocation));
    data = String(payload.substring(barLocation + 1));
}

/**
 * @brief Construct an outgoing Network Message object. To be used when preparing
 *        a message to be sent externally.
 * 
 * @param outTag The "Tag" of the message
 * @param outData The actual data associated with the message.
 */
NetworkMessage::NetworkMessage(String outTag, String outData, uint32_t currentTime)
{
    // Define a dummy sender, as it is not used
    sender = 0;

    // Assign our typical things
    tag = outTag;
    data = outData;
    timestamp = currentTime;
}

/**
 * @brief Transform the networkMessage into a payload to be sent
 * 
 * @return The payload to be sent
 */
String NetworkMessage::toString()
{
    // Take our tag/data information, and merge it into one string to be
    // sent as a payload
    return String(String(timestamp) + ';' + tag + '|' + data);
}

/**
 * @brief Get the ID of the node that sent this message
 * 
 * @return uint32_t 
 */
uint32_t NetworkMessage::getSender()
{
    return sender;
}

/**
 * @brief Get the timestamp of when this message was sent
 * 
 * @return uint32_t 
 */
uint32_t NetworkMessage::getTimestamp()
{
    return timestamp;
}

/**
 * @brief Gets the "tag" information from this message
 * 
 * @return String 
 */
String NetworkMessage::getTag()
{
    return tag;
}

/**
 * @brief Gets the data information from this message
 * 
 * @return String 
 */
String NetworkMessage::getData()
{
    return data;
}