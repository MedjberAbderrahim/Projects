#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define USER_SIZE sizeof(user)

typedef struct user{
    int uid;
    char name[10];
    int credits;
}user;

void DisplayMenu(user player){
    printf("-=[ Game of Chance Menu ]=-\n");
    printf("1 - Play the Pick a Number game\n");
    printf("2 - Play the No Match Dealer game\n");
    printf("3 - View the list of all players on this PC\n");
    printf("4 - View list of players\n");
    printf("5 - Change your user name\n");
    printf("6 - Reset your account to 100 credits\n");
    printf("7 - Quit\n");
    printf("[Name: %s]\n", player.name);
    printf("[You have %u credits] -> ", player.credits);
    
    return;
}

int PickNumber(user *player){
    
    srand(time(0));
    int target = (rand() % 20) + 1, input;
    printf("####### Pick a Number ######\nThis game costs 10 credits to play. Simply pick a number between 1 and 20, and if you pick the winning number, you will win the jackpot of 100 credits!");
    printf("\n\n10 credits have been deducted from your account.\nPick a number between 1 and 20: ");
    do{
        scanf("%d", &input);
        if(input > 20 || input <= 0)    printf("Please pick a number between 1 and 20: ");
    }while(input > 20 || input <= 0);

    if (target == input){
        player->credits += 90;
        printf("Correct answer!\n");
        return 1;
    }
    else{
        player->credits -= 10;  
        printf("Wrong answer! The correct answer was %d, good luck next time!\n", target);
        return 0;
    }
}

void NoMatchDealer(user *player){
    int bet, i, j = 0, FIND = 1;    int TAB[16];        srand(time(0));
    printf("::::::: No Match Dealer :::::::\nIn this game you can wager up to all of your credits.\n");
    printf("The dealer will deal out 16 random numbers between 0 and 99.\nIf there are no matches among them, you double your money!");
    printf(" How many of your %d credits would you like to wager? ", player->credits);
    do{
        scanf("%d", &bet);
        if(bet <= 0 || bet > player->credits)   printf("Please bet a valide amount.\n");
    }while(bet <= 0 || bet > player->credits);
    
    for(i = 0; i < 16; i++)     TAB[i] = rand() % 100;
    printf("::: Dealing out 16 random numbers :::\n");
    for(i = 0; i < 8; i++)      printf("%d\t", TAB[i]);
    printf("\n");
    for(i = 8; i < 16; i++)     printf("%d\t", TAB[i]);

    for(short i  = 0; i < 15 && FIND; i++){
        for(short j = i + 1; j < 16 && FIND; j++)        if(TAB[i] == TAB[j])        FIND = 0;
    }

    if(FIND){  printf("\nThere were no matches, you win %d credits!\n", bet);        player->credits += 2 * bet; }    
    else{   printf("\nThere were matches, you lose %d credits, good luck next time.\n", bet);     player->credits -= bet; }
}

int searchPlayer(int fd, int uid){
    
    user player;

    while(read(fd, &player, USER_SIZE ) == USER_SIZE){
        if (player.uid == uid )      return 1;
    }
    lseek(fd, 0, SEEK_SET);

    return 0;
}

void registerPlayer(int fd, user player){
    
    lseek(fd, 0, SEEK_END);
    if( write(fd, &player, USER_SIZE) == -1 ){
        fprintf(stderr, "ERROR! [REGISTER PLAYER]: Error while writing to file.\n");
        exit(-1);
    }
    lseek(fd, 0, SEEK_SET);

    return;
}

void applyChanges(int fd, int uid, int NewAmount){
    
    user player;

    lseek(fd, 0, SEEK_SET);
    while(1){
        if( read(fd, &player, USER_SIZE ) != USER_SIZE ){
            fprintf(stderr, "ERROR! [applyChanges]: User not found!\n");
            exit(-1);
        } 

        if(player.uid == uid){
            lseek(fd, -1 * USER_SIZE, SEEK_CUR);
            player.credits = NewAmount;
            if( write(fd, &player, USER_SIZE) == -1){
                fprintf(stderr, "ERROR! [applyChanges]: Error while writing to file.\n");
                exit(-1);
            }
        
            lseek(fd, 0, SEEK_SET);
            return;
        }
    }
}

void changeName(int fd, int uid, char* newName){
    lseek(fd, 0, SEEK_SET);

    user tmp;
    while( read(fd, &tmp, USER_SIZE) == USER_SIZE){
        if(tmp.uid == uid){
            lseek(fd, - USER_SIZE, SEEK_CUR);
            memcpy(&tmp.name, newName, sizeof(newName));
            if( write(fd, &tmp, USER_SIZE) != USER_SIZE){
                fprintf(stderr, "ERROR! [changeName]: Error while writing the new name to file.\n");
                exit(-1);
            }
            return;
        }
    }

    fprintf(stderr, "ERROR! [changeName]: Didn't find account.\n");
    exit(-1);
}

user getUser(int fd, int uid){
    user player;

    lseek(fd, 0, SEEK_SET);
    while(1){
        if( read(fd, &player, USER_SIZE) != USER_SIZE ){  fprintf(stderr, "ERROR! [getUser]: Error while finding the user!\n");    exit(1);    }
        
        if( player.uid == uid){      
            lseek(fd, 0, SEEK_SET);
            return player;
        }
    }
}


void PrintPlayers(int fd){
    user player;

    lseek(fd, 0, SEEK_SET);
    while(1){
        if(read(fd, &player, USER_SIZE) != USER_SIZE ){
            lseek(fd, 0, SEEK_SET);     return;
        }
        printf("%d    %s\t\t%d$\n", player.uid, player.name, player.credits);
    }
}

int main(int argc, char const *argv[])
{
    int fd, uid = getuid(), choice;     char c;     user player;

    fd = open(".GameOfChance.data", O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
    
    if( fd == -1 ){  fprintf(stderr, "ERROR! [main]: Error while opening the file!\n");    exit(1);    }


    if(searchPlayer(fd, uid) == 0){
        printf("-=-={ New Player Registration }=-=-\nEnter Your name: ");
        scanf("%s", player.name);
        player.uid = uid;
        player.credits = 100;
        registerPlayer(fd, player);
    }
    player = getUser(fd, uid);
        
    printf("Hello %s.\nYou have %d credits.", player.name, player.credits);
    do{
        DisplayMenu(player);
        scanf("%d", &choice);
        
        switch (choice)
        {
        case 1:
            if(player.credits < 10)     printf("You don't have enough credits to play this game.\n");
            else    PickNumber(&player);
            break;

        case 2:
            NoMatchDealer(&player);
            break;

        case 4:
            PrintPlayers(fd);
            break;
        
        case 5:
            char* newName = (char*)malloc(11);
            printf("Please enter new name (10 characters max): ");
            scanf("%11s", newName);                                     changeName(fd, uid, newName);
            memcpy(&player.name, newName, sizeof(newName));             free(newName);
            break;

        case 6:
            player.credits = 100;       printf("You have reseted your credits! you have 100 credits.\n");
            break;

        case 7:
            goto end;

        default:
            printf("Invalide option.\n");
            goto end;
            break;
        }

        if(choice == 1 || choice == 2 || choice == 7)      applyChanges(fd, player.uid, player.credits);
        
        printf("\nWould you like to try again?(y/n): ");
        scanf(" %1c", &c);
        
    }while(c == 'y' || c == 'Y');

    end:
        close(fd);      printf("\nSee you next time!");

    return 0;
}
