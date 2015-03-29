// LED Strips
#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11
#define FADE_SPEED 20

// Mic & Photoresistor
#define MIC_PIN A1
#define PHOTOCELL_PIN A0
const int MAX_LIGHT = 290;

// Remote
#define VALID_SIGNAL_PIN 2
#define CHANNEL_A_PIN 3
#define CHANNEL_B_PIN 4
#define CHANNEL_C_PIN 5
#define CHANNEL_D_PIN 6

boolean enabled = false;
boolean lightSensitive = false;
boolean soundSensitive = false;
int mode = 0; // 0 = fade, 1 = red, 2 = violet, 3 = blue, 4 = cyan, 5 = green, 6 = yellow, 7 = white

void setup()
{
    // lights
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    
    // set inital light values
    allOff();
    
    // remote
    pinMode(VALID_SIGNAL_PIN, INPUT);
    pinMode(CHANNEL_A_PIN, INPUT);
    pinMode(CHANNEL_B_PIN, INPUT);
    pinMode(CHANNEL_C_PIN, INPUT);
    pinMode(CHANNEL_D_PIN, INPUT);
    
    Serial.begin(9600);
}

void loop()
{
    // debug sound
    int soundLevel = analogRead(MIC_PIN);
    Serial.println(255 - (soundLevel / 4));
  
    // Toggle on/off
    if(buttonPressed() == 'A')
    {
      if(enabled) enabled = false;
      else if(!enabled) enabled = true;
      
      delay(200); // Takes care of unintentional button presses
    }
                
    // Toggle light sensitivity
    if(buttonPressed() == 'D')
    {
      if(lightSensitive) lightSensitive = false;
      else if(!lightSensitive) lightSensitive = true;
      
      delay(200); // Takes care of unintentional button presses
    }
    
    // Toggle sound sensitivity
    if(buttonPressed() == 'C')
    {
      if(soundSensitive) soundSensitive = false;
      else if(!soundSensitive) soundSensitive = true;
      
      delay(200); // Takes care of unintentional button presses
    }
    
    // Handle mode changing
    if(buttonPressed() == 'B')
    {
      mode++;
      if(mode == 9) mode = 0; // Limits the number of modes to 8
      
      delay(200); // Takes care of unintentional button presses
    }
  
    // Print all information to the console
    Serial.print("Enabled: ");
    Serial.print(enabled);
    Serial.print(", Light sensitive: ");
    Serial.print(lightSensitive);
    Serial.print(", Sound sensitive: ");
    Serial.print(soundSensitive);
    Serial.print(", Current mode: ");
    Serial.println(mode);
  
    // Return if disabled
    if(enabled == false) 
    {
      allOff();
      return;
    }
    
    // Handle lighting
    switch(mode)
    {
      case 0:
        allOff();
        fadeLights();
        break;
      case 1:
        allOff();
        if(!checkLight()) 
        {
          if(soundSensitive)
          {
            analogWrite(RED_PIN, getSoundLevel());
          }
          else
          {
            digitalWrite(RED_PIN, HIGH);
          }
        }
        break;
      case 2:
        allOff();
        if(!checkLight()) 
        {
          digitalWrite(RED_PIN, HIGH);
          digitalWrite(BLUE_PIN, HIGH);
        }
        break;
      case 3:
        allOff();
        if(!checkLight()) 
        {
          digitalWrite(BLUE_PIN, HIGH);
        }
        break;
      case 4:
        allOff();
        if(!checkLight()) 
        {
          digitalWrite(GREEN_PIN, HIGH);
          digitalWrite(BLUE_PIN, HIGH);
        }
        break;
      case 5:
        allOff();
        if(!checkLight()) 
        {
          digitalWrite(GREEN_PIN, HIGH);
        }
        break;
      case 6:
        allOff();
        if(!checkLight()) 
        {
          digitalWrite(GREEN_PIN, HIGH);
          digitalWrite(RED_PIN, HIGH);
        }
        break;
      case 7:
        allOff();
        if(!checkLight())
        {
          digitalWrite(RED_PIN, HIGH);
          digitalWrite(GREEN_PIN, HIGH);
          digitalWrite(BLUE_PIN, HIGH);
        }
        break;
      case 8:
        allOff();
        if(!checkLight())
        {
          rainbow();
        }
    }
}

/**
  * Returns the character corresponding to the pressed button and 0 for no pressed button
  */
char buttonPressed()
{
  if(digitalRead(CHANNEL_A_PIN) == HIGH) return 'A';
  else if(digitalRead(CHANNEL_B_PIN) == HIGH) return 'B';
  else if(digitalRead(CHANNEL_C_PIN) == HIGH) return 'C'; 
  else if(digitalRead(CHANNEL_D_PIN) == HIGH) return 'D';
  else return 0;
}

/**
  * Turns all lights off
  */ 
void allOff()
{
  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 0);
}

/**
  * Flashes the colors randomly
  */
void rainbow()
{
  long num = random(8);
  
  switch(num)
  {
    case 0:
      analogWrite(RED_PIN, 255);
      break;
    case 1:
      analogWrite(GREEN_PIN, 255); 
      break;
    case 2:
      analogWrite(BLUE_PIN, 255);
      break;
    case 3:
      analogWrite(RED_PIN, 255);
      analogWrite(BLUE_PIN, 255);
      break;
    case 4:
      analogWrite(BLUE_PIN, 255);
      analogWrite(GREEN_PIN, 255);
      break;
    case 5:
      analogWrite(RED_PIN, 255);
      analogWrite(GREEN_PIN, 127);
      break;
    case 6:
      analogWrite(RED_PIN, 255);
      analogWrite(GREEN_PIN, 255);
      break;
    case 7:
      analogWrite(RED_PIN, 255);
      analogWrite(GREEN_PIN, 192);
      analogWrite(BLUE_PIN, 203);
      break;
  }
  
  delay(0);
}

/**
  * Fades the lights through the sequence RED -> VIOLET -> BLUE -> CYAN -> GREEN -> YELLOW -> WHITE
  */
void fadeLights()
{
    int i;

    // Fade red in (RED)
    for (i = 0; i < 256; i++)
    {   
        if(checkLight() || buttonPressed() != 0) return;
        
        analogWrite(RED_PIN, i);
        delay(FADE_SPEED);
    }
    
    // Fade blue in (VIOLET)
    for (i = 0; i < 256; i++)
    {   
        if(checkLight() || buttonPressed() != 0) return;
        
        analogWrite(BLUE_PIN, i);
        delay(FADE_SPEED);
    }
    
    // Fade red out (BLUE)
    for (i = 255; i > 0; i--)
    {
        if(checkLight() || buttonPressed() != 0) return;
        
        analogWrite(RED_PIN, i);
        delay(FADE_SPEED);
    }
    
    // Fade green in (CYAN)
    for(i = 0; i < 256; i++) 
    {
        if(checkLight() || buttonPressed() != 0) return;
        
        analogWrite(GREEN_PIN, i);
        delay(FADE_SPEED);
    }
    
    // Fade blue out (GREEN)
    for(i = 255; i > 0; i--)
    {
        // check light first
        if(checkLight() || buttonPressed() != 0) return;
        
        // handle lighting
        analogWrite(BLUE_PIN, i);
        delay(FADE_SPEED);
    }
    
    // Fade red in (YELLOW)
    for(i = 0; i < 256; i++)
    {
        if(checkLight() || buttonPressed() != 0) return;
        
        analogWrite(RED_PIN, i);
        delay(FADE_SPEED);
    }
    
    // Fade blue in (WHITE)
    for(i = 0; i < 256; i++)
    {   
        if(checkLight() || buttonPressed() != 0) return;
        
        analogWrite(BLUE_PIN, i);
        delay(FADE_SPEED);
    }
    
    // Fade all out (BLACK/OFF)
    for (i = 255; i > 0; i--)
    {
        if(checkLight() || buttonPressed() != 0) return;
        
        analogWrite(RED_PIN, i);
        analogWrite(GREEN_PIN, i);
        analogWrite(BLUE_PIN, i);
        delay(FADE_SPEED);
    }
}

/**
  * Returns the sound level scaled for light control
  */
int getSoundLevel()
{
  return 255 - (analogRead(MIC_PIN) / 4);
}
  
/**
  * Checks the ambient light level and returns true if it's above the threshold set by (const) MAX_LIGHT
  */
boolean checkLight()
{
    if(!lightSensitive) return false;
    
    int photocellReading = analogRead(PHOTOCELL_PIN);
    
    if(photocellReading > MAX_LIGHT)
    {
      // Stop all lights
      allOff();
      
      return true;  
    }
    
    return false;
}
