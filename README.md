# Reti_laboratorio

Progetti svolti per il Corso Reti di Cacolatori.
## Descrizione dei progetti:

- Applicazione TCP client/server, dove il server è una calcolatrice remota:
1. Il client richiede la connessione al server
2. Stabilita la connessione, il client legge dallo standard input l'operazione da eseguire e invia i dati al server
3. Il server legge quanto inviato dal client, esegue l'operazione richiesta e invia il risultato al client
4. Il client legge la risposta inviata dal server, la visualizza sullo standard output e attende una nuova operazione da compiere

Il server non termina mai il suo processo e deve essere in grado di accettare una coda massima di 5 client



- Applicazione UDP client/server, dove il server è una calcolatrice remota:
1. Il client è avviato passando come parametri di input il nome e il numero di porta del server UDP da contattare
2. Il client legge da tastiera l'operazione da compiere ed invia i dati al server
3. Il server esegue l'operazione ed invia i dati in un esatto formato, es: 2 + 5 = 7
4. Il client legge la risposta inviata dal server, la visualizza sullo standard output e attende una nuova operazione da compiere

Il server non termina mai il processo, ma resta in ascolto.
