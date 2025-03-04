#include <mictcp.h>
#include <api/mictcp_core.h>
#include <stdlib.h>

#define TIMEMOUT 10
#define PORT_S 9000
#define PORT_D 10000
#define max_sockets 10

mic_tcp_sock sockets[max_sockets];
int i = 0;

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
	set_loss_rate(20);
	result = initialize_components(sm);
	return result;
}

/*
 * Permet d’attribuer une adresse à un socket.
 * Retourne 0 si succès, et -1 en cas d’échec
 */
int mic_tcp_bind(int socket, mic_tcp_sock_addr addr)
{
	printf("[MIC-TCP] Appel de la fonction: ");
	printf(__FUNCTION__); printf("\n");
	return 0;
}

/*
 * Met le socket en état d'acceptation de connexions
 * Retourne 0 si succès, -1 si erreur
 */
int mic_tcp_accept(int socket, mic_tcp_sock_addr *addr)
{
	printf("[MIC-TCP] Appel de la fonction: ");
	printf(__FUNCTION__); printf("\n");
	return 0;
}

/*
 * Permet de réclamer l’établissement d’une connexion
 * Retourne 0 si la connexion est établie, et -1 en cas d’échec
 */
int mic_tcp_connect(int socket, mic_tcp_sock_addr addr)
{
	printf("[MIC-TCP] Appel de la fonction: ");
	printf(__FUNCTION__); printf("\n");
	return 0;
}

/*
 * Permet de réclamer l’envoi d’une donnée applicative
 * Retourne la taille des données envoyées, et -1 en cas d'erreur
 */
int mic_tcp_send(int mic_sock, char* mesg, int mesg_size)
{
	printf("[MIC-TCP] Appel de la fonction: ");
	printf(__FUNCTION__); printf("\n");
	mic_tcp_pdu p;
	p.header.source_port = PORT_S;
	p.header.dest_port = PORT_D;
	p.header.seq_num = 0;
	p.header.ack_num = 0;
	p.header.syn = 0;
	p.header.ack = 0;
	p.header.fin = 0;
	p.payload.data = mesg;
	p.payload.size = mesg_size;
	int t = IP_send(p,sockets[mic_sock].addr);
	return t;
}

/*
 * Permet à l’application réceptrice de réclamer la récupération d’une donnée
 * stockée dans les buffers de réception du socket
 * Retourne le nombre d’octets lu ou bien -1 en cas d’erreur
 * NB : cette fonction fait appel à la fonction app_buffer_get()
 */
int mic_tcp_recv(int socket, char* mesg, int max_mesg_size)
{
	printf("[MIC-TCP] Appel de la fonction: ");
	printf(__FUNCTION__); printf("\n");
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
	printf("[MIC-TCP] Appel de la fonction: ");
	printf(__FUNCTION__); printf("\n");
	app_buffer_put(pdu.payload);
}
