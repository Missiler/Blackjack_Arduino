#ifndef BlackjackLib_H
#define BlackjackLib_H
#pragma once
#include <Adafruit_SSD1306.h>
#include <array>
#include <vector>

enum class ButtonAction {
  None,
  Up,
  Left,
  Right,
  Down,
  GreenTop,
  GreenBottom,
  Split
};

class Player;

class Card{

public:

    Card(String val, String suitAssign);

    int getVal();
    String getSuit();
    String getType();
    void revealCard();
    bool getFaceDir();
    void setFaceDown();
    void setFaceUp();

    Card* next = nullptr; 

private:

    String type;
    int value;
    String suit;
    bool faceDirUp = true;


};

class Deck{

    public:

        void createPlayingDeck(int num_of_decks);
        void insertCard(String value, String suitAssign);
        int countCards();
        void shuffle();
        int drawCard();
        void dealCard(Deck& deck, Player& player);
        void setCardFaceDown();
        void setCardFaceUp();

        String getCardSuit();
        int getCardVal();
        String getCardType();
        bool getCardFaceUp();
        void clear();


    private:
        Card* head;


};

class Hand{

    public:
        Hand();
        ~Hand();

        int numOfCards();
        void addCard(String value, String suitAssign);
        String checkCards();
        int getValue();
        boolean isBusted();
        void clear();
        void revealHiddenCard();
        void setLastCardFaceDown();
        void setFirstCardFaceDown();
        bool canSplit();
        Hand* split();

    private:
        Card* head;
        int cardCount;
        int handValue;
        int aceCount;
};


class Player{

    public:

        //Initilize the player
        Player(String name);
        ~Player();

        //Player hands
        int getNumHands();
        Hand* getHand(int index);
        void addHand(Hand* hand);
        void splitHand(int handIndex);
        void clearHands();
        bool hasActiveHand();
        int getActiveHandIndex();
        void setActiveHandIndex(int index);
        
        // For backward compatibility - operate on active hand
        int numOfCards();
        String checkHandCards();
        int getHandVal();
        boolean isBusted();
        void addToHand(String value, String suitAssign);
        boolean checkSplit();
        void clearHand();
        void revealHiddenCard();
        void setLastCardFaceDown();
        void setFirstCardFaceDown();
        bool getWonState();
        void setWonStateWon();
        void setWonStateLoss();
        void pushResult();
        
        Player* next = nullptr;

        //Private information
        String getName();
        int getPosX();
        int getPosY();
        void setPos(int x, int y);
        int getPoints();
        int setPoints();
        int betPoints();
        bool isTurn();
        void setTurnFalse();
        void setTurnTrue();
        void setPushFalse();

    private:

    std::vector<Hand*> hands;
    int activeHandIndex;
    int points = 0;

    bool wonCase = false;
    bool push = false;
    bool isTurnNow = false;

    int posX;
    int posY;

    String name;

};


class BlackjackGame{

    public:

    //Initilize the game
    void gameInit(Deck& deck);

    enum class GameState {
        InitialState,
        WaitingForPlayers,
        Dealing,
        PlayerTurn,
        DealerTurn,
        Finished,
        DealerTurnPause,
        Restart
    };

    //States
    void setState(GameState newState);
    GameState getState() const;
    void update(ButtonAction action = ButtonAction::None, unsigned long currentMillis = 0);
    String getStateString();

    //Each State function
    void initialStateFunction();
    void waitingForPlayersFunction();
    void dealingFunction(unsigned long currentMillis);
    void playerTurnFunction(ButtonAction action);
    void dealerTurnFunction(unsigned long currentMillis);
    void finishedFunction();
    void restartFunction(ButtonAction action);

    //Players
    void createPlayers(String name);
    String getPlayers();
    void reversePlayers();
    String getPlayerHands();
    int getPlayerVal();
    void setPlayerPosition();
    String getPlayerIndexHand(int index);
    int getPlayerIndexValue(int index);
    std::array<int,2> getPlayerIndexPos(int index);
    String getPlayerIndexName(int index);
    bool getPlayerIndexWon(int index);
    int getPlayerIndexNumberOfCards(int index);
    bool getPlayerIndexCurrentTurn(int index);


    private:
        void moveToNextHandOrPlayer();

        int num_of_players = 0;
        GameState state = GameState::WaitingForPlayers;
        Player* head = nullptr;
        Deck* deckPtr = nullptr;
        Player* currentPlayer = nullptr;
        int dealingStep = 0;
        unsigned long lastDealTime = 0;
        int dealerTurnStep = 0;
};

#endif