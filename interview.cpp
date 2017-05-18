#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
//#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>


 static sem_t guest_sem[100];
 static sem_t mutex;
 pthread_mutex_t count_mutex;
 int cooked=0;
 int howMuchToCook=0;
 int NUM_GUESTS=4;
 int AVAILBLE=3;
 int ready=0;
 bool finished[100];
 bool waitingList[100];
 bool isInputsjf=false;
 bool isInputpri=false;
 bool isInputfcfs=false;
 int sortedList[100]={-1};
 int thrCount=0;

/* create thread argument struct for thr_func() */
typedef struct _guest_data_t {
    int guest_id;
    int numOfChickenOrdered;
    int timeRequiredToEat;
    int priority;
} guest_data;

//compare fun for sorting
int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}
bool arrayAlreadyHas(int m){
	int i=0;
	for(i=0;i<NUM_GUESTS;i++){
		if(sortedList[i]==m){
			return true;
		}
	}
return false;
}
void *chef(void *arg) {
    bool done=false;
    while (!done) {
        sem_wait(&mutex);
	printf("\nChef cooks one tandoori chicken.[%d/%d]",++ready,AVAILBLE);
        cooked++;
	sleep(1);
        if(cooked>=howMuchToCook){
            done=true;
        }
    }
}

int getNextGuestId(int curGuest){

	int nextGuest=(curGuest+1)%NUM_GUESTS;

	while(true)
	{
		if(finished[nextGuest]){
		nextGuest = (nextGuest+1)%NUM_GUESTS;
		} else {
			break;
		}
	}

	return nextGuest;
}
int getNextSJ(){
	int i=0;
	int unfinished=0,last=0;
	for(i=0;i<NUM_GUESTS;i++){
		if(!finished[sortedList[i]]){
			unfinished++;
			last=sortedList[i];
		}
		if(waitingList[sortedList[i]] && !finished[sortedList[i]]){
			return sortedList[i];
		}
	}

	if(unfinished==1){
		return last;
	}
return -1;
}
void *sjfGuest(void *arg){
	guest_data *data = (guest_data *) arg;
	int guest_id = data->guest_id;
	int count=0,thrc=0;
	if(guest_id==sortedList[0]){
		 while(!waitingList[sortedList[NUM_GUESTS-1]]){
		}
	}
	while(count < data->numOfChickenOrdered){
		int nextGuestId=-1;
		waitingList[guest_id]=true;
		sem_wait(&guest_sem[guest_id]);
		sem_post(&mutex);
		waitingList[guest_id]=false;

		while(nextGuestId==-1){
			nextGuestId=getNextSJ();
		}
		sleep(data->timeRequiredToEat);
		ready--;
        	printf("\nGuest %d Eats a tandoori chicken. [%d/%d]", guest_id+1, ++count, data->numOfChickenOrdered);
		if(count>=data->numOfChickenOrdered){
			finished[guest_id]=true;
		}
		sem_post(&guest_sem[nextGuestId]);
	}
	sleep(1);
	printf("\nGuest %d finishes and exits", guest_id + 1);
    	pthread_exit(NULL);
}

void *guest(void *arg) {

    guest_data *data = (guest_data *) arg;
    int guest_id = data->guest_id;
    int count=0;
    int nextGuestId=0;

    while ( count< data->numOfChickenOrdered) {

        sem_wait(&guest_sem[guest_id]);
     	sem_post(&mutex);
	nextGuestId=getNextGuestId(guest_id);
	sleep(data->timeRequiredToEat);
	ready--;
        printf("\nGuest %d Eats a tandoori chicken. [%d/%d]", guest_id+1, ++count, data->numOfChickenOrdered);

	sem_post(&guest_sem[nextGuestId]);
    }
	finished[guest_id]=true;
    sleep(1);
    printf("\nGuest %d finishes and exits", guest_id + 1);
    //finished[guest_id]=true;
    pthread_exit(NULL);
}

bool validInteger(char *temp){
	int len = strlen(temp);
	int i;
	for(i=0;i<len;i++){
		if(temp[i] != '\n' && !isdigit(temp[i]) )
			return false;
	}
	return true;
}
bool validCommand(char str[100]) {
        char *token;
        token = strtok(str," ");
        int count=0;
	while(token != NULL) {
		if(count==0 && strcmp(token,"tcr")==0) {
                	token = strtok(NULL," ");
		} else if(count == 1 && ( strcmp(token,"-fcfs")==0 || strcmp(token,"-sjf")==0 || strcmp(token,"-pri")==0 )) {
			if(strcmp(token,"-sjf") == 0) isInputsjf=true;
			if(strcmp(token,"-pri") == 0) isInputpri=true;
			if(strcmp(token,"-fcfs") == 0) isInputfcfs=true;
			token = strtok(NULL," ");
		} else if(( count == 2 || count == 3 ) && token[0] == '-') {
			if(!validInteger(++token)) return false;
			int val = atoi(token);
			if(val>0){
				if(count == 3){
					NUM_GUESTS = val;
				} else if(count == 2) {
					AVAILBLE = val;
				}
			} else {
				printf("\nInvalid input. Number of dining tables/Number of cooked food should be greater than 0.\n");
				return false;
			}
			token = strtok(NULL," ");
		} else {
			return false;
		}
		count++;
        }
return true;
}

bool getValidInput() {
	char input[100];
	while(true){
		printf("12114_G05>$");
		fgets(input, sizeof(input), stdin);
		if( strcmp(input, "\n") == 0 ) {
			continue;
		}else if(strcmp(input,"bye\n")==0) {
   			printf("Good Bye\n");
			exit(0);
		} else if(!validCommand(input)) {
			printf("Invalid Command");
			printf("\nUsage : tcr -option[fcfs/sjf/pri] -Number of Cooked Food -Number of Dinning Table");
			printf("\nExample : tcr -fcfs -3 -4\n");
			continue;
		} else {
			break;
		}
	}
return true;
}
int getPriority(int guestId){
	char input[10];
	while(true){
		printf("What is the priority of Guest %d: ",guestId+1);
		fgets(input,sizeof(input),stdin);

		if(!validInteger(input) || atoi(input)==0 ){
			printf("Invalid input. Please enter correct value.");
		} else{
			break;
		}
	}
return atoi(input);
}
int getNumOfChkn(int guestId){
	char numOfChk[10];
	while(true)
	{
             printf("How many chickens does Guest %d want to eat: ",guestId+1);
             fgets(numOfChk, sizeof(numOfChk), stdin);

             if(!validInteger(numOfChk) || atoi(numOfChk)==0) {
                    printf("Invalid input. Please enter correct integer\n");
             } else{
                   break;
		}
	}
	return atoi(numOfChk);
}

int getTimeTakenToEachChkn(int guestId){
	char timeTaken[10];
	while(true)
	{
                 printf("How long does it take for Guest %d to eat a chicken: ",guestId+1);
                 fgets(timeTaken, sizeof(timeTaken), stdin);
                 if(!validInteger(timeTaken) || atoi(timeTaken)==0) {
                          printf("Invalid input. Please enter correct integer\n");
                 } else{
                          break;
                }
        }

return atoi(timeTaken);
}

void initialize(){
	int sl=0;
	for(sl=0;sl<100;sl++){
       		 sortedList[sl]=-1;
	}
}

int main(void) {

while(getValidInput()) {
if(isInputfcfs) {
	isInputfcfs=false;
	initialize();
	int k=0,g=0,i,rc,rcode;
	pthread_t guest_thr[NUM_GUESTS],chef_thr;
    	guest_data guests[NUM_GUESTS];

	// Initializing the semaphores
	sem_init(&mutex, 0, 0);
    	sem_init(&guest_sem[0], 0, 1);
	for(g=1;g<NUM_GUESTS;g++){
        	sem_init(&guest_sem[g],0,0);
    	}

	// Initialize all the Guests.
	for(k=0;k<NUM_GUESTS;k++){
		guests[k].guest_id = k;
      		guests[k].numOfChickenOrdered =getNumOfChkn(k);
      		guests[k].timeRequiredToEat = getTimeTakenToEachChkn(k);
      		howMuchToCook += guests[k].numOfChickenOrdered;
      		finished[k]=false;
	}

	//Start the Chef thread
    	if ((rcode = pthread_create(&chef_thr, NULL, chef, NULL))) {
        	fprintf(stderr, "error: pthread_create, rcode: %d\n", rcode);
        	return 1;
    	}
    	printf("Chef Starts Cooking");

	/* create guests */
    	for (i = 0; i < NUM_GUESTS; ++i) {
      		printf("\nGuest %d arrives and wants to eat %d tandoori chickens.", i+1,guests[i].numOfChickenOrdered);
      		if ((rc = pthread_create(&guest_thr[i], NULL, guest, &guests[i]))) {
        		fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
        		return 1;
      		}
    	}

    	//join chef thread to main thread.
    	pthread_join(chef_thr, NULL);

    	/* block until all guest threads complete */
    	for (i = 0; i < NUM_GUESTS; ++i) {
      		pthread_join(guest_thr[i], NULL);
    	}

	printf("\nAll guests finish eating and exit.\n");
} else if(isInputsjf){
	isInputsjf=false;
	initialize();
	int shortestGuestId=0,minTime=65535,totalTime[NUM_GUESTS],k=0,g=0,i,rc,rcode;
        pthread_t guest_thr[NUM_GUESTS],chef_thr;
        guest_data guests[NUM_GUESTS];

        // Initialize all the Guests.
        for(k=0;k<NUM_GUESTS;k++){
                guests[k].guest_id = k;
		int numOfChkn = getNumOfChkn(k);
		int time = getTimeTakenToEachChkn(k);
		if((numOfChkn*time)<minTime){
			minTime=numOfChkn*time;
			shortestGuestId=k;
		}
		totalTime[k]=numOfChkn*time;
                guests[k].numOfChickenOrdered =numOfChkn;
                guests[k].timeRequiredToEat = time;
                howMuchToCook += guests[k].numOfChickenOrdered;
                finished[k]=false;
        }

	//Initializing the semaphores
        sem_init(&mutex, 0, 0);
	for(g=0;g<NUM_GUESTS;g++){
                if(g!=shortestGuestId){
			sem_init(&guest_sem[g],0,0);
		} else{
			sem_init(&guest_sem[g],0,1);
		}
        }

	//sort guests according to timeTaken
	qsort(totalTime, NUM_GUESTS, sizeof(int), cmpfunc);
	int n=0;
	for(n=0;n<NUM_GUESTS;n++){
		int val = totalTime[n];
		int m=0;
		for(m=0;m<NUM_GUESTS;m++){
			int num = guests[m].numOfChickenOrdered;
			int time = guests[m].timeRequiredToEat;
			if(val==(num*time)){
				if(arrayAlreadyHas(m)){
                                        continue;
                                }else{
                                        sortedList[n]=m;
                                        break;
                                }
			}
		}
	}
        //Start the Chef thread
        if ((rcode = pthread_create(&chef_thr, NULL, chef, NULL))) {
                fprintf(stderr, "error: pthread_create, rcode: %d\n", rcode);
                return 1;
        }
        printf("Chef Starts Cooking");

        /* create guests */
        for (i = 0; i < NUM_GUESTS; ++i) {
                printf("\nGuest %d arrives and wants to eat %d tandoori chickens.", i+1,guests[i].numOfChickenOrdered);
                if ((rc = pthread_create(&guest_thr[i], NULL, sjfGuest, &guests[i]))) {
                        fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
                        return 1;
                }
        }

        //join chef thread to main thread.
        pthread_join(chef_thr, NULL);

        /* block until all guest threads complete */
        for(i = 0; i < NUM_GUESTS; ++i) {
                pthread_join(guest_thr[i], NULL);
        }
printf("\nAll guests finish eating and exit.\n");
} else if(isInputpri){
	isInputpri=false;
	initialize();
        int shortestGuestId=0,minTime=65535,priorityQueue[NUM_GUESTS],k=0,g=0,i,rc,rcode;
        pthread_t guest_thr[NUM_GUESTS],chef_thr;
        guest_data guests[NUM_GUESTS];

        // Initialize all the Guests.
        for(k=0;k<NUM_GUESTS;k++){
                guests[k].guest_id = k;
                int numOfChkn = getNumOfChkn(k);
                int time = getTimeTakenToEachChkn(k);
		int priority = getPriority(k);
                if(priority<minTime){
                        minTime=priority;
                        shortestGuestId=k;
                }
                priorityQueue[k]=priority;
                guests[k].numOfChickenOrdered =numOfChkn;
                guests[k].timeRequiredToEat = time;
		guests[k].priority = priority;
                howMuchToCook += guests[k].numOfChickenOrdered;
                finished[k]=false;
        }

        //Initializing the semaphores. shortest guest should be initialed to 1.
        sem_init(&mutex, 0, 0);
        for(g=0;g<NUM_GUESTS;g++){
                if(g!=shortestGuestId){
                        sem_init(&guest_sem[g],0,0);
                } else{
                        sem_init(&guest_sem[g],0,1);
                }
        }

        //sort guests according to timeTaken
        qsort(priorityQueue, NUM_GUESTS, sizeof(int), cmpfunc);
        int n=0,m=0;
        for(n=0;n<NUM_GUESTS;n++){
           	for(m=0;m<NUM_GUESTS;m++){
              		if(priorityQueue[n]==guests[m].priority){
				if(arrayAlreadyHas(m)){
					continue;
				}else{
					sortedList[n]=m;
					break;
				}
			}
           	}
	}

        //Start the Chef thread
        if ((rcode = pthread_create(&chef_thr, NULL, chef, NULL))) {
                fprintf(stderr, "error: pthread_create, rcode: %d\n", rcode);
                return 1;
        }
        printf("Chef Starts Cooking");
	/* create guests */
        for (i = 0; i < NUM_GUESTS; ++i) {
                printf("\nGuest %d arrives and wants to eat %d tandoori chickens.", i+1,guests[i].numOfChickenOrdered);
                if ((rc = pthread_create(&guest_thr[i], NULL, sjfGuest, &guests[i]))) {
                        fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
                        return 1;
                }
        }

        //join chef thread to main thread.
        pthread_join(chef_thr, NULL);

        /* block until all guest threads complete */
        for(i = 0; i < NUM_GUESTS; ++i) {
                pthread_join(guest_thr[i], NULL);
        }
	printf("\nAll guests finish eating and exit.\n");
}//pri end
}
return 0;
}
