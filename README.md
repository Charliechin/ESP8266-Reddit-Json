# ESP8266-Reddit-Json
Using a ESP8266, send a HTTPS request to REDDIT API to fetch title and flair

# What is this all about?

The aim of this sketch is to show on screen data from the subreddit [/r/AskOuija] and update it every few seconds.

In this subreddit, people can ask questions, and these can be answered by other users on the condition that they can only write one letter.  By linking these letters the answers are formed.

If you feel that the Ouija has completed its answer, people may comment with "Goodbye" to end the thread.

A bot will automatically mark the most voted answer as valid and the post will be updated.

## How?

- Send a GET Request to reddit.com/r/askouija/top.json
- Receive a JSON document
-- Check that the post contains a response.
- Display it
- Rinse and repeat

If the post has an answer, it displays on a generic OLED screen (in this case, we use a 0.96 Inch I2C IIC SPI Serial 128 x 64 OLED LCD)
the title of the post and the answer.

If the post has no response, we send a new GET request to proceed to repeat the previous step;
for every post with no response found, dots are painted on the screen at the bottom of the screen, to let you know that the ESP8266 is fetching new data. 

## How to make it work?

1. Change these two lines to match your SSID and password:

```
char ssid[] = "TALKTALK9707C8"; // your network SSID (name)
char password[] = "MKNJPQPK";   // your network key
```

2. Upload!


## Images

![Reddit AskOuija](https://i.imgur.com/YEf4Bvd.jpeg)




[/r/AskOuija]: https://www.reddit.com/r/askouija
