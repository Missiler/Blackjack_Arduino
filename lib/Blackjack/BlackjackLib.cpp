#include "BlackjackLib.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <iostream>
#include <string>
#include <array>

#define CHAR_W 6   // Default Adafruit GFX font
#define CHAR_H 8
#define SIZE_X 128
#define SIZE_Y 64

//Korten som ska användas är relativt simpla. Endast clothing och value

class Card;
//CARD ---------------------------------------------------------------
Card::Card(String val, String suitAssign){

    suit = suitAssign;
    type = val;
    faceDirUp = true;
    if(val == "J" || val == "Q" || val == "K"){
        value = 10;
    }
    else if(val == "A"){
        value = 11;
    }
    else{
        value = val.toInt();
    }
}

String Card::getSuit(){
    return suit;
}

int Card::getVal(){
    return value;
}

String Card::getType(){
    if(faceDirUp == false){
        return "?";
    }
    return type;
}

void Card::revealCard(){
    faceDirUp = true;
}

bool Card::getFaceDir(){
    return faceDirUp;
}

void Card::setFaceDown(){
    faceDirUp = false;
}

void Card::setFaceUp(){
    faceDirUp = true;
}

//DECK --------------------------------------------------------------------------------

void Deck::insertCard(String value, String suitAssign)
{
    Card* node = new Card(value, suitAssign);
    node->next = head;
    head = node;
}

int Deck::drawCard()
{
    if (head == nullptr) {
        return 0;   
    }

    Card* drawn = head;         
    int cardVal = drawn->getVal();

    head = head->next;           
    delete drawn;                

    return cardVal;
}

int Deck::getCardVal(){
    if(head == nullptr){
        return 0;
    }
    return head ->getVal();
}

String Deck::getCardSuit(){
    if(head == nullptr){
        return "";
    }
    return head->getSuit();
}

String Deck::getCardType(){
    if(head == nullptr){
        return "";
    }

    if(head->getFaceDir() == false){
        return "?";
    }
    return head ->getType();
}

void Deck::createPlayingDeck(int num_of_decks){

    const int num_of_suits = 4;
    const int num_of_values = 13;
    
    static const String suits[num_of_suits] = {"Hearts","Diamonds","Spades","Clubs"};
    static const String values[num_of_values] = {"2","3","4","5","6","7","8","9","10","J","Q","K","A"};

    for (int a = 0; a < num_of_decks; a++){
        for(int i = 0; i < num_of_suits; i++){
            for(int k = 0; k<num_of_values; k++){
                insertCard(values[k],suits[i]);
            }
        }
    }
}

void Deck::clear()
{
    while (head != nullptr) {
        Card* tmp = head;
        head = head->next;
        delete tmp;
    }
    head = nullptr;
}

bool Deck::getCardFaceUp(){
    if(head == nullptr){
        return false;
    }
    return head->getFaceDir();
}

int Deck::countCards() {
    int count = 0;
    Card* curr = head;
    while (curr != nullptr) {
        count++;
        curr = curr->next;
    }
    return count;
}

void Deck::shuffle()
{
    int n = countCards();
    if (n < 2) return;

    // Step 1: Copy pointers into array
    Card** arr = new Card*[n];
    Card* curr = head;

    for (int i = 0; i < n; i++) {
        arr[i] = curr;
        curr = curr->next;
    }

    // Step 2: Fisher-Yates shuffle
    for (int i = n - 1; i > 0; i--) {
        int j = random(i + 1);
        Card* temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }

    // Step 3: Rebuild linked list
    head = arr[0];
    for (int i = 0; i < n - 1; i++) {
        arr[i]->next = arr[i + 1];
    }
    arr[n - 1]->next = nullptr;

    delete[] arr;
}

void Deck::dealCard(Deck& deck, Player& player){

    String type = deck.getCardType();
    String suit = deck.getCardSuit();
    player.addToHand(type,suit);
    deck.drawCard();
}

void Deck::setCardFaceDown(){
    if(head == nullptr){
        return;
    }
    head->setFaceDown();
}

void Deck::setCardFaceUp(){
    if(head == nullptr){
        return;
    }
    head->setFaceUp();
}

// PLAYER ---------------------------------------------------
Player::Player(String name) {
    this->name = name;
    head = nullptr;
    next = nullptr;
    cardCount = 0;
    handValue = 0;
    aceCount = 0;
    numOfHands = 1;
    points = 0;
    posX = 0;
    posY = 0;
}

int Player::numOfCards(){
    return cardCount;
}

void Player::addToHand(String value, String suitAssign)
{
    cardCount++;

    Card* node = new Card(value, suitAssign);
    node->next = head;
    head = node;

    int v = head->getVal();           // 2-10, 10 for face, 11 for Ace

    // Add the card
    handValue += v;

    if (value == "A") {
        aceCount++;
    }
}

String Player::checkHandCards()
{
    String out = "";
    Card* curr = head;

    while (curr != nullptr) {
        if (out.length() > 0) out += "|";
        out += curr->getType();
        curr = curr->next;
    }

    return out;
}

int Player::getHandVal(){
    int val = handValue;
    int aces = aceCount;
    while (val > 21 && aces > 0) {
        val -= 10;
        aces--;
    }
    return val;
}

boolean Player::checkSplit(){

    if (cardCount != 2 || head == nullptr || head->next == nullptr) {
        return false;
    }

    int val1 = head->getVal();
    int val2 = head->next->getVal();

    return val1 == val2;
}

void Player::clearHand()
{
    while (head != nullptr) {
        Card* tmp = head;
        head = head->next;
        delete tmp;
    }

    head = nullptr;
    cardCount = 0;
    handValue = 0;
    aceCount = 0;

    // If you track soft aces, also reset it:
    // softAces = 0;
}

boolean Player::isBusted(){
    
    if(getHandVal() > 21){
        return true;
    }
    return false;
}

String Player::getName(){
    return name;
}

int Player::getPosX(){
    return posX;
}

int Player::getPosY(){
    return posY;
}

void Player::setPos(int x, int y){
    posX = x;
    posY = y;
}

void Player::revealHiddenCard() {
    Card* cur = head;

    while (cur != nullptr) {
        cur->setFaceUp();
        cur = cur->next;
    }
}

void Player::setLastCardFaceDown() {
    if (head) {
        head->setFaceDown();
    }
}

void Player::setFirstCardFaceDown() {
    if (head && head->next) {
        head->next->setFaceDown();
    } else if (head) {
        head->setFaceDown();
    }
}

bool Player::getWonState(){
    return wonCase;
}

void Player::setWonStateWon(){
    wonCase = true;
}

void Player::setWonStateLoss(){
    wonCase = false;
}

void Player::pushResult(){
    push = true;
    wonCase = false;
}

bool Player::isTurn()
{
    return isTurnNow;
}

void Player::setTurnFalse(){
    isTurnNow = false;
}
void Player::setTurnTrue(){
    isTurnNow = true;
}
void Player::setPushFalse(){
    push = false;
}


//GAME -------------------------------------------------------
void BlackjackGame::setState(GameState newState){
    state = newState;
}

BlackjackGame::GameState BlackjackGame::getState() const{
    return state;
}

String BlackjackGame::getStateString(){
    switch(state){
        case GameState::InitialState: return "InitialState";
        case GameState::WaitingForPlayers: return "WaitingForPlayers";
        case GameState::Dealing: return "Dealing";
        case GameState::PlayerTurn: return "Player Turn";
        case GameState::DealerTurn: return "Dealer Turn";
        case GameState::Finished: return "Finish State";
        case GameState::Restart: return "Restart";
    }
}

void BlackjackGame::createPlayers(String name){
    Player* node = new Player(name);
    num_of_players++;
    node->next = head;
    head = node;
}

String BlackjackGame::getPlayers(){

    String out = "";
    Player* curr = head;

    while (curr != nullptr) {
        if (out.length() > 0) out += ", ";
        out += curr->getName();
        curr = curr->next;
    }

    return out;
}

int BlackjackGame::getPlayerVal(){
    return head->getHandVal();
}

String BlackjackGame::getPlayerHands(){

    String out = "";
    Player* curr = head;

    while (curr != nullptr) {
        if (out.length() > 0) out += ", ";
        out += curr->checkHandCards();
        curr = curr->next;
    }

    return out;

}

String BlackjackGame::getPlayerIndexHand(int index){
    int i = 0;
    Player* curr = head;

    while (curr != nullptr)
    {
        if (index == i)
        {
            return curr->checkHandCards();
        }
        curr = curr->next;
        i++;
    }

    // Index not found
    return "";
}

std::array<int,2> BlackjackGame::getPlayerIndexPos(int index){
    int i = 0;
    Player* curr = head;
    if(curr == nullptr){
        return{0,0};
    }
    while(curr != nullptr){
        if(index == i){
            return {curr->getPosX(), curr->getPosY()};
        }
        i++;
        curr = curr->next;
    }
    return {0,0};
}

String BlackjackGame::getPlayerIndexName(int index){
    int i = 0;
    Player* curr = head;

    while (curr != nullptr)
    {
        if (index == i)
        {
            return curr->getName();
        }
        curr = curr->next;
        i++;
    }

    // Index not found
    return "";
}

//AI Generated. Could not be bothered
void BlackjackGame::reversePlayers()
{
    Player* prev = nullptr;
    Player* current = head;
    Player* next = nullptr;

    while (current != nullptr) {
        next = current->next;   // save next
        current->next = prev;   // reverse link
        prev = current;         // move prev forward
        current = next;         // move current forward
    }

    head = prev; // new head
}


void BlackjackGame::gameInit(Deck& deck){
    createPlayers("Dealer");
    deckPtr = &deck;
    state = GameState::InitialState;
}

void BlackjackGame::initialStateFunction(){
    // Clear all player hands
    Player* curr = head;
    while(curr != nullptr){
        curr->clearHand();
        curr->setWonStateLoss();
        curr->setPushFalse();
        curr = curr->next;
    }
    state = GameState::WaitingForPlayers;
}

void BlackjackGame::waitingForPlayersFunction(){
    state = GameState::Dealing;
}


void BlackjackGame::update(ButtonAction action){

    switch(state){

        //Initial State is displaying all the players and such.
        case GameState::InitialState:
        initialStateFunction();
        
        break;

        //Waiting for Players to be ready: Maybe Betting Later
        case GameState::WaitingForPlayers:
            waitingForPlayersFunction();


        break;

        //Deal cards to to dealer first, then two to each player
        case GameState::Dealing:
            dealingFunction();



        break;

        //Player turn to hit or stand
        case GameState::PlayerTurn:
            playerTurnFunction(action);



        break;

        //Dealer Turn
        case GameState::DealerTurn:
            dealerTurnFunction();



        break;

        //Check who wins and such
        case GameState::Finished:
            finishedFunction();


        break;

        case GameState::Restart:
            restartFunction(action);
        
        break;
    }
}

//State Functions ----------------------------
//Deal the cards
void BlackjackGame::dealingFunction(){
    Player* curr = head;

    while(curr != nullptr){
        for(int i = 0; i < 2; i++){
            deckPtr->dealCard(*deckPtr, *curr);
        }
        curr = curr->next;
    }
    // Hide dealer's first card
    head->setFirstCardFaceDown();
    // Set current player to first player
    currentPlayer = head->next;
    // After dealing, move to player turn
    setState(GameState::PlayerTurn);
}

void BlackjackGame::playerTurnFunction(ButtonAction action){
    if(!currentPlayer) return;
    currentPlayer->setTurnTrue();
    switch(action){
        case ButtonAction::GreenTop:
            if(!currentPlayer->isBusted()){
            // Hit: deal a card to the current player
            deckPtr->dealCard(*deckPtr, *currentPlayer);
            }
            if(currentPlayer->isBusted()){
                currentPlayer->setTurnFalse();
                currentPlayer = currentPlayer->next;
                if(!currentPlayer) setState(GameState::DealerTurn);
            }
            break;
        case ButtonAction::GreenBottom:
            // Stand: move to next player
            currentPlayer->setTurnFalse();
            currentPlayer = currentPlayer->next;
            if(!currentPlayer) setState(GameState::DealerTurn);
            break;
        default:
            break;
    }
}

void BlackjackGame::dealerTurnFunction(){
    Player* dealer = head;
    dealer->revealHiddenCard();
    while(dealer->getHandVal() < 17 && !dealer->isBusted()){
        delay(1000);
        deckPtr->dealCard(*deckPtr, *dealer);
    }
    setState(GameState::Finished);
}

void BlackjackGame::finishedFunction(){
    Player* curr = head;
    if(curr == nullptr){
        state = GameState::Restart;
    }
    int dealerVal = curr->getHandVal();
    curr = curr->next;
    while(curr != nullptr){
        if(dealerVal <= 21){
                
            if(curr->getHandVal() == dealerVal){
                curr->pushResult();
            }
            if(curr->isBusted() == true || curr->getHandVal() < dealerVal){
                curr->setWonStateLoss();
            }
            if(curr->getHandVal() > dealerVal && curr->getHandVal() <= 21){
                curr->setWonStateWon();
            }
        }
        else{
            if(curr->getHandVal() <= 21){
                curr->setWonStateWon();
            }
            if(curr->isBusted() == true){
                curr->setWonStateLoss();
            }
        }
        curr = curr->next;
    
    }
    delay(1000);
    state = GameState::Restart;
}

void BlackjackGame::restartFunction(ButtonAction action){
    // Reset to initial state for a new game
    switch(action){

        case ButtonAction::GreenTop: 

        setState(GameState::InitialState);

        break;

    }
}

//User interface UI ---------------------------------------------------------

void BlackjackGame::setPlayerPosition()
{
    if (!head) return;

    // Dealer (top, centered)
    head->setPos(SIZE_X - CHAR_W * 8, SIZE_Y/2);

    if (num_of_players <= 0) return;

    // column width (float math!)
    float step = ((float)SIZE_Y / (2.0f * num_of_players));

    int i = 1;
    Player* curr = head->next;   // start at first non-dealer player
    while (curr != nullptr)
    {
        curr->setPos(0, (int)round(step * 2 * i));
        curr = curr->next;
        i++;
    }
}

bool BlackjackGame::getPlayerIndexWon(int index){
    Player* curr = head;
    int i = 0;
    while(curr != nullptr){
        if(i == index){
            return curr->getWonState();
        }
        i++;
        curr = curr->next;
    }
    return false;
}

int BlackjackGame::getPlayerIndexNumberOfCards(int index){
        Player* curr = head;
    int i = 0;
    while(curr != nullptr){
        if(i == index){
            return curr->numOfCards();
        }
        i++;
        curr = curr->next;
    }
    return false;
}

bool BlackjackGame::getPlayerIndexCurrentTurn(int index){
            Player* curr = head;
    int i = 0;
    while(curr != nullptr){
        if(i == index){
            return curr->isTurn();
        }
        i++;
        curr = curr->next;
    }
    return false;
}

int BlackjackGame::getPlayerIndexValue(int index){
                Player* curr = head;
    int i = 0;
    while(curr != nullptr){
        if(i == index){
            return curr->getHandVal();
        }
        i++;
        curr = curr->next;
    }
    return false;
}