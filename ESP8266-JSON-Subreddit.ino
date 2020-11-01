#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
// OLED stuff
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerifItalic9pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// OLED stuff

char ssid[] = "TALKTALK9707C8"; // your network SSID (name)
char password[] = "MKNJPQPK";   // your network key
String static n = "";
String static title = "title: Init val";
String static answer = "ans: Init val";
String static path = "/r/askOuija/top.json?limit=1&after=";
int static xPosLoading = 5;
bool has_answer = false;
// For Non-HTTPS requests
// WiFiClient client;

// For HTTPS requests
WiFiClientSecure client;

// Just the base of the URL you want to connect to
#define TEST_HOST "www.reddit.com"

// OPTIONAL - The finferprint of the site you want to connect/ to.
#define TEST_HOST_FINGERPRINT "DB E9 D5 FE EB EF 68 34 55 FD 62 BA C9 BB 04 D4 E3 22 18 81"

void setup()
{

  Serial.begin(115200);
  initScreen();
  // Connect to the WiFI
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  // display.setFont(&FreeMonoBold12pt7b);
  printText("Connecting to: ", 10, 0);
  Serial.print("Connecting Wifi: ");
  printText(ssid, 10, 30);
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int posX = 10;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    printText("...", posX, 50);
    delay(500);
    posX = posX + 3;
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  //--------

  // If you don't need to check the fingerprint
  display.clearDisplay();
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
  // String path = "/r/askOuija/new.json?limit=1&after=";
  String url = path + next;
  url.replace("\"", ""); // gets rid of escaped quotes in the text ('\"')
  // Opening connection to server (Use 80 as port if HTTP)
  if (!client.connect(TEST_HOST, 443))
  {
    Serial.println(F("Connection failed"));
    return "Connection Failed";
  }
  // give the esp a breather
  yield();
  // Send HTTP request
  client.print(F("GET "));
  client.print(url);
  addHeaders();
  handleResponse();

  String quote;
  String _answer;
  int loadingDotXpos;

  while (client.connected())
  {
    if (client.available())
    {
      quote = client.readString();
      Serial.print("Fetching answer: ");
      _answer = getAnswer(quote);

      if (_answer.indexOf("unanswered") > 0 || _answer == "null")
      {
        Serial.println("This one has no answer");
        has_answer = false;
        break;
      }
      else
      {
        has_answer = true;
        Serial.println("This one has answer");
        break;
      }
    }
  }

  client.stop();
  getTitle(quote);
  getAnswer(quote);
  getNext(quote);
}

void loop()
{
  String next = "";

  makeHTTPRequest(n);

  if (has_answer)
  {
    display.clearDisplay();
    display.setFont();
    printText(title, 5, 2);
    delay(1000);
    printText("Ouija", 28, 38);
    delay(400);
    printText("says: ", 65, 38);
    delay(1250);
    display.setFont(&FreeSerifItalic9pt7b);
    printText(answer, 1, 63);
    xPosLoading = 5;
    delay(800);
  }
  else
  {
    Serial.println("no answer");
    printText(".", xPosLoading, 55);
    xPosLoading = xPosLoading + 3;
  }
}

String getTitle(String quote)
{
  int ouija_title_start = quote.indexOf("\"title\"");
  int ouija_title_end = quote.indexOf(", \"", ouija_title_start + 1); // we start the search from the position where "title" is
  String _title = quote.substring(ouija_title_start + 9, ouija_title_end);
  // Sanitize the string
  title.replace("\\\"", "'"); // gets rid of escaped quotes in the text ('\"')
  Serial.println(_title);
  Serial.println(" ");

  title = _title;

  return title;
}

String getAnswer(String quote)
{
  int ouija_flair_start = quote.indexOf("\"link_flair_text\"");
  int ouija_flair_end = quote.indexOf(", \"", ouija_flair_start + 1); // we start the search from the position where "link_flair_text" is
  // was ouija_flair_start + 19: "Ouija says: "
  String _answer = quote.substring(ouija_flair_start + 31, ouija_flair_end - 1);
  Serial.println(_answer);
  Serial.println("===========================");
  answer = _answer;
  return answer;
}

String getNext(String quote)
{

  int next_start = quote.indexOf("\"after\"");
  int next_end = quote.indexOf(", \"", next_start);
  String nextOuija = quote.substring(next_start + 9, next_end);
  nextOuija.replace("\\\"", "'"); // gets rid of escaped quotes in the text ('\"')
                                  //    Serial.print("about to exit the function, next: ");
                                  //    Serial.println(nextOuija);

  n = nextOuija;
  Serial.print("Next: ");
  Serial.println(n);
  return nextOuija;
}

void initScreen()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  // display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(1000);
}
// TEXT FUNCTIONS

void printText(String text, int x, int y)
{
  // Coords for Questions: 10,0
  // Coords for Answers: 10, 50
  // display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(x, y);
  display.println(text);
  display.display(); // Show initial text
  delay(100);
}

void scrollText(String q, String a)
{
  display.clearDisplay();

  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(q);
  display.display(); // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}
