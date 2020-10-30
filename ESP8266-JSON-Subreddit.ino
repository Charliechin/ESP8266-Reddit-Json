

/*******************************************************************
    A sample project for making a HTTP/HTTPS GET request on an ESP8266

    It will connect to the given request and print the body to
    serial monitor

    Parts:
    D1 Mini ESP8266 * - http://s.click.aliexpress.com/e/uzFUnIe

 *  * = Affilate

    If you find what I do usefuland would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/


    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/

// ----------------------------
// Standard Libraries
// ----------------------------

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//------- Replace the following! ------
char ssid[] = "TALKTALK9707C8"; // your network SSID (name)
char password[] = "MKNJPQPK";   // your network key
String static n = "";
// For Non-HTTPS requests
// WiFiClient client;

// For HTTPS requests
WiFiClientSecure client;

// Just the base of the URL you want to connect to
#define TEST_HOST "www.reddit.com"

// OPTIONAL - The finferprint of the site you want to connect to.
#define TEST_HOST_FINGERPRINT "DB E9 D5 FE EB EF 68 34 55 FD 62 BA C9 BB 04 D4 E3 22 18 81"

void setup()
{

  Serial.begin(115200);

  // Connect to the WiFI
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFii connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  //--------

  // If you don't need to check the fingerprint
  client.setInsecure();

  // If you want to check the fingerprint
  //  client.setFingerprint(TEST_HOST_FINGERPRINT);
}

void addHeaders()
{
  client.println(F(" HTTP/1.1"));
  //Headers
  client.print(F("Host: "));
  client.println(TEST_HOST);
  client.print(F("User-Agent: "));
  client.println("ESP826/0.1");
  client.println(F("Cache-Control: no-cache"));
}
void handleResponse()
{
  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    return;
  }
}
String makeHTTPRequest(String next)
 // Returns a String next with the value of the next page of the current post
{
  String path = "/r/askOuija/top.json?limit=1&after=";
  String url = path + next;
  url.replace("\"", ""); // gets rid of escaped quotes in the text ('\"')
  // Opening connection to server (Use 80 as port if HTTP)
  if (!client.connect(TEST_HOST, 443))
  {
    Serial.println(F("Connection failed"));
    return "Connection Failed";
  }
  // give the esp a breather
//  Serial.print("entering makeHTTPRequest, next is: ");
//  Serial.println(next);

//  Serial.print("url: ");
//  Serial.println(url);
  

  yield();
  // Send HTTP request
  client.print(F("GET "));
  // This is the second half of a request (everything that comes after the base URL)
  client.print(url);
  addHeaders();
  handleResponse();



  String quote;

  while (client.connected())
  {
    if (client.available())
    {
      quote = client.readString();
      break;
    }
  }
  client.stop();
  writeTitle(quote);
  writeAnswer(quote);

  getNext(quote);


}

void loop()
{
  int i = 0;
  String current = "";
  String next = "";
  
  while (i < 3) {
    makeHTTPRequest(n);


    i++;
    delay(8000);
  }
 }




  String writeTitle(String quote) {
    int ouija_title_start = quote.indexOf("\"title\"");
    int ouija_title_end = quote.indexOf(", \"", ouija_title_start + 1); // we start the search from the position where "title" is
    String title = quote.substring(ouija_title_start + 9, ouija_title_end);
    // Sanitize the string
    title.replace("\\\"", "'"); // gets rid of escaped quotes in the text ('\"')
    Serial.println(title);
    Serial.println(" ");
    return title;
  }

  String writeAnswer(String quote) {
    int ouija_flair_start = quote.indexOf("\"link_flair_text\"");
    int ouija_flair_end = quote.indexOf(", \"", ouija_flair_start + 1); // we start the search from the position where "title" is
    String answer = quote.substring(ouija_flair_start + 19, ouija_flair_end);
    // Sanitize the string
    answer.replace("\\\"", "'"); // gets rid of escaped quotes in the text ('\"')
    Serial.println(answer);
    Serial.println("===========================");    
    return answer;
  }

  String getNext(String quote) {
    
    int next_start = quote.indexOf("\"after\"");
    int next_end = quote.indexOf(", \"", next_start);
    String nextOuija = quote.substring(next_start + 9, next_end);
    nextOuija.replace("\\\"", "'"); // gets rid of escaped quotes in the text ('\"')
//    Serial.print("about to exit the function, next: ");
//    Serial.println(nextOuija);
    
    n = nextOuija;
    return nextOuija;
  }
