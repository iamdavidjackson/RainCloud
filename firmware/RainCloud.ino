// -----------------------------------
// Controlling RainClouds over the Internet
// -----------------------------------

int waterLevelSwitches[4] = {D0, D1, D2, D3};
int waterLevel = 0;

int rainBarrelTap = D4;
int faucetTap = D5;

int soilHumidityPin = A0;
double soilHumidity = 0;

int airHumidityPin = A1;
double airHumidity = 0;

int airTemperaturePin = A2;
double airTemperature = 0;

boolean rainBarrelTapOn = false;
boolean faucetTapOn = false;

void setup()
{

   // Here's the pin configurations
   pinMode(rainBarrelTap, OUTPUT);
   pinMode(faucetTap, OUTPUT);
   pinMode(soilHumidityPin, INPUT);
   pinMode(airHumidityPin, INPUT);
   pinMode(airTemperaturePin, INPUT);

   int i;
   for (i = 0; i < 4; i = i + 1) {
     pinMode(waterLevelSwitches[i], INPUT);
   }

   // Register functions
   Particle.function("water", waterToggle);

   // Register Variables
   Particle.variable("waterLevel", waterLevel);
   Particle.variable("soilHumidity", soilHumidity);
   Particle.variable("airHumidity", airHumidity);
   Particle.variable("airTemperature", airTemperature);

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

  reading = analogRead(soilHumidityPin);
  soilHumidity = (((reading * 3.3)/4095) - 0.5) * 100;

  reading = analogRead(airHumidityPin);
  airHumidity = (((reading * 3.3)/4095) - 0.5) * 100;

  reading = analogRead(airTemperaturePin);
  airTemperature = (((reading * 3.3)/4095) - 0.5) * 100;

  // check if the rain barrel tap is open and close to running out of water
  if (rainBarrelTapOn && waterLevel < 25) {
    turnFaucetOn();
  }

  delay(100);
}

int waterToggle(String command) {

    if (command == "on") {
      if (waterLevel > 25) {
        turnRainBarrelOn();
      } else {
        turnFaucetOn();
      }
      return 1;
    }
    else if (command == "off") {
      turnAllOff();
      return 0;
    }
    else {
      return -1;
    }
}

void turnFaucetOn() {
  faucetTapOn = true;
  rainBarrelTapOn = false;
  digitalWrite(faucetTap, HIGH);
  digitalWrite(rainBarrelTap, LOW);
  Particle.publish("tap_on","faucet");
  Particle.publish("tap_off","rainBarrel");
}

void turnRainBarrelOn() {
  rainBarrelTapOn = true;
  faucetTapOn = false;
  digitalWrite(rainBarrelTap, HIGH);
  digitalWrite(faucetTap, LOW);
  Particle.publish("tap_off","faucet");
  Particle.publish("tap_on","rainBarrel");
}

void turnAllOff() {
  rainBarrelTapOn = false;
  faucetTapOn = false;
  digitalWrite(rainBarrelTap, LOW);
  digitalWrite(faucetTap, LOW);
  Particle.publish("tap_off","faucet");
  Particle.publish("tap_off","rainBarrel");
}
