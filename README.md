# BrewServerV1.2
A Beer Brewing wireless fermentation monitor

Code is functional.  
Feel free to modify or ask qusetions on Facebook in our "IoT Tech Shop" group https://www.facebook.com/groups/3188341227932417/ or Bill's Brews group at https://www.facebook.com/groups/1192890997887938/

Also, the index.html file must be loaded in the data section of your Arduino project and uploaded to your ESP32 using "ESP32 Sketch Data Upload"under Tools.  This is the webpage being server up by the ESP32.  You can modify that to your liking.  I The data update times are controlled by the web page from the browser.

Based on the Heltec Lora Wifi 32 V2 Boards (ESP32)  Should also work with the Non-LoRa Heltec board with OLED Display
Amazon Links: LoRa- https://amzn.to/3nyuue5  Non-LoRa- https://amzn.to/3nyuue5

Microphone should be attached to either top cap or side of the airlock to listen for bloops. Adjust sensitivity for consistant counts.

Fermentation progress monitored by listening for "CO2 Bloops". 
DAOKI 5PCS High Sensitivity Sound Microphone Sensor Detection Module for Arduino AVR PIC is the sound sensor
Amazon Link: https://amzn.to/3A4Gh8Y

Temperature Probe is any 18b20 Temp Sensor.  Be sure to add 4.7K pullup resistor. I secure probe to outside surface of my fermentor with tape so as to not contaminate wort.
Amazon Link: https://amzn.to/3nxvC1E

Suggested airlocks:

https://amzn.to/3A6h0eB

https://amzn.to/3tC7wGK




