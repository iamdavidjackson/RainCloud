// -----------------------------------
// Controlling RainClouds over the Internet
// -----------------------------------

int waterLevelSwitches[4] = {D0, D1, D2, D3};
int waterLevel = 100;

int rainBarrelTap = D7;
int faucetTap = D5;

boolean rainBarrelTapOn = false;
boolean faucetTapOn = false;

void setup()
{

   // Here's the pin configurations
   pinMode(rainBarrelTap, OUTPUT);
   pinMode(faucetTap, OUTPUT);

   int i;
   for (i = 0; i < 4; i = i + 1) {
     pinMode(waterLevelSwitches[i], INPUT);
   }

   // Register functions
   Particle.function("water", waterToggle);

   // Register Variables
   Particle.variable("waterLevel", &waterLevel, INT);

   // Turn both taps off
   digitalWrite(rainBarrelTap, LOW);
   digitalWrite(faucetTap, LOW);

}

void loop()
{
  int i;
  int reading;
  int runningWaterLevel = 0;
  for (i = 0; i < 4; i = i + 1) {
    reading = digitalRead(waterLevelSwitches[i]);
    if (reading == HIGH) {
      runningWaterLevel = runningWaterLevel + 25;
    }
  }

  if (waterLevel != runningWaterLevel) {
    waterLevel = runningWaterLevel;
    Particle.publish("water_level_changed",String(waterLevel));
  }

  // check if the rain barrel tap is open and close to running out of water
  if (rainBarrelTapOn && waterLevel < 25) {
    faucetTapOn = true;
    rainBarrelTapOn = false;
    digitalWrite(faucetTap, HIGH);
    digitalWrite(rainBarrelTap, LOW);
    Particle.publish("tap_on","faucet");
    Particle.publish("tap_off","rainBarrel");
  }

  delay(100);
}

int waterToggle(String command) {

    if (command == "on") {
      if (waterLevel > 25) {
        rainBarrelTapOn = true;
        faucetTapOn = false;
        digitalWrite(rainBarrelTap, HIGH);
        digitalWrite(faucetTap, LOW);
        Particle.publish("tap_off","faucet");
        Particle.publish("tap_on","rainBarrel");
      } else {
        faucetTapOn = true;
        rainBarrelTapOn = false;
        digitalWrite(faucetTap, HIGH);
        digitalWrite(rainBarrelTap, LOW);
        Particle.publish("tap_on","faucet");
        Particle.publish("tap_off","rainBarrel");
      }
      return 1;
    }
    else if (command == "off") {
      digitalWrite(rainBarrelTap, LOW);
      digitalWrite(faucetTap, LOW);
      Particle.publish("tap_off","faucet");
      Particle.publish("tap_off","rainBarrel");
      return 0;
    }
    else {
      return -1;
    }
}
