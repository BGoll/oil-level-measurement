#include <ArduinoJson.h>

#if SIMULATED_DATA

float readDistance()
{
    return random(3, 100);
}

#else

int readDistance()
{
    long distance = 0;
    long travelTime = 0;

    Serial.print("TRIGGER_PIN: ");
    Serial.println(TRIGGER_PIN);
    Serial.print("ECHO_PIN: ");
    Serial.println(ECHO_PIN);

    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(3);
    noInterrupts();
    digitalWrite(TRIGGER_PIN, HIGH); // trigger impuls 10 us
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    travelTime = pulseIn(ECHO_PIN, HIGH); // measure echo time
    Serial.print("travelTime: ");
    Serial.println(travelTime);
    interrupts();
    travelTime = (travelTime / 2); // dividing the travelTime by two, since it contains the time to the object and back again
    distance = travelTime / 29.1; // convert time to centimeters
    Serial.print("Distance: ");
    Serial.println(distance);
    return(distance);
}

#endif

void readMessage(int messageId, char *payload)
{
    int distance = readDistance();
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["deviceId"] = DEVICE_ID;
    //root["messageId"] = messageId;

    // NAN is not the valid json, change it to NULL
    if(std::isnan(distance))
    {
        root["distance"] = NULL;
    }
    else
    {
        root["distance"] = distance;
    }

    root.printTo(payload, MESSAGE_MAX_LEN);
}

void parseTwinMessage(char *message)
{
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(message);
    if (!root.success())
    {
        Serial.printf("Parse %s failed.\r\n", message);
        return;
    }

    if (root["desired"]["interval"].success())
    {
        interval = root["desired"]["interval"];
    }
    else if (root.containsKey("interval"))
    {
        interval = root["interval"];
    }
}
