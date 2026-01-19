#ifndef PIXELLIB_H
#define PIXELLIB_H
#pragma once
#include <Adafruit_SSD1306.h>


class PixelChain;   // forward declaration



class PixelUnit{

public:

  PixelUnit(char c);

//Alla behöver välja vart det ska användas. ofta är det kallat "display"
  void spawnPixel(Adafruit_SSD1306& display,int spawnPosX, int spawnPosY);

  void draw(Adafruit_SSD1306& display);

  void clear(Adafruit_SSD1306& display);

  void setPos(int x, int y);

  int getPosX();

  int getPosY();

  void movePixel(Adafruit_SSD1306& display,unsigned long period, int directionX, int directionY);

  void bounceChange();

  void spawnPowerUp(Adafruit_SSD1306& display, PixelChain& chain);

  int getPowerUp();



  //void spawnPowerUp(Adafruit_SSD1306& display, int powerUp);

  PixelUnit* next = nullptr;


private:

  int spawnPosX;
  int spawnPosY;
  int speed;
  int posX;
  int posY;
  int powerUp;

  //directionX and directionY is the starting/fed direction of the system
  int directionX;
  int directionY;
  
  //dirX and dirY is if it is going in positive or negative x- and y-direction.
  int dirX = 1;
  int dirY = 1;

  char pixl;
  unsigned long lastUpdate = 0;

  char powerUpPixl;
  int powerUpNum;

  //If the character is able to bounce.
  bool bouncing = false;

  //Screen
  static const int CHAR_W = 6;
  static const int CHAR_H = 8;

  //To find if we have won
  int attempts;
  bool won;

};

// PIXEL CHAIN- Uses PIXEL UNIT

class PixelChain{
  public:
    PixelChain(PixelUnit* head);

    unsigned long moveSpeed;
    unsigned long lastUpdate = 0;

    void move(Adafruit_SSD1306& display, int moveSpeed,int dx, int dy);

    void insertAtHead(char insert);
    void insertAtEnd(char insert);
    void insertPos(int pos, char insert);
    bool collisionWithBody();
    bool headOnPixel(PixelUnit& food);
    void removeAllPixels();

    int getLength();
    bool occupies(int x, int y) const;

    void changeXDirPlus();
    void changeXDirMinus();
    void changeYDirPlus();
    void changeYDirMinus();
    void stopMoving();

    void changeBounce();

    private:

    PixelUnit*head;
    boolean bouncing;
    int dirX = 0;
    int dirY = 0;
};


#endif