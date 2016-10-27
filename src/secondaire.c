#include "polling.h"
#include "header.h"

void invitationEmission();
void printState();
void receptionDonnee();
void requeteEmission();
void receptionAcquittement();
void rien(){
	printState();
	printf(" Il ne se passe rien\n");
};

int n = 0, pid_primaire = 0, tab=0, nb_data_req_rx = 0;

//station secondaire
int main (int argc,char *argv[]){
	if (argc != 3) {
		perror("2 paramètres nécessaires");
		exit(1);
	}

	int i=0;
	char *prefixe_fichier;

	n = atoi(argv[1]);
	pid_primaire = atoi(argv[2]);
	tab=(3*n);

	if(getenv("PREFIXE") != NULL){
		prefixe_fichier = malloc(4+sizeof(getenv("PREFIXE")));
		snprintf(prefixe_fichier, sizeof(prefixe_fichier), "%s", getenv("PREFIXE"));
		sprintf(prefixe_fichier + strlen(prefixe_fichier), "_St");
		sprintf(prefixe_fichier + strlen(prefixe_fichier), "%s", argv[1]);
		stdout = fopen(prefixe_fichier, "a");
	}

    state = IDLE;
    string_state = string_idle;

	for EVER{
		fflush(stdout);
		switch(state){
			case IDLE:
				printState();
				fprintf(stdout, "\n");
				signal(DATA_RX, receptionDonnee);
				signal(DATA_REQ_RX, requeteEmission);
				// signal(POLL_RX, rien);
				signal(POLL_RX, SIG_IGN);
				pause();
			break;
			case W_POLL:
				printState();
				fprintf(stdout, " Attente\n");
				signal(DATA_RX, receptionDonnee);
				signal(DATA_REQ_RX, requeteEmission);
				signal(POLL_RX, invitationEmission);
				pause();
			break;
			case SD_DATA:
				printState();
				fprintf(stdout, "\n");
				signal(DATA_RX, receptionDonnee);
				kill(pid_primaire, DATA_TX);
				state = W_ACK;
				string_state = string_w_ack;
			break;
			case W_ACK:
				printState();
				fprintf(stdout, "\n");
				signal(DATA_RX, receptionDonnee);
				signal(ACK_RX, receptionAcquittement);
				signal(POLL_RX, SIG_IGN);
				pause();
			break;
		}
	}

	return(EXIT_SUCCESS);
}

void invitationEmission(){
	printState();
	fprintf(stdout, " Poll_Rx\n");
	state = SD_DATA;
	string_state = string_sd_data;
}

void printState(){
	int i;

	for(i=0;i<=tab;i++)
		fprintf(stdout, "\t");

	fprintf(stdout, "St%d %s", n, string_state);
}

void receptionAcquittement(){
	printState();
	if(nb_data_req_rx > 0){
		nb_data_req_rx--;
		state = W_POLL;
		string_state = string_w_poll;
	}
	if(nb_data_req_rx == 0){
		state = IDLE;
		string_state = string_idle;
	}
	fprintf(stdout, " Ack_Rx %d\n", nb_data_req_rx);
}

void receptionDonnee(int sig){
	printState();
	fprintf(stdout, " Data_rx\n");
}

void requeteEmission(){
	printState();
	if(state == IDLE){
		state = W_POLL;
		string_state = string_w_poll;
	}
	nb_data_req_rx++;
	fprintf(stdout, " Data_Req_Rx %d\n", nb_data_req_rx);
}
