#include <stdio.h>
#include <unistd.h> //Sleep, write, close
#include <fcntl.h> //open
#include <stdlib.h>
#include <string.h> //strlen
#include <time.h>


#define DEVICE_FILE "/dev/leds"



void counter(int file);
int leds(int file, char *buf);
int menu();
void guess_sequence(int file);



int main(){

	int final = 0;
	int option;

	int file = open(DEVICE_FILE, O_RDWR);

	if(file < 0) {
		printf("The file could not be opened\n");
		return 1;
	}

	while (final == 0) {

		option = menu();

		//Turn off  leds
		if((leds(file, "")) != 0) {
			printf("There was a problem turning off the LEDs\n");
		}


		if (option==0) final = 1;

		else if (option == 1) guess_sequence(file);

		else if (option == 2) counter(file);			
		
		// Turn off  leds
		if((leds(file, "")) != 0) {
			printf("There was a problem turning off the LEDs\n");
		}
	}


	if (close(file) < 0) {
		printf("Could not close file\n");
		return 1;
	} 

	return 0;
}


void counter(int file) {
	char* buff;
	int i;
	
	for (i=0; i <= 7; i++) {
		
		if (i==0) buff = "0";
		else if (i==1) buff = "3"; 
		else if (i==2) buff = "2";
		else if (i==3) buff = "23";
		else if (i==4) buff = "1";
		else if (i==5) buff = "13";
		else if (i==6) buff = "12";
		else if (i==7) buff = "123";
		 
		if((leds(file, buff)) != 0) {
			printf("There was a problem lighting the LEDs\n");
		}
		
		//Delay time
		usleep(1000000);
	}
}




void guess_sequence(int file){

	int choice = 0;
	int level;
	int numOfLeds = 3; //With each success the number of LEDs to be displayed is increased by 1
	int correct = 1; //If value of correct is 0, the execution is terminated because the user has failed
	printf("\nTry to guess the sequence shown by keyboard.\n");

	printf("\nChoose a level: \n - Level 1: 0.5 seconds between each LED \n - Level 2: 0.25 seconds between each LED \n - Level 3: 0.125 seconds between each LED");

	while (choice == 0) {

		printf("\nLevel: ");
		scanf("%d", &level);

		if (level >= 1 && level <= 3)
			choice = 1;
		else
			printf("\nThat level is invalid, choose again\n");
	}

	int delayLeds;
	if(level == 1) delayLeds = 500000; //Half a second
	else if(level == 2) delayLeds = 250000;
	else delayLeds = 125000;


				
	while(correct == 1){
		char in[numOfLeds];

		int i;
		printf("\nGenerating sequence \n");
		srand(time(NULL)); //Generating random value
		for(i = 0; i < numOfLeds; i++){

			in[i] = (char)((rand()%3+1) + '0');//It generates random numbers from 1 to 3 and converts them into char
			//printf("%c", in[i]);
		}
					
		printf("\nThere it goes! Pay attention to the keyboard\n");
		usleep(4000000); //Espera 4 segundos


		//Show LEDs on keyboard
		for(i = 0; i < numOfLeds; i++){
			usleep(delayLeds);
			write(file, &in[i], 1);
			usleep(delayLeds);
			write(file, "", 1);
		}

		leds(file, "");

		//Check LEDs
		i = 0;
		printf("\nNow type in the LEDs in the order in which you have seen it\n");
		printf("\nNumLock = 1, CapsLock = 2, ScrollLock = 3\n");

		int leido;
		while(i < numOfLeds && correct == 1){
			printf("Led %d: ", i+1);
			scanf("%d", &leido);

			if((char)(leido + '0') != in[i]){
				printf("\nYOU HAVE LOST!\n");
				correct=0;

				//The LEDs begin to flash indicating stage is over
				int j = 0;
				char* bufferString = "123";
				while(j < 8){
					leds(file, bufferString);
					usleep(500000);
					if(j%2 == 0) bufferString = "";
					else bufferString = "123";
					j++;
				}
			}

			i++;
		}

		//You have won this round
		if(correct == 1){
			printf("\nYOU'VE WON!\n");
			printf("\nNext stage! Number of LEDs increased by 1\n");
			numOfLeds++;
		}
	}

	char* bufferString = "";
	leds(file, bufferString);
}

int leds(int file, char *buf) {

	int len = strlen(buf);
    
    //checks the number of bytes written
	if(write(file, buf, len) < 0) {
		printf("Cannot write to file\n");
		return -1;
	}

	return 0;
}



int menu() {
	int option = -1;
	int exit_flag = 0;

	printf("\n\n Options :\n");
	printf("----------------------------\n");
	printf(" 1.- Memorize sequence\n");
	printf(" 2.- Binary counter\n"); // Chooses one of the previous modes randomly
	printf(" 0.- Exit\n");

	while (!exit_flag) {
		printf("Choose an option: ");

		scanf("%d", &option);
		if (option >= 0 && option <= 2)
			exit_flag = 1;
		else
			printf("Invalid option! Choose another\n");
	}

	return option;
}


