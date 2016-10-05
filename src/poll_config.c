#include "polling.h"
#include "header.h"

int main (int argc,char *argv[]){
	if (argc < 9) {
		perror("8 à 9 paramètres attendus");
		exit(1);
	}
	if(argc > 10){
		perror("8 à 9 paramètres attendus");
		exit(1);
	}

	char *primaire = argv[1], *secondaire = argv[2], *trafic = argv[3], *string_state = string_idle,
	*prefixe_fichier = NULL; // Retirer cette ligne cause la levée d'une erreur lors de la compilation

	int i, state = IDLE, numero_station = 1, pid;
	// On ne convertit pas en int car il faudrait les cast à nouveau en char pour les remettre en argument
	// des exec
	char *n = argv[4], //nombre de station
	*nb_polling = argv[5], *delai_polling = argv[6], *delai_min_requete = argv[7],
	*delai_max_requete = argv[8], iStr[10], ppid[10], pid_St[NB_STATIONS][10];

	// Injecte le pid dans une chaîne de caractères pour l'utiliser avec l'exec
	snprintf(ppid, sizeof(ppid), "%d", getpid());

	if(argc == 10){
		prefixe_fichier = argv[9];
	}

	if(atoi(delai_min_requete) > atoi(delai_max_requete)){
		perror("Le délai minimum ne peut être plus grand que le délai maximum");
		exit(1);
	}
	if(atoi(delai_min_requete) < 0){
		perror("Le délai minimum ne peut être négatif ou nul");
		exit(1);
	}

	for(i=1; i <= min(atoi(n), NB_STATIONS); i++){
		snprintf(iStr, sizeof(iStr), "%d", i);
		// Création d'une station secondaire
		if((pid=fork()) == 0){
			// stdout = fopen(*prefixe_fichier+"_st"+*iStr, "a");
			printf("Je créé la station secondaire %s\n", iStr);
			fflush(stdout);
			execl(secondaire, secondaire, iStr, ppid, (char *) NULL);
			perror("Erreur d'execl pour le secondaire");
			exit(1);
		}else{
			snprintf(pid_St[i], sizeof(pid_St[i]), "%d", pid);

			// Création d'un générateur de requête
			if((pid = fork()) == 0){
				execl(trafic, trafic, iStr, pid_St[i], delai_min_requete, delai_max_requete, (char *) NULL);
				perror("Erreur d'execl pour le trafic");
				exit(1);
			}
		}
	}

	printf("Je suis poll_config, mes paramètres sont: primaire=%s, secondaire=%s, trafic=%s, n=%s, nb_polling=%s, delai_polling=%s, delai_min_requete=%s, delai_max_requete=%s", primaire, secondaire, trafic, n, nb_polling, delai_polling, delai_min_requete, delai_max_requete);
	if(argc == 10){
		printf(", prefixe_fichier=%s", prefixe_fichier);
	}
	printf(".\n");

	// Création de la station primaire
	execl(primaire, primaire, nb_polling, delai_polling, n, pid_St[0], pid_St[1], pid_St[2], pid_St[3], pid_St[4], NULL);
	perror("Erreur d'execl pour le primaire");
	exit(1);
}

int min(int a, int b){
	if(a >= b){
		return b;
	}else{
		return a;
	}
}