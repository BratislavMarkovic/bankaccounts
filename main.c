#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 30

struct SAccount {

	int number;
	char  owner[31];
	int saldo;

} ;

typedef struct SAccount Account;

int cleanInput()
{
	int i = 0;
    while (getchar()!='\n') i++;
    return i;
}


void printHeader(){

    printf("%-10s%-30s%-10s\n","Number","Owner","Saldo (kr)");
    printf("--------------------------------------------------\n\n");
}

void printAccount(Account * pAccount){

    printf("%-10.06d%-30s%-10d\n",pAccount->number,pAccount->owner,pAccount->saldo);

}


void printAccounts(Account * pAccounts[],int size){

	printHeader();

	for(int i = 0 ; i < size ; i++){

		printAccount(pAccounts[i]);

	}

}

int readAccountNumber(FILE * input,FILE * output,int * pAccountNumber, int ignoreEmpty){
	char num[7];

	while(1){
		fprintf(output,"Enter account number:");
		if(ignoreEmpty == 0){
			fprintf(output," (Press enter to abort):");
		}else {
			fprintf(output,":");
		}
		if(fgets(num,7,input) != NULL){
			if(num[strlen(num)-1]!='\n'){
			// if last read character is not '\n' (Enter) then clean input becasue there are more chars to read
				cleanInput();
			}
			if(ignoreEmpty == 0 && strlen(num)==1){
				// this string contains only '\n' character, user wants to abort
				return 1;
			} else{
				// parse num
				*pAccountNumber =  atoi(num);
				if(*pAccountNumber != 0){
					return 0;
				} else {
					fprintf(output,"Account number must contain digits and cannot be 0!\n");
				}
			}
		} else {
			return 1;
		}
	} 
	
}

int readOwner(FILE * input,FILE * output,char * pOwner){
	while(1){
		fprintf(output,"Enter account owner:");
		if(fgets(pOwner,31,input) != NULL){
			// if last read character is new line then clean input
			if(pOwner[strlen(pOwner)-1]!='\n'){
				cleanInput();
				fprintf(output,"All but first 50 characters of entered owner name will be ignored\n");
			}
			if(strlen(pOwner)==1){
				fprintf(output,"Account owner name must be provided\n");
			} else{
				// cleanup enter from the name if it is the last char
				if(pOwner[strlen(pOwner)-1]=='\n'){
					pOwner[strlen(pOwner)-1] = '\0';
				}
				return 0;
			}
		} else {
			return 1;
		}

	}
}

void addAccount(FILE * input,FILE * output,Account * pAccounts[], int * pSize, int rSaldo){

	while(1){

		if(*pSize == MAX_ACCOUNTS){
			
			fprintf(output,"No more acccounts can be added. Please remove some of existing accounts in order to add new ones.\n");
			
			return;
		}

		// if account number not entered break the infinite loop

		fprintf(output,"Add new account:\n");

		int accountNumber = 0;

		if(readAccountNumber(input,output,&accountNumber,0) != 0){

			fprintf(output,"Action cancelled by user.\n");
			
			return;

		}

		// check if account number exists in the list

		for(int i = 0; i < *pSize; i++){

			if(pAccounts[i]->number == accountNumber){
				
				fprintf(output,"Provided account number already exists.\n");

				return;
			}

		}

		Account * pAccount = malloc(sizeof(Account));

		pAccount->number = accountNumber;

		readOwner(input,output,pAccount->owner);

		pAccount->saldo = 0;

		if(rSaldo){

			readSaldo(input,output,&pAccount->saldo,"saldo",1);
		} 

		pAccounts[*pSize] = pAccount;

		(*pSize)++;

		fprintf(output,"New account added successfully\n");

	}

}

int compare(Account * pAccountFirst,Account * pAccountSecond, int compareBy){

	switch(compareBy){

		case 'n':{
			return pAccountFirst->number - pAccountSecond->number;
		}
		case 'o':{
			return strcmp(pAccountFirst->owner,pAccountSecond->owner);
		} 
		case 's':{
			return pAccountFirst->saldo - pAccountSecond->saldo;
		}
		default:
			return 0;

	}

}

void sortAccounts(Account * pAccounts[],int size){

	int choice = 0;

	while(1){
		
		printf("Choose sorting option between account number (n) | owner (o) | saldo (s):");
		
		choice = fgetc(stdin);

		cleanInput();
		
		if(choice != 0){

			switch(choice){
				case 'n':
				case 'o':
				case 's':
				{
					break;
				}
				default:
					continue;
			}
			break;
		} else {
			printf("Unexpected error!!!");
			return;
		}
	}



	int swapping;

	do {

		swapping = 0;

		for(int i = 0; i < size-1 ; i++ ){
			if(compare(pAccounts[i],pAccounts[i+1],choice) > 0){
				Account * temp = pAccounts[i];
				pAccounts[i] = pAccounts[i+1];
				pAccounts[i+1] = temp;
				swapping = 1; 
			}
		}

	}while(swapping);

	printf("Sorting successfully completed.\n");

}

int readSaldo(FILE * input,FILE * output,int * pSaldo,const char * valueDescription,int allowZero){
	char num[11];

	while(1){
		
		fprintf(output,"Enter ");

		fprintf(output,valueDescription);
		fprintf(output,":");

		if(fgets(num,11,input) != NULL){
			if(num[strlen(num)-1]!='\n'){
			// if last read character is not '\n' (Enter) then clean input becasue there are more chars to read
				cleanInput();
			}
			// parse num
			*pSaldo =  atoi(num);
			if( *pSaldo != 0){
				return 0;
			} else {
				fprintf(output,valueDescription);
				fprintf(output," must contain digits and cannot be 0!\n");
			}
		} else {
			return 1;
		}
	} 

}

void searchAccounts(Account * pAccounts[],int size){

	FILE * input = stdin;
	FILE * output = stdout;

	int choice = 0;
	int accountNumber = 0;
	int minSaldo , maxSaldo = 0;
	char ownerPattern[31];

	while(1){
		
		fprintf(output,"Choose search option between account number (n) | owner (o) | saldo (s):");
		
		choice = fgetc(input);

		cleanInput();
		
		if(choice != 0){

			switch(choice){
				case 'n':
				{
					// get account number from input
					readAccountNumber(input,output,&accountNumber,1);
					break;
				}
				case 'o':
				{
					readOwner(input,output,ownerPattern);
					break;
				}
				case 's':
				{
					readSaldo(input,output,&minSaldo,"min saldo",1);
					readSaldo(input,output,&maxSaldo,"max saldo",1);
					break;
				}
				default:
					continue;
			}
			break;
		} else {
			fprintf(output,"Unexpected error");
			return;
		}
	}

	Account * results[size];
	int resultsSize = 0;

	for(int i = 0; i < size ; i++){

		switch(choice){
			case 'n':
			{
				if(pAccounts[i]->number == accountNumber){
					results[resultsSize++] = pAccounts[i];
				}
				break;
			}
			case 'o':
			{
				if(strstr(pAccounts[i]->owner,ownerPattern)!= NULL){
					results[resultsSize++] = pAccounts[i];
				}
				break;
			}
			case 's':
			{
				if(pAccounts[i]->saldo >= minSaldo && pAccounts[i]->saldo <= maxSaldo){
					results[resultsSize++] = pAccounts[i];
				}
				break;
			}
			default:
				continue;
		}

	}

	printAccounts(results,resultsSize);

}

int chooseAccount(Account * pAccounts[],int size){

	FILE * input = stdin;
	FILE * output = stdout;

	int choice = 0;
	int accountNumber = 0;
	char ownerPattern[31];

	while(1){
		
		fprintf(output,"Choose search option between account number (n) | owner (o):");
		
		choice = fgetc(input);

		cleanInput();
		
		if(choice != 0){

			switch(choice){
				case 'n':
				{
					// get account number from input
					readAccountNumber(input,output,&accountNumber,1);

					for(int i = 0; i < size ; i++){
						if(pAccounts[i]->number == accountNumber){
							return i;
						}
					}
					fprintf(output,"Account with provided number doesn't exist\n");
					break;
				}
				case 'o':
				{
					readOwner(input,output,ownerPattern);

					Account * results[size];
					int resultsSize = 0;

					for(int i = 0; i < size ; i++){
						if(strstr(pAccounts[i]->owner,ownerPattern)!= NULL){
							results[resultsSize++] = pAccounts[i];
						}
					}

					printAccounts(results,resultsSize);
					break;
				}
				default:
					continue;
			}
			//break;
		} else {
			fprintf(output,"Unexpected error");
			return -1;
		}
		
	}

}

void changeSaldo(Account * pAccounts[],int size){

	FILE * input = stdin;
	FILE * output = stdout;


	printf("Choose an account to change saldo.\n");

	Account * pChosen = pAccounts[chooseAccount(pAccounts,size)];

	printAccount(pChosen);

	int transactionType = 0;
	int saldo = 0;

	while(1){
		
		printf("Choose transaction option between deposit (d) | withdrawal (w):");
		
		transactionType = fgetc(stdin);

		cleanInput();
		
		if(transactionType != 0){

			switch(transactionType){
				case 'd':{
					break;
				}
				case 'w':
				{
					if(pChosen->saldo != 0){
						break;
					} else {
						printf("Withdrawal is not available for chosen account!\n");
					}
				}
				default:
					continue;
			}
			break;
		} else {
			printf("Unexpected error!");
			return;
		}
	}

	while(1){

		readSaldo(input,output,&saldo,"amount",0);	

		if(transactionType == 'w' ){

			if(pChosen->saldo < saldo){

				printf("Enter amount that is less or equal than current saldo on customer account!\n");

			} else if(saldo < 0){

				printf("Enter amount that is not negative number!\n");

			} else {

				break;

			}

		} else {
			break;
		}

	}

	if(transactionType == 'w'){
		pChosen->saldo -= saldo;
	} else {
		pChosen->saldo += saldo;
	}

	printf("Saldo successfully updated.\n");

}

void removeAccount(Account * pAccounts[],int * pSize){

	printf("Choose an account to remove.\n");

	int accountIndex = chooseAccount(pAccounts,*pSize);

	Account * pChosen = pAccounts[accountIndex];

	if(pChosen->saldo > 0){
		printf("Account cannot be removed since saldo is not empty\n");
	} else {

		for(int i=accountIndex; i< *pSize-1;i++){
			pAccounts[i] = pAccounts[i+1];
		}
		free(pChosen);
		(*pSize)--;
		printf("Account sucessfully removed.\n");
	}

}

void transferBetweenAccounts(Account * pAccounts[],int size){

	FILE * input = stdin;
	FILE * output = stdout;

	Account * pFrom = NULL;
	Account * pTo = NULL;

	printf("Transfer between accounts\n");

	if(size<2){
		printf("Transfer requires at least two accounts\n");
		return;
	}


	while(1) {

		printf("Choose an account to transfer from\n");

		pFrom = pAccounts[chooseAccount(pAccounts,size)];

		if(pFrom->saldo > 0){
			break;
		} else {
			printf("Account to transfer from must have positive saldo\n");
		}

	}

	while(1) {

		printf("Choose an account to transfer to\n");

		pTo = pAccounts[chooseAccount(pAccounts,size)];

		if(pFrom != pTo){
			break;
		} else {
			printf("Transfer must be done between different accounts\n");
		}

	}

	int amount = 0;

	while(1){

		readSaldo(input,output,&amount,"amount",0);

		if(pFrom->saldo < amount){
			printf("Provide amount that is less then saldo\n");
		} else {
			break;
		}

	}

	pFrom->saldo-=amount;
	pTo->saldo+=amount;

	printf("Transfer sucessfully completed!\n");

}

int addValue(int unique_values [],int * unique_value_counter, int unique_value){

	for(int i = 0; i < *unique_value_counter; i++){
		if(unique_values[i] == unique_value){
			return i;
		}
	}
	unique_values[*unique_value_counter] = unique_value;
	return (*unique_value_counter)++;
}

void printStatistics(Account * pAccounts[],int size){

	double average = 0;
	int values [size];
	int unique_values [size];
	int apperances [size];
	int typical_value = 0;
	int typical_value_index = -1;
	double median = 0;
	int unique_value_counter = 0;

	for(int i = 0; i < size; i++){
		values[i] = pAccounts[i]->saldo;
		average+=values[i];
		apperances[addValue(unique_values,&unique_value_counter,values[i])]++;
	}

	// calculate how many elements with max max value exists in array
	int maxNumberOfApperances = 0;
	for(int i = 0; i < unique_value_counter; i++){
		if(apperances[i]>maxNumberOfApperances){
			maxNumberOfApperances = apperances[i];
			typical_value_index = i;
		}
	}

	typical_value = unique_values[typical_value_index];


	// caculate average
	average /= size;


	// calculate median
	// sort values list

	int swapping = 0;

	do {

		swapping = 0;

		for(int i = 0; i < size-1 ; i++ ){
			if(values[i]>values[i+1]){
				int temp = values[i];
				values[i] = values[i+1];
				values[i+1] = temp;
				swapping = 1; 
			}
		}

	}while(swapping);

	median = size%2==1?values[(size/2)]:(values[(size/2)] + values[(size/2)-1])/2;

	printf("Average saldo value is %f, typical saldo value is %d and median is %f\n",average,typical_value,median);


}

void loadAccounts(const char * fileName,Account * pAccounts[],int * pSize){

	FILE * input = fopen(fileName,"r");

	FILE * log = fopen("load_log.txt","w+");

	addAccount(input,log,pAccounts,pSize,1);

	fclose(input);

	fclose(log);

}

void storeAccount(FILE * output,Account * pAccount){

	fprintf(output,"%d\n%s\n%d\n",pAccount->number,pAccount->owner,pAccount->saldo);

}

void storeAccounts(const char * fileName,Account * pAccounts[],int  size){
	
	FILE * output = fopen(fileName,"w+");

	for(int i = 0; i < size ; i++){

		storeAccount(output,pAccounts[i]);

	}

	fclose(output);

}

int selectFromMainMenu(){
	FILE * output = stdout;
	FILE * input = stdin;

	while(1){
		fprintf(output,"------------------------------\n\n");
		fprintf(output,"(a)dd new account\n");
		fprintf(output,"(p)rint all accounts\n");
		fprintf(output,"(s)ort accounts\n");
		fprintf(output,"(f)ind account\n");
		fprintf(output,"(c)hange saldo\n");
		fprintf(output,"(r)emove account\n");
		fprintf(output,"(t)ransfer between accounts\n");
		fprintf(output,"(i)nfo statistic\n");
		fprintf(output,"(q)uit program\n");
		fprintf(output,"-------------------------------\n\n");
		fprintf(output,"Choose action above:");

		int choice = fgetc(input);

		cleanInput();
		
		if(choice != 0){

			switch(choice){
				case 'a':
				case 'p':
				case 's':
				case 'f':
				case 'c':
				case 'r':
				case 't':
				case 'i':
					return choice;
				case 'q':
					return -1;
				default:
					continue;
			}
		} else {
			fprintf(output,"Unexpected error");
			return -1;
		}
	}

}


int main()
{

	Account * accounts[MAX_ACCOUNTS];

	int noOfAccounts;

	loadAccounts("accounts.txt",accounts,&noOfAccounts);

	int choice = 0;

	while((choice = selectFromMainMenu()) != -1){

		switch(choice){
			case 'a':
				addAccount(stdin,stdout,accounts,&noOfAccounts,0);
				break;
			case 'p':
				printAccounts(accounts,noOfAccounts);
				break;
			case 's':
				sortAccounts(accounts,noOfAccounts);
				break;
			case 'f':
				searchAccounts(accounts,noOfAccounts);
				break;
			case 'c':
				changeSaldo(accounts,noOfAccounts);
				break;
			case 'r':
				removeAccount(accounts,&noOfAccounts);
				break;
			case 't':
				transferBetweenAccounts(accounts,noOfAccounts);
				break;
			case 'i':
				printStatistics(accounts,noOfAccounts);
				break;
		}

	}

	storeAccounts("accounts.txt",accounts,noOfAccounts);

    return 0;
}
