#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <pixelLib.h>
#include <BlackjackLib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

int currentState = 0;

struct playerHands{
  String player1Hand;
  String dealerHand;
};
//Input Pins:
struct inputPins{

  int ipin2 = 2;
  int ipin3 = 3;
  int ipin4 = 4;
  int ipin5 = 5;
  int ipin6 = 6;
  int ipin7 = 7;
};


//Positioning
struct position
{

  int player1HandPosX = 5;
  int player1HandPosY = 5;

};

//Directions 
// +1 down/right
// -1 left/up
struct direction{

  int directionX = 0;
  int directionY = 0;
  int lastDirX = 0;
  int lastDirY = 0;

};


//Time
struct timeVars{

  int startMillis;
  int currentMillis;
  unsigned long lastUpdate;
  unsigned long lastButtonUpdate;
  const unsigned long spawnInterval = 1000;
  unsigned long buttonInterval = 50;
  int period = 100;
  int updateRate;

};

struct screenDims{

  int CHAR_W = 8;
  int CHAR_H = 8;

};


//Controller:
struct controllers{

bool lastButton2 = LOW;
bool lastButton3 = LOW;
bool lastButton4 = LOW;
bool lastButton5 = LOW;
bool lastButton6 = LOW;
bool lastButton7 = LOW;

bool button2; //Upbutton
bool button3; //Left Button
bool button4; //Right Button
bool button5; //Down button
bool button6; //green button top
bool button7; //green bottom bot


char inputChar;

int moveSpeed = 250;

};

//Debugging Variables
struct Debugging{

int numL = 0;
int numX = 0;
int numM = 0;
int numA = 0;

int currCardVal = 0;
String currCardsuit;
String currCardType;

bool failed = false;

int numCollide = 0;
String debugM = "Its Fine";

int player1Hand = 0;
int dealerHand = 0;

};



struct powerUpParams{

  bool spawned = false;


};

inputPins pins;
position pos;
direction dir;
timeVars timing;
screenDims screen;
controllers ctrl;
Debugging debug;
powerUpParams puParams;
playerHands hands;

struct menuPositions{


  int welcomeTextPosX = 5 * screen.CHAR_W;
  int welcomeTextPosY = 0 * screen.CHAR_H;

  int playTextPosX = 2 * screen.CHAR_W;
  int playTextPosY = 5 * screen.CHAR_H;
  int playText2PosX = 5 * screen.CHAR_W;
  int playText2PosY = 6 * screen.CHAR_H; 

  int dealerNamePosX = 6 * screen.CHAR_W;
  int dealerNamePosY = 0 * screen.CHAR_H;

  int dealerResultPosX = 12 * screen.CHAR_W;
  int dealerResultPosY = 0 * screen.CHAR_H;

  int dealerValueX = 5 * screen.CHAR_W;
  int dealerValueY = 1 * screen.CHAR_H;

  int dealerHandX = 8 * screen.CHAR_W;
  int dealerHandY = 2 * screen.CHAR_H;


  int playerNameX = 6 * screen.CHAR_W;
  int playerNameY = 5 * screen.CHAR_H;

  int playerValueX = 5 * screen.CHAR_W;
  int playerValueY = 5 * screen.CHAR_H;

  int playerHandX = 6 * screen.CHAR_W;
  int playerHandY = 7 * screen.CHAR_H;

  int hitPositionX = 0 * screen.CHAR_W;
  int hitPositionY = 7 * screen.CHAR_H;

  int standPositionX = 10 * screen.CHAR_W;
  int standPositionY = 7 * screen.CHAR_H;

};

struct menuText{

  String welcomeText = "Blackjack";
  String playText = "Press Up Button";
  String playText2 = "To play";

  String dealerName = "Dealer";
  String PlayerName = "Player";

  String standText = "Stand";
  String hitText = "Hit";

};

enum GameState {
  ST_TITLE,
  ST_DEAL_INITIAL,
  ST_PLAYER_TURN,
  ST_DEALER_TURN,
  ST_RESULTS
};

GameState state = ST_TITLE;

struct playerCards{

  String HiddenDealerType;
  String HiddenDealerSuit;

};


menuPositions menuPos;
menuText menuT;
playerCards holdCard;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Deck deck1;

BlackjackGame blackjack;

int total_players = 4;

//Functions ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//The code between is AI-generated as I could not be bothered to redo the entire thing on my own. It is not important code, just annoying to add----------------------------------------------------------------
struct ButtonEntry {
  int pin;
  bool last;           // last read state
  ButtonAction action; // what this button means
};

// Table of buttons (maps pins to actions)
ButtonEntry buttons[] = {
  {pins.ipin2, LOW, ButtonAction::Up},
  {pins.ipin3, LOW, ButtonAction::Left},
  {pins.ipin4, LOW, ButtonAction::Right},
  {pins.ipin5, LOW, ButtonAction::Down},
  {pins.ipin6, LOW, ButtonAction::GreenTop},
  {pins.ipin7, LOW, ButtonAction::GreenBottom}
};

const int NUM_BUTTONS = sizeof(buttons) / sizeof(buttons[0]);

ButtonAction readControls()
{
  // button interval gating (your existing timing logic)
  if (timing.currentMillis - timing.lastButtonUpdate < timing.buttonInterval) {
    return ButtonAction::None;
  }
  timing.lastButtonUpdate = timing.currentMillis;

  // Scan buttons and detect rising edge (LOW -> HIGH)
  for (int i = 0; i < NUM_BUTTONS; i++) {
    bool now = digitalRead(buttons[i].pin);

    if (now == HIGH && buttons[i].last == LOW) {
      buttons[i].last = now;          // update last before returning
      return buttons[i].action;       // report the first detected press
    }

    buttons[i].last = now;
  }

  return ButtonAction::None;
}


void handleAction(ButtonAction action)
{
  switch (action)
  {
    case ButtonAction::Up:
      // TODO: Up pressed

      break;

    case ButtonAction::Left:
      // TODO: Left pressed
      break;

    case ButtonAction::Right:
      // TODO: Right pressed
      break;

    case ButtonAction::Down:
      // TODO: Down pressed
      break;

    case ButtonAction::GreenTop:
      // TODO: Green top pressed
      break;

    case ButtonAction::GreenBottom:
      // TODO: Green bottom pressed
      break;

    case ButtonAction::None:
    default:
      break;
  }
}
//AI Finished ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void updateUI(){

for (int i = 0; i <= total_players; i++) {
  //Player Names
  auto pos = blackjack.getPlayerIndexPos(i);
  display.setCursor(pos[0], pos[1]);
  display.print(blackjack.getPlayerIndexName(i));

  //Player Turn
  if(blackjack.getPlayerIndexCurrentTurn(i) == true){
    display.drawLine(pos[0],pos[1] + screen.CHAR_H,pos[0] + screen.CHAR_W * 1, pos[1] + screen.CHAR_H,SSD1306_WHITE);
  }
  //Player Cards
  if(i == 0){
  display.setCursor(pos[0] + (screen.CHAR_W/2),pos[1]+screen.CHAR_H);
  }
  else{
  display.setCursor(pos[0] + (screen.CHAR_W * 3),pos[1]);
  }
  display.print(blackjack.getPlayerIndexValue(i));

  //Player Won
  if(blackjack.getPlayerIndexWon(i) == true){
    display.drawRect(pos[0],pos[1],screen.CHAR_W * 2, screen.CHAR_H,SSD1306_WHITE);
  }
}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  Wire.begin();   // REQUIRED on Nano 33 BLE Sense Rev2
  Wire.setClock(400000);  // Set I2C clock speed to 400kHz
  
  pinMode(pins.ipin2, INPUT); //uses the internal 10k ohm resistor
  pinMode(pins.ipin3, INPUT); //uses the internal 10k ohm resistor
  pinMode(pins.ipin4, INPUT); //uses the internal 10k ohm resistor
  pinMode(pins.ipin5, INPUT); //uses the internal 10k ohm resistor
  pinMode(pins.ipin6, INPUT); //uses the internal 10k ohm resistor
  pinMode(pins.ipin7, INPUT); //uses the internal 10k ohm resistor


  pinMode(A0, INPUT);
  randomSeed(analogRead(A0));


  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);   // Stop if OLED fails
  }

  //Start the entire thing
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 32);
  display.println("?");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
  display.setCursor(0,0);
  deck1.createPlayingDeck(4);
  deck1.shuffle();

  blackjack.gameInit(deck1);
  for(int i = 0; i < total_players; i++){
    blackjack.createPlayers("P"+String(i+1));
  }
  blackjack.reversePlayers();
  
  blackjack.setPlayerPosition();
}
void loop() {

  timing.currentMillis = millis();
  //Start MENU ---------------------------------------------------------------------------------------------------
  //Controlling Logic
  display.clearDisplay();
  ButtonAction action = readControls();
  blackjack.update(action);

  updateUI();
  display.display();

}

/*Till Nästa gång:

Nu är alla funktioner klara för att bygga vidare. Efter gymmet ska jag:

0. Ladda upp på Github nu när allt funkar

1. Skriva klart projektplanen och skicka in den så att Uwe kan se.

2. Skapa ett UI så att det ser snyggt ut för mig så att det är lätt att gå vidare härifrån.

3. Kanske ladda upp allt på GitHub.

4. Sluta få ont i magen.


*/