#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h> // in order to use the "rand" and "srand" functions
#include <time.h> // in order to use "time" function

#define MAX_NAME_LEGNTH 21 //Maximum possible name lenght
#define MAX_TYPE_LEGNTH 6 //Maximum name length for card types  
#define CARDS_TYPE 14 //The amount of different card types
#define FIRST_CARDS_AMOUNT 4 //The size of player's hand in the game begining
#define COLOR_OPTION 5 //The amount of different card colors 
#define UPPER_CARDS_TYPE 9 //The amount of possible card types for the upper card
#define UPPER_COLOR_OPTION 4 //The amount of possible card colors for the upper card
#define COLOR_CARD 12 //The index of COLOR card
#define RED 0 //The index of red color
#define BLUE 1 //The index of blue color
#define GREEN 2 //The index of green color
#define YELLOW 3 //The index of yellow color
#define NO_COLOR 4 //The index of no color for TAKI

//Matan Kamin 313312886 

typedef struct card
{
    char color;
    char type[MAX_TYPE_LEGNTH];
} CARD;

typedef struct Player
{
    char name[MAX_NAME_LEGNTH];
    int cardsAmount; //Will hold the logical size of the hand array
    int pySizeHand; //Will hold the physical size of the hand array
    CARD *hand; //Will hold the cards of each player 
}PLAYER;

typedef struct statistics
{
    char type[MAX_TYPE_LEGNTH];
    int count; //Will hold the frequency of cards throughout the game
}STATS;

int getRandNum(int numForRange);
CARD generateCard(STATS statistics[], int statisticsSize, bool isUpperCard);
PLAYER* initPlayersArr(PLAYER* players, int playersAmount);
void initHandArray(PLAYER* players, int playersAmount, int cardsAmount, int pySizeHand);
void initStats(STATS statistics[], int statisticsSize);
void getPlayersNames(PLAYER* players, int playersAmount);
void splitCard(PLAYER* players, int playersAmount, STATS statistics[], int statisticsSize);
void printCard(CARD card);
void printUpperCard(CARD card);
void printPlayerHand(PLAYER* player);
void throwCard(PLAYER* player, CARD* upperCard, STATS statistics[], int statisticsSize, int playerNumber);
void takeCard(PLAYER* players, STATS statistics[], int statisticsSize);
CARD chooseColor(CARD card);
bool isColorCard(CARD card);
bool isTakiCard(CARD card);
bool isSameColor(CARD card1, CARD card2);
bool isPlus(CARD card);
void overrideCard(PLAYER* player, int cardIndex);
void nextPlayerTurn(int* currentTurn, int playersAmount, bool isReverse, bool isStop);
bool isStop(CARD card);
bool isReverse(CARD card);
bool winnerCheck(PLAYER* player);
void sortStatistics(STATS statistics[], int size);
STATS* merge(STATS a1[], int size1, STATS a2[], int size2);
void copyArr(STATS dest[], STATS src[], int size);
void printStatistics(STATS* statistics, int typesAmount);
void printSpecialCStat(STATS* statistics);
void freeArrays(PLAYER* players, int playersAmount);

int main()
{
    PLAYER* players = NULL; //The players array
    int playersAmount; //The total # of players
    CARD upperCard;
    STATS statistics[CARDS_TYPE];
    int CurrentTurn = 0; //Will hold the index of the player whose turn to play
    bool isWinner = false;

    srand(time(NULL)); //For generate random numbers

    printf("************  Welcome to TAKI game !!! ***********");
    printf("\nPlease enter the number of players:\n");
    scanf("%d", &playersAmount);
    players = initPlayersArr(players, playersAmount);
    initHandArray(players, playersAmount, FIRST_CARDS_AMOUNT, FIRST_CARDS_AMOUNT);
    initStats(statistics, CARDS_TYPE);
    getPlayersNames(players, playersAmount);
    splitCard(players, playersAmount, statistics, CARDS_TYPE);
    upperCard = generateCard(statistics, CARDS_TYPE, true);

    while (!isWinner)
    {
        printUpperCard(upperCard);
        throwCard(players + CurrentTurn, &upperCard, statistics, CARDS_TYPE, CurrentTurn);
        isWinner = winnerCheck(players + CurrentTurn);
        if (!isWinner)
        {
            nextPlayerTurn(&CurrentTurn, playersAmount, isReverse(upperCard), isStop(upperCard));
        }
    }
    printf("\nThe winner is... %s! Congratulations!", (players + CurrentTurn)->name);
    sortStatistics(statistics, CARDS_TYPE);
    printStatistics(statistics, CARDS_TYPE);
    freeArrays(players, playersAmount);
}

//Returns a random number that's smaller than the given range
//efficiency: O(1)
int getRandNum(int numForRange)  
{
    int random;

    random = rand() % numForRange;

    return random;
}

//Creates a new card - return regular card or upper card (without special type) and raise the relevant counter of the card type in the statistics array by 1.
//efficiency: O(1)
CARD generateCard(STATS statistics[], int statisticsSize, bool isUpperCard) 
{
    char cardsTypes[CARDS_TYPE][MAX_TYPE_LEGNTH] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "+", "STOP", "<->", "COLOR", "TAKI" };
    char cardsColor[COLOR_OPTION] = { 'R', 'B','G', 'Y', ' ' };
    int randColor, randType;
    CARD card;

    if (isUpperCard)
    {
        randType = getRandNum(UPPER_CARDS_TYPE);
        randColor = getRandNum(UPPER_COLOR_OPTION);
    }
    else 
    {
        randType = getRandNum(CARDS_TYPE);
        randColor = getRandNum(COLOR_OPTION);
    }

    if (randType == COLOR_CARD)
    {
        card.color = cardsColor[NO_COLOR];
        strcpy(card.type, cardsTypes[COLOR_CARD]);
        statistics[COLOR_CARD].count += 1;
    }
    else
    {
        randColor = getRandNum(COLOR_OPTION - 1);
        card.color = cardsColor[getRandNum(COLOR_OPTION - 1)];
        strcpy(card.type, cardsTypes[randType]);
        statistics[randType].count += 1;
    }
    return card;
}

//Creates the players array by a given number of players - return players array with the wanted pysical size
//efficiency: O(n)
PLAYER* initPlayersArr(PLAYER* players, int playersAmount) //Allocate memory for "Players" array
{
    players = (PLAYER*)malloc(playersAmount * sizeof(PLAYER));

    if (players != NULL)
        return players;
    else
    {
        printf("Memory allocation failed!!!\n");
        exit(1);
    }
}

//Creates the hands array of each player in player array by a given number of players - return hand array for each player with the wanted physical size
//efficiency: O(n)
void initHandArray(PLAYER* players, int playersAmount, int cardsAmount, int pySizeHand)//Allocate memory for "hand" array
{
    int i;

    for (i = 0; i < playersAmount;i++)
    {
        players[i].cardsAmount = cardsAmount;
        players[i].pySizeHand = pySizeHand;

        players[i].hand = (CARD*)malloc((players[i].pySizeHand) * sizeof(CARD));
        if (players[i].hand == NULL)
        {
            printf("Memory allocation failed!!!\n");
            exit(1);
        }
    }
}

//Fills the statistics array with the cards types. Each one of the different types has different counter. 
//efficiency: O(n)
void initStats(STATS statistics[], int statisticsSize)
{
    int i;
    char cardsTypes[CARDS_TYPE][MAX_TYPE_LEGNTH] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "+", "STOP", "<->", "COLOR", "TAKI" };

    for (i = 0; i < statisticsSize; i++)
    {
        strcpy(statistics[i].type, cardsTypes[i]);
        statistics[i].count = 0;
    }
}

//Fills the names of the players inside the players array. Using the players amount to fill each player in the relevant place. 
//efficiency: O(n)
void getPlayersNames(PLAYER* players, int playersAmount)
{
    int i;

    for (i = 0; i < playersAmount; i++)
    {
        printf("Please enter the first name of player #%d:\n", i + 1);
        scanf("%s", players[i].name);
    }
}

//Recieves and print CARD object. 
//efficiency: O(1)
void printCard(CARD card)
{
    int typelength = (int)strlen(card.type);

    printf("\n*********");
    printf("\n*       *");
    switch (typelength)
    {
    case 1:
        printf("\n*   %s   *", card.type);
        break;
    case 3:
        printf("\n*  %s  *", card.type);
        break;
    case 4:
        printf("\n*  %s *", card.type);
        break;
    case 5:
        printf("\n* %s *", card.type);
        break;
    }
    printf("\n*   %c   *", card.color);
    printf("\n*       *");
    printf("\n*********\n\n");
}

//Assignees cards for each one of the players at the game beginning. 
//efficiency: O(n**2)
void splitCard(PLAYER* players, int playersAmount, STATS statistics[], int statisticsSize)
{
    int i, j;

    for (i = 0; i < playersAmount; i++)
    {
        for (j = 0; j < players[i].cardsAmount; j++)
        {
            players[i].hand[j] = generateCard(statistics, CARDS_TYPE, false);
        }
    }
}

//Prints the upper card. 
//efficiency: O(1)
void printUpperCard(CARD card)
{
    printf("\nUpper card:");
    printCard(card);
}

//Recives player object and prints all the card that he has. 
//efficiency: O(n)
void printPlayerHand(PLAYER* player)
{
    int i;

    printf("\n%s's turn:\n\n", player->name);

    for (i = 0; i < player->cardsAmount; i++)
    {
        printf("Card #%d:", i + 1);
        printCard(player->hand[i]);
    }
}

/*The game body:
1. Prints the relevant player hand by the playerNumber as index.
2. Asks for the player wanted move - take a card or throw one.
3. Validates if the chosen card is ok to throw.*/
//efficiency: O(n)
void throwCard(PLAYER* player, CARD* upperCard, STATS statistics[], int statisticsSize, int playerNumber)
{
    int cardIndex;
    bool invalidCard = true;

    printPlayerHand(player);
    printf("\nPlease enter 0 if you want to take a card from the deck\nor 1 - %d if you want to put one of your cards in the middle :\n", player->cardsAmount);
    scanf("%d", &cardIndex);

    while (invalidCard)
    {
        if (cardIndex == 0)
        {
            takeCard(player, statistics, CARDS_TYPE);
            invalidCard = false;
        }
        else if (isColorCard(player->hand[cardIndex-1]))
        {
            *upperCard = chooseColor(player->hand[cardIndex-1]);
            overrideCard(player, cardIndex-1);
            player->cardsAmount--;
            invalidCard = false;
            if (winnerCheck(player))
                return;
        }
        else if (isTakiCard(player->hand[cardIndex-1]) && isSameColor(player->hand[cardIndex - 1], *upperCard))
        {
            *upperCard = player->hand[cardIndex-1];
            overrideCard(player, cardIndex-1);
            player->cardsAmount--;
            if (winnerCheck(player))
                return;
            throwCard(player, upperCard, statistics, statisticsSize, playerNumber);
            invalidCard = false;
        }
        else if (isPlus(player->hand[cardIndex - 1]) && isSameColor(player->hand[cardIndex - 1], *upperCard))
        {
            *upperCard = player->hand[cardIndex - 1];
            overrideCard(player, cardIndex - 1);
            player->cardsAmount--;
            if (player->cardsAmount == 0)
            {
                takeCard(player, statistics, CARDS_TYPE);
            }
            throwCard(player, upperCard, statistics, statisticsSize, playerNumber);
            invalidCard = false;
            if (winnerCheck(player))
                return;
        }
        else if (isSameColor(player->hand[cardIndex - 1], *upperCard))
        {
            *upperCard = player->hand[cardIndex-1];
            overrideCard(player, cardIndex-1);
            player->cardsAmount--;
            invalidCard = false;
            if (winnerCheck(player))
                return;
        }
        else
        {
            printf("\nInvalid card! Try again.");
            printf("\nPlease enter 0 if you want to take a card from the deck\nor 1 - %d if you want to put one of your cards in the middle :\n", player->cardsAmount);
            scanf("%d", &cardIndex);
        }
    }
}

//Adds a new card to the player's hand array. If the player's hand doesn't have enough place for a new card, it will add more memory to the player's hand array.
//efficiency: O(n)
void takeCard(PLAYER* players, STATS statistics[], int statisticsSize)
{
    CARD* temp;
    int i;


    if (players->pySizeHand == players->cardsAmount)
        {
            temp = (CARD*)malloc(((players->pySizeHand) * 2) * sizeof(CARD));
            if (temp == NULL)
            {
                printf("Memory allocation failed!!!\n");
                exit(1);
            }
            else
            {
                players->pySizeHand *= 2;
                for (i = 0; i < players->cardsAmount && i < players->pySizeHand; i++)
                {
                    temp[i] = players->hand[i];
                }
                free(players->hand);
                players->hand = temp;
            }

        }
    players->hand[players->cardsAmount] = generateCard(statistics, CARDS_TYPE, false);
    players->cardsAmount += 1;
}

//Sets the chosen color to a TAKI card.
//efficiency: O(1)
CARD chooseColor(CARD card)
{
    int wantedColor;


    printf("\nPlease enter your color choice:\n");
    printf("1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
    scanf("%d", &wantedColor);

    switch (wantedColor)
    {
    case 1:
        card.color = 'Y';
        break;
    case 2:
        card.color = 'R';
        break;
    case 3:
        card.color = 'B';
        break;
    case 4:
        card.color = 'G';
        break;
    }
    return card;
}

//Returns T/F if a card is a COLOR card type.
//efficiency: O(1)
bool isColorCard(CARD card)
{
    if (card.color == ' ')
        return true;
    else
    {
        return false;
    }
}

//Returns T/F if a card is a TAKI card type.
//efficiency: O(1)
bool isTakiCard(CARD card)
{
    if (strcmp(card.type,"TAKI") == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//Returns T/F if two cards have the same color.
//efficiency: O(1)
bool isSameColor(CARD card1, CARD card2)
{
    if (card1.color == card2.color)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//Returns T/F if a card is a + card type.
//efficiency: O(1)
bool isPlus(CARD card)
{
    if (strcmp(card.type, "+") == 0)
        return true;
    else
        return false;
}

//Returns T/F if a card is a STOP card type.
//efficiency: O(1)
bool isStop(CARD card)
{
    if (strcmp(card.type, "STOP") == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//Returns T/F if a card is a REVERSE card type.
//efficiency: O(1)
bool isReverse(CARD card)
{
    if (strcmp(card.type, "<->") == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//Removes the card that the player throw from his hand array.
//efficiency: O(n)
void overrideCard(PLAYER* player, int cardIndex)
{
    int i;

    for (i = cardIndex; i < (player->cardsAmount - 1); i++)
    {
        player->hand[i] = player->hand[i + 1];
    }
}

//Controls the players' turn to play. If a player throws REVERSE card or a STOP card it will handle the changing in the turns order.
//efficiency: O(n)
void nextPlayerTurn(int* currentTurn, int playersAmount, bool isReverse, bool isStop)
{

    if (isReverse) {
        if (*currentTurn >= 1) {
            *currentTurn -= 1;
        }
        else {
            *currentTurn = playersAmount - 1;
        }
    }
    else if (isStop)
    {
        if (*currentTurn < playersAmount - 1) {
            *currentTurn += 2;
        }
        else {
            *currentTurn = 1;
        }

    }
    else {
        if (*currentTurn < playersAmount - 1) {
            *currentTurn += 1;
        }
        else {
            *currentTurn = 0;
        }
    }

}

//Returns T/F if a player finishes his cards.
//efficiency: O(n)
bool winnerCheck(PLAYER *player)
{
    if (player->cardsAmount == 0)
    {
        return true;
    }
    else
        return false;
}

//Sorts the statistics array by the most common cards throughout the game to the least common.
//efficiency: O(nlogn)
void sortStatistics(STATS statistics[], int size)
{
    STATS* tmpArr = NULL;
    if (size <= 1)
        return;
    sortStatistics(statistics, size / 2);
    sortStatistics(statistics + size / 2, size - size / 2);
    tmpArr = merge(statistics, size / 2, statistics + size / 2, size - size / 2);
    if (tmpArr)
    {
        copyArr(statistics, tmpArr, size);
        free(tmpArr);
    }
    else
    {
        printf("Memory allocation faliure!!!!\n");
        exit(1);
    }
}

//Merges two array by the most common cards throughout the game to the least common.
//efficiency: O(n)
STATS* merge(STATS a1[], int size1, STATS a2[], int size2)
{
    int ind1, ind2, resInd;
    STATS* res = (STATS*)malloc((size1 + size2) * (sizeof(STATS)));
    if (res)
    {
        ind1 = ind2 = resInd = 0;
        while ((ind1 < size1) && (ind2 < size2))
        {
            if (a1[ind1].count >= a2[ind2].count)
            {
                res[resInd] = a1[ind1];
                ind1++;
            }
            else
            {
                res[resInd] = a2[ind2];
                ind2++;
            }
            resInd++;
        }
        while (ind1 < size1)
        {
            res[resInd] = a1[ind1];
            ind1++;
            resInd++;
        }
        while (ind2 < size2)
        {
            res[resInd] = a2[ind2];
            ind2++;
            resInd++;
        }
    }
    return res;
}

//Coppies the data from src array to dest array.
//efficiency: O(n)
void copyArr(STATS dest[], STATS src[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        dest[i] = src[i];
}

//Prints the game statistics - by the frequency of cards throughout the game.
//efficiency: O(n)
void printStatistics(STATS* statistics, int typesAmount) {

    printf("\n************ Game Statistics ************ \n"
        "Card # | Frequency\n"
        "__________________\n");

    for (int i = 0; i < typesAmount; i++) {
        if ((int)strlen(((statistics + i)->type)) == 1) {
            printf("   %s   |    %d \n", statistics[i].type, statistics[i].count);
        }
        else {
            printSpecialCStat(statistics + i);
        }
    }
}

//Prints the special cards statistics.
//efficiency: O(1)
void printSpecialCStat(STATS* statistics) {
    if (strcmp(((statistics)->type), "STOP") == 0)
        printf(" STOP  |    %d  \n", (statistics)->count);
    else if (strcmp(((statistics)->type), "<->") == 0)
        printf("  <->  |    %d  \n", (statistics)->count);
    else if (strcmp(((statistics)->type), "COLOR") == 0)
        printf(" COLOR |    %d  \n", (statistics)->count);
    else if (strcmp(((statistics)->type), "TAKI") == 0)
        printf(" TAKI  |    %d  \n", (statistics)->count);
}

//Releses the memory of the players and their hands arrays.
//efficiency: O(n)
void freeArrays(PLAYER* players, int playersAmount)
{
    int i;

    for (i = 0; i < playersAmount; i++)
    {
        free((players + i)->hand);
    }
    free(players);
}
