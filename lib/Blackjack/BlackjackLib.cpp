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

// HAND ---------------------------------------------------
Hand::Hand() {
    head = nullptr;
    cardCount = 0;
    handValue = 0;
    aceCount = 0;
}

Hand::~Hand() {
    clear();
}

int Hand::numOfCards() {
    return cardCount;
}

void Hand::addCard(String value, String suitAssign) {
    cardCount++;

    Card* node = new Card(value, suitAssign);
    node->next = head;
    head = node;

    int v = head->getVal();
    handValue += v;

    if (value == "A") {
        aceCount++;
    }
}

String Hand::checkCards() {
    String out = "";
    Card* curr = head;

    while (curr != nullptr) {
        if (out.length() > 0) out += "|";
        out += curr->getType();
        curr = curr->next;
    }

    return out;
}

int Hand::getValue() {
    int val = handValue;
    int aces = aceCount;
    while (val > 21 && aces > 0) {
        val -= 10;
        aces--;
    }
    return val;
}

boolean Hand::isBusted() {
    return getValue() > 21;
}

void Hand::clear() {
    while (head != nullptr) {
        Card* tmp = head;
        head = head->next;
        delete tmp;
    }
    head = nullptr;
    cardCount = 0;
    handValue = 0;
    aceCount = 0;
}

void Hand::revealHiddenCard() {
    Card* curr = head;
    while (curr != nullptr) {
        if (!curr->getFaceDir()) {  // if face down
            curr->revealCard();
            break;  // assuming only one hidden card
        }
        curr = curr->next;
    }
}

void Hand::setLastCardFaceDown() {
    Card* curr = head;
    if (curr != nullptr) {
        while (curr->next != nullptr) {
            curr = curr->next;
        }
        curr->setFaceDown();
    }
}

void Hand::setFirstCardFaceDown() {
    Card* curr = head;
    if (curr != nullptr) {
        while (curr->next != nullptr) {
            curr = curr->next;
        }
        curr->setFaceDown();
    }
}

bool Hand::canSplit() {
    return cardCount == 2 && head != nullptr && head->next != nullptr && 
           head->getVal() == head->next->getVal();
}

Hand* Hand::split() {
    if (!canSplit()) return nullptr;
    
    // Create new hand with second card
    Hand* newHand = new Hand();
    Card* secondCard = head->next;
    head->next = nullptr; // Remove second card from this hand
    
    // Move second card to new hand
    newHand->head = secondCard;
    newHand->cardCount = 1;
    newHand->handValue = secondCard->getVal();
    if (secondCard->getType() == "A") {
        newHand->aceCount = 1;
    }
    
    // Update this hand
    cardCount = 1;
    handValue = head->getVal();
    aceCount = (head->getType() == "A") ? 1 : 0;
    
    return newHand;
}

// PLAYER ---------------------------------------------------
Player::Player(String name) {
    this->name = name;
    next = nullptr;
    points = 20;
    posX = 0;
    posY = 0;
    activeHandIndex = 0;
    wonCase = false;
    push = false;
    isTurnNow = false;
    
    // Create initial hand
    hands.push_back(new Hand());
}

Player::~Player() {
    clearHands();
}

int Player::getNumHands() {
    return hands.size();
}

Hand* Player::getHand(int index) {
    if (index >= 0 && index < hands.size()) {
        return hands[index];
    }
    return nullptr;
}

void Player::addHand(Hand* hand) {
    if (hand != nullptr) {
        hands.push_back(hand);
    }
}

void Player::splitHand(int handIndex) {
    if (handIndex >= 0 && handIndex < hands.size()) {
        Hand* originalHand = hands[handIndex];
        if (originalHand->canSplit()) {
            Hand* newHand = originalHand->split();
            if (newHand != nullptr) {
                hands.insert(hands.begin() + handIndex + 1, newHand);
            }
        }
    }
}

void Player::clearHands() {
    for (Hand* hand : hands) {
        delete hand;
    }
    hands.clear();
    hands.push_back(new Hand()); // Always have at least one hand
    activeHandIndex = 0;
}

bool Player::hasActiveHand() {
    return activeHandIndex < hands.size() && !hands[activeHandIndex]->isBusted();
}

int Player::getActiveHandIndex() {
    return activeHandIndex;
}

void Player::setActiveHandIndex(int index) {
    if (index >= 0 && index < hands.size()) {
        activeHandIndex = index;
    }
}

int Player::numOfCards(){
    Hand* activeHand = getHand(activeHandIndex);
    return activeHand ? activeHand->numOfCards() : 0;
}

void Player::addToHand(String value, String suitAssign) {
    Hand* activeHand = getHand(activeHandIndex);
    if (activeHand) {
        activeHand->addCard(value, suitAssign);
    }
}

String Player::checkHandCards() {
    Hand* activeHand = getHand(activeHandIndex);
    return activeHand ? activeHand->checkCards() : "";
}

int Player::getHandVal(){
    Hand* activeHand = getHand(activeHandIndex);
    return activeHand ? activeHand->getValue() : 0;
}

boolean Player::checkSplit(){
    Hand* activeHand = getHand(activeHandIndex);
    return activeHand ? activeHand->canSplit() : false;
}

void Player::clearHand() {
    clearHands();
}

boolean Player::isBusted(){
    Hand* activeHand = getHand(activeHandIndex);
    return activeHand ? activeHand->isBusted() : true;
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
    Hand* activeHand = getHand(activeHandIndex);
    if (activeHand) {
        activeHand->revealHiddenCard();
    }
}

void Player::setLastCardFaceDown() {
    Hand* activeHand = getHand(activeHandIndex);
    if (activeHand) {
        activeHand->setLastCardFaceDown();
    }
}

void Player::setFirstCardFaceDown() {
    Hand* activeHand = getHand(activeHandIndex);
    if (activeHand) {
        activeHand->setFirstCardFaceDown();
    }
}

bool Player::getWonState(){
    return wonCase;
}

void Player::setWonStateWon(){
    wonCase = true;
    points = points + 2 * bet;
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

void Player::increaseBet(){
    if(points <= 0){
        return;
    }
    bet++;
    points--;
}

void Player::decreaseBet(){
    if(bet <= 0){
        return;
    }

    bet--;
    points++;
}

void Player::betWin(){
    points = points + (2 * points);
}

int Player::getBet(){
    return bet;
}

void Player::resetBet(){
    bet = 0;
}

int Player::getPoints(){
    return points;
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



void BlackjackGame::createPlayers(String name)
{
    Player* node = new Player(name);
    num_of_players++;

    // Om listan är tom → gör spelaren till head
    if (head == nullptr) {
        head = node;
        return;
    }

    // Annars gå till sista noden
    Player* curr = head;
    while (curr->next != nullptr) {
        curr = curr->next;
    }

    // Lägg in sist
    curr->next = node;
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
            String result = "";
            int originalActive = curr->getActiveHandIndex();
            for(int handIndex = 0; handIndex < curr->getNumHands(); handIndex++){
                curr->setActiveHandIndex(handIndex);
                if(handIndex > 0) result += " | ";
                if(handIndex == originalActive) result += "-";
                result += curr->checkHandCards();
                if(handIndex == originalActive) result += "-";
            }
            curr->setActiveHandIndex(originalActive); // restore
            return result;
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
        curr->resetBet();
        curr = curr->next;
    }
    currentBettingPlayer = head;
    state = GameState::WaitingForPlayers;
}

void BlackjackGame::waitingForPlayersFunction(ButtonAction action){
    if(currentBettingPlayer->getName() == "Dealer"){
        currentBettingPlayer = currentBettingPlayer->next;
        currentBettingPlayer->setTurnTrue();
    }
    if(!currentBettingPlayer) return;
    switch(action){
        case ButtonAction::Left:
            currentBettingPlayer->increaseBet();
 
            break;
        case ButtonAction::Right:
            currentBettingPlayer->decreaseBet();

            break;
        case ButtonAction::GreenBottom:
            currentBettingPlayer->setTurnFalse();
            currentBettingPlayer = currentBettingPlayer->next;
            currentBettingPlayer->setTurnTrue();
            if(currentBettingPlayer && currentBettingPlayer->getName() == "Dealer"){
                currentBettingPlayer = nullptr;
            }

            if(!currentBettingPlayer){
                // Förr: state = GameState::Dealing;
                if (countActiveNonDealerPlayers() == 0) {
                    // Ingen spelare har betat – stanna kvar
                    // (valfritt: sätt ett UI-meddelande)
                    // message = "Need players!";
                    setState(GameState::InitialState);
                    return;
                }
                state = GameState::Dealing;
                dealingStep = 0; // bra att nollställa
            }
    break;

            break;
        default:
            break;
        case ButtonAction::Down:

            createPlayers("P" + String(num_of_players));
        
        break;

        case ButtonAction::Up:
            removeLastPlayer();
        break;
    }
}


void BlackjackGame::update(ButtonAction action, unsigned long currentMillis){

    switch(state){

        //Initial State is displaying all the players and such.
        case GameState::InitialState:
        initialStateFunction();
        
        break;

        //Waiting for Players to be ready: Maybe Betting Later
        case GameState::WaitingForPlayers:
            waitingForPlayersFunction(action);


        break;

        //Deal cards to to dealer first, then two to each player
        case GameState::Dealing:
            dealingFunction(currentMillis);



        break;

        //Player turn to hit or stand
        case GameState::PlayerTurn:
            playerTurnFunction(action);



        break;

        //Dealer Turn
        case GameState::DealerTurn:
            dealerTurnFunction(currentMillis);



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

//AI rewrote such that it updates in and reveals the cards once every time interval we'd like. I just could not be bothered to be honest.
void BlackjackGame::dealingFunction(unsigned long currentMillis){
    const int maxPlayers = 10;
    if (dealingStep == 0) {
        // Initialize dealing
        dealingStep = 1;
        lastDealTime = currentMillis;
        // Populate active players
        numActivePlayers = 0;
        Player* curr = head;

        while (curr && numActivePlayers < maxPlayers) {
        if (curr == head || curr->getBet() > 0) {
            activePlayers[numActivePlayers++] = curr;
        }
        curr = curr->next;
    }

    }
    if(numActivePlayers == 0){
        setState(GameState::WaitingForPlayers);
        dealingStep = 0;
        return;
    }

    int totalCards = numActivePlayers * 2;
    if (dealingStep > totalCards) {
        // All cards dealt
        // Set current player to first active player after dealer
        currentPlayer = (numActivePlayers > 1) ? activePlayers[1] : nullptr;
        // After dealing, move to player turn
        setState(GameState::PlayerTurn);
        dealingStep = 0;  // Reset for next game
        return;
    }

    if (currentMillis - lastDealTime >= 1000) {  // 1 second interval
        int playerIndex = (dealingStep - 1) / 2;
        if(playerIndex < numActivePlayers){
            Player* curr = activePlayers[playerIndex];
            // Deal the next card
            deckPtr->dealCard(*deckPtr, *curr);
            // If it's the dealer's first card, hide it
            if (playerIndex == 0 && (dealingStep - 1) % 2 == 0) {
                curr->setLastCardFaceDown();
            }
        }
        dealingStep++;
        lastDealTime = currentMillis;
    }
}

void BlackjackGame::playerTurnFunction(ButtonAction action){
    if(!currentPlayer) return;
    currentPlayer->setTurnTrue();
    
    // Check if current hand can be split
    bool canSplit = currentPlayer->checkSplit();
    
    switch(action){
        case ButtonAction::GreenTop:
            if(!currentPlayer->isBusted()){
            // Hit: deal a card to the current player
            deckPtr->dealCard(*deckPtr, *currentPlayer);
            }
            if(currentPlayer->isBusted()){
                // Move to next hand or next player
                moveToNextHandOrPlayer();
            }
            break;
        case ButtonAction::GreenBottom:
            // Stand: move to next hand or next player
            moveToNextHandOrPlayer();
            break;
        case ButtonAction::Up:
        case ButtonAction::Split:
            if(canSplit){
                currentPlayer->splitHand(currentPlayer->getActiveHandIndex());
                // Hands now have one card each, player can play them separately
            }
            break;
        default:
            break;
    }
}

void BlackjackGame::moveToNextHandOrPlayer() {
    // Try to move to next hand of current player
    int nextHandIndex = currentPlayer->getActiveHandIndex() + 1;
    if (nextHandIndex < currentPlayer->getNumHands()) {
        currentPlayer->setActiveHandIndex(nextHandIndex);
        return;
    }
    
    // No more hands for this player, move to next player
    currentPlayer->setTurnFalse();
    currentPlayer = currentPlayer->next;
    // Skip players with bet == 0
    while(currentPlayer && currentPlayer != head && currentPlayer->getBet() == 0){
        currentPlayer = currentPlayer->next;
    }
    if (!currentPlayer || currentPlayer == head) {
        setState(GameState::DealerTurn);
    } else {
        // Reset to first hand of new player
        currentPlayer->setActiveHandIndex(0);
    }
}

//AI rewrote it such that it uses the Millis function such that the reveal is done more nice.
void BlackjackGame::dealerTurnFunction(unsigned long currentMillis){
    Player* dealer = head;
    if (dealerTurnStep == 0) {
        // Reveal hidden card
        dealer->revealHiddenCard();
        dealerTurnStep = 1;
        lastDealTime = currentMillis;
        return;
    }

    if (dealer->getHandVal() >= 17 || dealer->isBusted()) {
        // Done
        setState(GameState::Finished);
        dealerTurnStep = 0;
        return;
    }

    if (currentMillis - lastDealTime >= 1000) {
        // Deal one card
        deckPtr->dealCard(*deckPtr, *dealer);
        lastDealTime = currentMillis;
    }
}

void BlackjackGame::finishedFunction(){
    Player* dealer = head;
    if(dealer == nullptr){
        state = GameState::Restart;
        return;
    }
    
    // Dealer has one hand (typically)
    int dealerVal = dealer->getHandVal();
    
    // Check each player's hands
    Player* curr = dealer->next;
    while(curr != nullptr){
        // Check each hand of the current player
        bool hasWinningHand = false;
        bool allHandsBusted = true;
        
        for(int handIndex = 0; handIndex < curr->getNumHands(); handIndex++){
            curr->setActiveHandIndex(handIndex);
            int handVal = curr->getHandVal();
            bool handBusted = curr->isBusted();
            
            if(!handBusted){
                allHandsBusted = false;
                if(dealerVal <= 21){
                    if(handVal > dealerVal && handVal <= 21){
                        hasWinningHand = true;
                    }
                } else {
                    hasWinningHand = true;
                }
            }
        }
        
        // Set player result based on hands
        if(hasWinningHand){
            curr->setWonStateWon();
        } else if(allHandsBusted || (dealerVal <= 21 && !hasWinningHand)){
            curr->setWonStateLoss();
        } else {
            curr->pushResult(); // tie
        }
        
        curr = curr->next;
    }
    
    delay(1000);
    state = GameState::Restart;
}

void BlackjackGame::restartFunction(ButtonAction action){
    Player* curr = head;
    // Reset to initial state for a new game
    switch(action){
        while(curr != nullptr){
            curr->resetBet();
            curr = curr->next;
        }

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
            if(curr->getNumHands() > 0){
                return curr->numOfCards();
            }
            return 0;
        }
        i++;
        curr = curr->next;
    }
    return 0;
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

int BlackjackGame::getPlayerIndexBet(int index){
    Player* curr = head;
    int i = 0;
    while(curr != nullptr){
        if(i == index){
            return curr->getBet();
        }
        i++;
        curr = curr->next;
    }
    return 0;
}

int BlackjackGame::getPlayerIndexValue(int index){
    Player* curr = head;
    int i = 0;
    while(curr != nullptr){
        if(i == index){
            if(curr->getNumHands() > 0){
                return curr->getHandVal();
            }
            return 0;
        }
        i++;
        curr = curr->next;
    }
    return 0;
}

int BlackjackGame::getPlayerIndexPoints(int index){
            Player* curr = head;
    int i = 0;
    while(curr != nullptr){
        if(i == index){
            return curr->getPoints();
        }
        i++;
        curr = curr->next;
    }
    return false;
}

int BlackjackGame::getNumPlayers(){
    return num_of_players;
}


void BlackjackGame::removeLastPlayer()
{
    // Om listan är tom eller bara består av dealern → gör ingenting
    if (head == nullptr || head->next == nullptr) {
        return;
    }

    Player* curr = head;

    // Gå fram tills curr->next är sista spelaren
    while (curr->next->next != nullptr) {
        curr = curr->next;
    }

    // curr->next är sista spelaren → ta bort den
    Player* last = curr->next;
    curr->next = nullptr;
    delete last;
    num_of_players--;
}


String BlackjackGame::getMessage() {

    switch(state){

        case GameState::InitialState:
            return "Lesgo";

        case GameState::WaitingForPlayers:
            return "Middle";

        case GameState::Dealing:
            return "Dealing";

        case GameState::PlayerTurn:
            return "Player Turn";

        case GameState::DealerTurn:
            return "Dealer Turn";

        case GameState::Finished:
            return "Press Green Top";

        case GameState::Restart:
            return "Restarting...";
    }

    // Om någon mot förmodan hamnar här
    return "";
}



int BlackjackGame::countActiveNonDealerPlayers() const {
    int count = 0;
    Player* curr = head ? head->next : nullptr; // hoppa över dealer
    while (curr) {
        if (curr->getBet() > 0) count++;
        curr = curr->next;
    }
    return count;
}
