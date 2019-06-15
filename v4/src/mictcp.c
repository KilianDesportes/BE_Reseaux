#include <mictcp.h>
#include <api/mictcp_core.h>
#include <stdlib.h>

#define TIMEMOUT 10
#define PORT_S 9000
#define PORT_D 10000
#define max_sockets 10
#define pourcent_perte_min "10"
#define str_perte_voulue "10"
#define taille_fenetre 50
#define max_essai 2

mic_tcp_sock sockets[max_sockets];
int fenetre[taille_fenetre];
int i = 0;
int sn = 0;
int nb_attendu = 0;
int cpt_error = 0;
int pourcent_perte_actuel = 10;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


/*
 * Permet de créer un socket entre l’application et MIC-TCP
 * Retourne le descripteur du socket ou bien -1 en cas d'erreur
 */
int mic_tcp_socket(start_mode sm)
{
	int result = -1;
	mic_tcp_sock s;
	s.state = IDLE;
	s.fd = i;
	sockets[i] = s;
	i++;
	set_loss_rate(0);
	result = initialize_components(sm); /* Appel obligatoire */
	return result;
}

/*
 * Permet d’attribuer une adresse à un socket.
 * Retourne 0 si succès, et -1 en cas d’échec
 */
int mic_tcp_bind(int socket, mic_tcp_sock_addr addr)
{
	if (socket > max_sockets)
	{
		perror("bind failed\n");
		return -1;
	}
	sockets[socket].addr = addr;
	printf("bind success\n");
	return 0;
}

/*
 * Met le socket en état d'acceptation de connexions
 * Retourne 0 si succès, -1 si erreur
 */
int mic_tcp_accept(int socket, mic_tcp_sock_addr *addr)
{
	/*Attente du SYN*/
	pthread_mutex_lock(&mutex);
	while (sockets[socket].state == IDLE)
	{
		pthread_cond_wait(&cond,&mutex);
	}
	pthread_mutex_unlock(&mutex);

	sockets[socket].state = ESTABLISHED;

	return 0;
}

/*
 * Permet de réclamer l’établissement d’une connexion
 * Retourne 0 si la connexion est établie, et -1 en cas d’échec
 */
int mic_tcp_connect(int socket, mic_tcp_sock_addr addr)
{
	/*Envoi du PDU SYN*/
	mic_tcp_pdu p;
	p.payload.size = 0;
	p.header.source_port = sockets[socket].addr.port;
	p.header.dest_port = addr.port;
	p.header.seq_num = 0;
	p.header.ack_num = 0;
	p.header.syn = 1;
	p.header.ack = 0;
	p.header.fin = 0;
	p.payload.data = str_perte_voulue;
	p.payload.size = strlen(str_perte_voulue)+1;

	mic_tcp_pdu pdu_recv = { 0 };

	printf("SYN send %d\n",	IP_send(p, addr));
	/*Protocole en SYN_SENT*/
	sockets[socket].state = SYN_SENT;
	
	/*Attente du SYN_ACK*/
	int cpt = 0;
	while (((IP_recv(&pdu_recv,&sockets[socket].addr, TIMEMOUT) == -1) || ((pdu_recv.header.syn != 1) && (pdu_recv.header.ack != 1))) 
		&& cpt < max_essai)
	{
		cpt++;
	}
	if(cpt == max_essai){
		perror("Error rcv SYN ACK\n");
		return -1;
	}

	if(pdu_recv.header.fin == 1){
		printf("Pourcentage d'erreur non acceptable par le serveur\n");
		sockets[socket].state = IDLE;
		return -1;
	}else{
		printf("SYN ACK recv\n");
	}

	p.header.source_port = sockets[socket].addr.port;
	p.header.dest_port = addr.port;
	p.header.syn = 0;
	p.header.ack = 1;
	IP_send(p, addr);

	printf("ACK send\n");

	sockets[socket].state = ESTABLISHED;

	return 0;

}

/*
 * Permet de réclamer l’envoi d’une donnée applicative
 * Retourne la taille des données envoyées, et -1 en cas d'erreur
 */
int mic_tcp_send(int mic_sock, char *mesg, int mesg_size)
{
	int t;
	int sended = 0;
	mic_tcp_pdu p;
	while (sended == 0)
	{
		p.header.source_port = 1;
		p.header.dest_port = sockets[mic_sock].addr.port;
		p.header.seq_num = sn;
		p.header.ack_num = 0;
		p.header.syn = 0;
		p.header.ack = 0;
		p.header.fin = 0;
		p.payload.data = mesg;
		p.payload.size = mesg_size;
		t = IP_send(p, sockets[mic_sock].addr);
		IP_recv(&p, &sockets[mic_sock].addr, TIMEMOUT);
		if (p.header.ack == 1)
		{
			if(fenetre[sn%taille_fenetre]==1){
				cpt_error--;
				printf("Last window was error, new number of error %d\n",cpt_error);
			}
			fenetre[sn%taille_fenetre]=0;
			sended = 1;
			sn++;
		}
		else
		{
			if ((float)cpt_error < (float)taille_fenetre / pourcent_perte_actuel)
			{
				printf("Pourcentage acceptable\n");
				fenetre[sn%taille_fenetre]=1;
				sn++;
				cpt_error++;
				sended = 1;
				printf("Error, new number of error %d\n",cpt_error);
			}else{
				printf("Pourcentage non acceptable\n");
			}
			printf("Nb erreur %d\n",cpt_error);
		}
	}
	return t;
}

/*
 * Permet à l’application réceptrice de réclamer la récupération d’une donnée
 * stockée dans les buffers de réception du socket
 * Retourne le nombre d’octets lu ou bien -1 en cas d’erreur
 * NB : cette fonction fait appel à la fonction app_buffer_get()
 */
int mic_tcp_recv(int socket, char *mesg, int max_mesg_size)
{
	mic_tcp_payload payload;
	payload.data = mesg;
	payload.size = max_mesg_size;
	return app_buffer_get(payload);
}

/*
 * Permet de réclamer la destruction d’un socket.
 * Engendre la fermeture de la connexion suivant le modèle de TCP.
 * Retourne 0 si tout se passe bien et -1 en cas d'erreur
 */
int mic_tcp_close(int socket)
{
	return -1;
}

/*
 * Traitement d’un PDU MIC-TCP reçu (mise à jour des numéros de séquence
 * et d'acquittement, etc.) puis insère les données utiles du PDU dans
 * le buffer de réception du socket. Cette fonction utilise la fonction
 * app_buffer_put().
 */

void process_received_PDU(mic_tcp_pdu pdu, mic_tcp_sock_addr addr)
{
	int temp;	
	for(int i = 0 ; i < max_sockets ; i++){
		if(sockets[i].addr.port == pdu.header.dest_port){
			temp = i;
		}	
	}


	mic_tcp_pdu p_pdu = { 0 };
	/*Contexte de connexion*/
	if(sockets[temp].state != ESTABLISHED){
	/*Reception SYN*/
		if(pdu.header.syn == 1 && pdu.header.ack == 0)
		{
			sockets[temp].state = SYN_RECEIVED;
			int pourcent_perte_voulu_client = atoi(pdu.payload.data);
			if(pourcent_perte_voulu_client >= atoi(pourcent_perte_min)){
				printf("Perte supérieur : OK\n");
				pourcent_perte_actuel = pourcent_perte_voulu_client;
			}else{
				printf("Perte inférieur : NOK\n");
				sockets[temp].state = IDLE;
				p_pdu.header.fin = 1;
			}
			p_pdu.header.source_port = sockets[temp].addr.port;
			p_pdu.header.dest_port = pdu.header.source_port;
			p_pdu.header.syn = 1;
			p_pdu.header.ack = 1;
			printf("send %d\n",IP_send(p_pdu, addr));
		}
		/*Reception ACK dans le contexte de la connexion*/
		else if(pdu.header.ack == 1){
			pthread_cond_signal(&cond);
		}
	}
	/*Reception paquet normal*/
	else 
	{	
		p_pdu.header.source_port = sockets[temp].addr.port;
		p_pdu.header.dest_port = pdu.header.source_port;
		p_pdu.header.seq_num = pdu.header.seq_num;
		p_pdu.header.ack = 1;
		if (nb_attendu == pdu.header.seq_num)
		{
			app_buffer_put(pdu.payload);
		}
		nb_attendu = pdu.header.seq_num + 1;
		IP_send(p_pdu, addr);
	}
}
