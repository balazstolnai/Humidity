#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>


class Ntpnet
{
    public: 
    unsigned long lastNTPResponse;

    private:
    ESP8266WiFiMulti wifiMulti;
    WiFiUDP UDP;
    IPAddress timeServerIP;
    const char* NTPServerName;
    const int NTP_PACKET_SIZE;
    byte NTPBuffer[48];
    unsigned long intervalNTP;
    unsigned long prevNTP;
    uint32_t timeUNIX;
    unsigned long currentMillis;

    public:

    Ntpnet () :NTPServerName ("pool.ntp.org"),  NTP_PACKET_SIZE(48), intervalNTP(600000), prevNTP(0), timeUNIX (0){}

    void SendPacket()
    {
        memset(NTPBuffer, 0, NTP_PACKET_SIZE);
        NTPBuffer[0] = 0b11100011;
        UDP.beginPacket(timeServerIP, 123);
        UDP.write(NTPBuffer, NTP_PACKET_SIZE);
        UDP.endPacket();
    }

    uint32_t getTime()
    {
        if (UDP.parsePacket() == 0)
        {
        return 0;
        }

        UDP.read(NTPBuffer, NTP_PACKET_SIZE);
        
        uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
        
        const uint32_t seventyYears = 2208988800UL;

        uint32_t UNIXTime = NTPTime - seventyYears;
        return UNIXTime;
    }

    void udpStart()
    {
        Serial.println("Starting UDP");
        UDP.begin(123);
        Serial.print("Local port:\t");
        Serial.println(UDP.localPort());
        Serial.println();
        
        if(!WiFi.hostByName(NTPServerName, timeServerIP))
        { 
            Serial.println("DNS lookup failed. Rebooting.");
            Serial.flush();
            ESP.reset();
        }
        Serial.print("Time server IP:\t");
        Serial.println(timeServerIP);
        
        Serial.println("\r\nSending NTP request ...");
        
        SendPacket();
        uint32_t time;
        do
        {
            time = getTime();                   
            if (time)                                   
            {                                  
                timeUNIX = time;
                Serial.print("NTP response:\t");
                Serial.println(timeUNIX);
                lastNTPResponse = currentMillis;
            } 
            else{delay(5);}
        }while(!time);

    }

    void ntpInLoop()
    {
        currentMillis = millis();

        if (currentMillis - prevNTP > intervalNTP)
        {
            prevNTP = currentMillis;
            Serial.println("\r\nSending NTP request ...");
            SendPacket();
        }

        uint32_t time = getTime();
        if (time)
        {                                  
            timeUNIX = time;
            Serial.print("NTP response:\t");
            Serial.println(timeUNIX);
            lastNTPResponse = currentMillis;
        } 
 
    }

    int* giveTime()
    {
        uint32_t actualTime = timeUNIX + (currentMillis - lastNTPResponse)/1000;
        static int a[3];
        a[0] = getHours(actualTime);
        a[1] = getMinutes(actualTime);
        a[2] = getSeconds(actualTime);
        
        return a;
        
    }

    inline int getSeconds(uint32_t UNIXTime) 
    {
    return UNIXTime % 60;
    }

    inline int getMinutes(uint32_t UNIXTime) 
    {
    return UNIXTime / 60 % 60;
    }

    inline int getHours(uint32_t UNIXTime)
    {
    return (UNIXTime / 3600 % 24);
    }

    inline int getRawHours(uint32_t UNIXTime)
    {
    return (UNIXTime / 3600);
    }
};  
