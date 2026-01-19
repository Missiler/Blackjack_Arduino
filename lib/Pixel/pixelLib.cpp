#include "PixelLib.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CHAR_W 8   // Default Adafruit GFX font
#define CHAR_H 8

//Notes for next session:

/*
Today was a very good day. Recently added: The Chain and some important choices. Also possible to control is now, spawn new, collide and bounce.

For next session:
Implement spawn of powerups.
Implement such that when collided with powerups -> gain power up. --> ADD INTO ------>  bool PixelChain::headOnPixel(PixelUnit& pixel) Turn into void and change certain values.

Add Menu, GUI and such.


*/



//Skapar en ny PixelUnit, slipepr använda spawnPixel, utan här skapas pixel vanligt.
PixelUnit::PixelUnit(char c){
    pixl = c;
    posX = 0;
    posY = 0;
}

void PixelUnit::spawnPixel(Adafruit_SSD1306& display,int spawnPosX, int spawnPosY){

    //Self explanatory: It assigns 2 spawn points and draws it
    posX = spawnPosX;
    posY = spawnPosY;

    display.setCursor(spawnPosX,spawnPosY);
    display.setTextColor(SSD1306_WHITE);
    display.print(pixl);
    display.display();

  }

void PixelUnit::setPos(int x, int y){
    posX = x;
    posY = y;
}

void PixelUnit::draw(Adafruit_SSD1306& display){
    display.setCursor(posX, posY);
    display.setTextColor(SSD1306_WHITE);
    display.print(pixl);
}

void PixelUnit::clear(Adafruit_SSD1306& display){
    display.fillRect(posX, posY, CHAR_W, CHAR_H,SSD1306_BLACK);
}

int PixelUnit::getPosX(){
    return posX;
  }

int PixelUnit::getPosY(){
    return posY;
  }

void PixelUnit::bounceChange(){
    bouncing = !bouncing;
}

void PixelUnit::movePixel(Adafruit_SSD1306& display,unsigned long period, int directionX, int directionY){


    //millis(), like in the other parts, is used as an update rate.
    unsigned long now = millis();

    if(now - lastUpdate >= period){

        //Remove the current area
        display.fillRect(posX, posY, CHAR_W, CHAR_H, SSD1306_BLACK);
        
        //If it does not bounce, pass through to the other side
        if(!bouncing){

        if(posX >= display.width() - CHAR_W){
            posX = 0;
        }
        if(posX <= 0){
            posX = display.width() - CHAR_W;
        }
        if(posY >= display.height() - CHAR_H){
            posY = 0;
        }
        if(posY <= 0){
            posY = display.height() - CHAR_H;
        }
        }


        //Have it bounce instead
        else{

        if(posX >= display.width() - CHAR_W){
            dirX = -dirX;
        }
        if(posX <= 0){
            dirX = -dirX;
        }
        if(posY >= display.height() - CHAR_H){
            dirY = -dirY;
        }
        if(posY <= 0){
            dirY = -dirY;
        }
        }

        //New position values
        posX = posX + (directionX * dirX);
        posY = posY + (directionY * dirY);

        //Draw the assigned char
        display.setCursor(posX, posY);
        display.setTextColor(SSD1306_WHITE);
        display.print(pixl);
        display.display();


        lastUpdate = now;

    }

}

void PixelUnit::spawnPowerUp(Adafruit_SSD1306& display,PixelChain& chain){
//This is made to spawn different powerups. This could mean increase speed, decrease speed or increase tail. More has to be implemented such as:
//If the spawn point is on the same place as the chain.

//Potential Solution: Search the arrays for X and Y direction such that if there's a match, generate new points.
    spawnPosX = random(0,16) * CHAR_W;
    spawnPosY = random(0,8) * CHAR_H;

    if(chain.occupies(spawnPosX, spawnPosY)){
        spawnPosX = random(1,128/CHAR_W) * CHAR_W;
        spawnPosY = random(1,64 / CHAR_H) * CHAR_H;
    }

    powerUpNum = random(0,3);
    posX = spawnPosX;
    posY = spawnPosY;
    
    switch(powerUp){
        case 0:
            powerUpPixl = '0';
        break;

        case 1:
            powerUpPixl = '1';
        break;

        case 2:
            powerUpPixl = 'M';
        break;
    }

    display.setCursor(spawnPosX,spawnPosY);
    display.setTextColor(SSD1306_WHITE);
    display.print(powerUpPixl);
    display.display();
}

int PixelUnit::getPowerUp(){
    return powerUp;
}
//Pixel Chain-----------------------

PixelChain::PixelChain(PixelUnit* headNode) {
    head = headNode;

}

void PixelChain::move(Adafruit_SSD1306& display, int moveSpeed,int dx, int dy) {

    unsigned long now = millis();

    if(now - lastUpdate < moveSpeed){
        return;
    }

    lastUpdate = now;
    //Remove the current position
    for (PixelUnit* p = head; p != nullptr; p = p->next) {
        p->clear(display);
    }

    int prevX = head->getPosX();
    int prevY = head->getPosY();

    //Set the current position to the future position.
    head->setPos(prevX + (dirX * dx), prevY +(dirY * dy));

    int x = head->getPosX();
    int y = head->getPosY();

    if(!bouncing){
    
    if (x >= display.width()){
         x = 0;
    }
    else if (x <= 0){
    x = display.width() - CHAR_W;
    }

    if (y >= display.height()){
      y = 0;
    }
    else if (y <= 0){
      y = display.height() - CHAR_H;
    }                
    }

    else{
    if (x >= display.width() - CHAR_W){
         dirX = -dirX;
    }
    else if (x <= 0){
    dirX = -dirX;
    }

    if (y >= display.height() - CHAR_H){
      dirY = -dirY;
    }
    else if (y <= 0){
      dirY = -dirY;
    }                
    }

    //Depending on the cases above, the heads position is set.
    head->setPos(x, y);


    //Go to the next and have it follow the head. This means that the only part that actually has to be "controlled" is the head, since the rest wil always follow.
    PixelUnit* current = head->next;
    while (current != nullptr) {

        int tempX = current->getPosX();
        int tempY = current->getPosY();

        current->setPos(prevX, prevY);

        prevX = tempX;
        prevY = tempY;

        current = current->next;
    }


    for (PixelUnit* p = head; p != nullptr; p = p->next) {
        p->draw(display);
    }

    display.display();
}

void PixelChain::insertAtHead(char insert){


    PixelUnit* newPixel = new PixelUnit(insert);

    if(head == NULL){
        head = newPixel;
    }

    newPixel->next = this->head;
    this->head = newPixel;

    }

void PixelChain::insertAtEnd(char insert){
    PixelUnit* newPixel = new PixelUnit(insert);

    if(head == nullptr){
        head = newPixel;
    }

    PixelUnit* last = head;
    while(last->next != nullptr){
        last = last->next;
    }

    last ->next = newPixel;
}

void PixelChain::insertPos(int pos, char insert){
    if(pos<1){
        return;
    }
    if(pos == 1){
        PixelUnit* newPixel = new PixelUnit(insert);
        newPixel->next = head;
    }
    
    PixelUnit* curr = head;

    for(int i=1; i< pos -1 && curr !=nullptr; i++){
        curr = curr->next;
    }
    if(curr == nullptr){
        return;
    }
    PixelUnit* newPixel = new PixelUnit(insert);
    newPixel->next = curr->next;
    curr->next = newPixel;

}

void PixelChain::changeBounce(){

    bouncing = !bouncing;
}

bool PixelChain::collisionWithBody(){

    //Very self explanatory
    int headX = head->getPosX();
    int headY = head->getPosY();

    PixelUnit* current = head->next;

    while(current != nullptr){
        if(current->getPosX() == headX && current->getPosY() == headY){
            return true;
        }
        current = current->next;
    }
    return false;
}

bool PixelChain::headOnPixel(PixelUnit& pixel){
    return (head->getPosX() == pixel.getPosX() && head->getPosY() == pixel.getPosY());
}

int PixelChain::getLength(){
    int count = 0;
    PixelUnit* current = head;

    while(current->next != nullptr){
        count++;
        current = current->next;
    }
    return count;

}

bool PixelChain::occupies(int x, int y) const {
    for (PixelUnit* p = head; p != nullptr; p = p->next) {
        if (p->getPosX() == x && p->getPosY() == y) {
            return true;
        }
    }
    return false;
}

void PixelChain::changeXDirPlus(){
    dirY = 0;
    dirX = 1;
}

void PixelChain::changeXDirMinus(){
    dirY = 0;
    dirX = -1;
}

void PixelChain::changeYDirPlus(){
    dirY = 1;
    dirX = 0;
}

void PixelChain::changeYDirMinus(){
    dirY = -1;
    dirX = 0;
}

void PixelChain::stopMoving(){
    dirY = 0;
    dirX = 0;
}

void PixelChain::removeAllPixels() {
    if (head == nullptr) return;
    head->next = nullptr;
    }