#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>

using namespace websockets;

const char* ssid = "mynetwork"; // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "mynetworkpassword"; // The password of the Wi-Fi network
const int maxClients = 4; // Accept up to 4 connections
int clientsConnected = 0;

WebsocketsClient clients[maxClients];
WebsocketsServer server;

void setup()
{
  Serial.begin(115200);

  WiFi.hostname("ESP-websockets");
  WiFi.begin(ssid, password); // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  server.listen(81);
}

// prepare data to be sent to a client
String prepareData()
{
  String dataToReturn = "time: " + String(millis()) + "\r\n";
  return dataToReturn;
}

// this method goes through every client and polls for new messages 
void pollAllClients()
{
  for (int i = 0; i < clientsConnected; i++)
  {
    clients[i].poll();
  }
}

// the client that sent the message gets the data in response
void onMessage(WebsocketsClient &client, WebsocketsMessage message)
{
  int repeatCount = message.data().toInt();
  Serial.print("repeat: ");
  Serial.print(repeatCount);
  Serial.println(", Sending Data.");
  
  for (int i = 0; i <= repeatCount; i++) {
    client.send( prepareData() );
  }
}

void loop()
{
  if (server.available())
  {
    // if there is a client that wants to connect
    if (server.poll() && clientsConnected < maxClients)
    {
      // accept the connection and register callback
      Serial.print("Accepting a new client, number of client: ");
      Serial.println(clientsConnected + 1);

      // accept client and register onMessage event
      WebsocketsClient client = server.accept();
      client.onMessage(onMessage);

      // store it for later use
      clients[clientsConnected] = client;
      clientsConnected++;
    }

    // check for updates in all clients
    pollAllClients();

    delay(10);
  }
}
