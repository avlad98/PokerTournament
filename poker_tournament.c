/* Vlad Andrei-Alexandru - 311CB */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"

#define config 	1
#define event 	2
#define output	3
#define MAX	64
#define DLM " \n"

TLista AlocaCelula(void *x)
{
	TLista aux = (TLista) malloc(sizeof(Celula));
	if(!aux)
		return NULL;
	aux->info = x;
	aux->urm = NULL;
	return aux;
}

void AfisareLista(TLista L, void (*f)(void*, FILE*), FILE* out)
{
	if(!L)
		return;

	TLista santinela = L;
	for(L = L->urm; L != santinela; L = L->urm)
	{
		f(L->info, out);
		if(L->urm == santinela)
			fprintf(out, ".");
		else
			fprintf(out, "; ");
	}
}

void AfisareJucator(void *x, FILE* out)
{
	Jucator p = (Jucator) x;
	fprintf(out, "%s - %d", p->nume, p->nrMaini);
}

void InserareListaCirculara(ALista L, TLista x)
{
	TLista santinela = *L;
	TLista cursor = (*L)->urm;

	//Deplasare cursor la "finalul" listei
	while(cursor->urm != santinela)
		cursor = cursor->urm;

	//reface legaturile	
	x->urm = santinela;
	cursor->urm = x;
}

TLista InitializareListaCirculara()
{
	TLista santinela = (TLista) malloc(sizeof(Celula));
	if(!santinela)
		return NULL;
	santinela->info = NULL;
	santinela->urm = santinela;

	return santinela;
}

int CreeazaJucator(ALista Ljucatori, char* nume_Jucator, int nr_Maini)
{
	// Functia returneaza:
	// 	0->succes
	// 	1->nu a reusit alocarea structurii jucator
	// 	2->nu a reusit alocarea celulei

	//Creeaza structura pentru jucator
	Jucator structPlayer = (Jucator) malloc(sizeof(struct jucator));
	if(!structPlayer)
		return 1;
	structPlayer->nume = nume_Jucator;
	structPlayer->nrMaini = nr_Maini;

	//Creeaza celula pentru jucator
	TLista ptrPlayer = AlocaCelula(structPlayer);
	if(ptrPlayer == NULL)
		return 2;

	//Leaga celula in lista
	InserareListaCirculara(Ljucatori, ptrPlayer);
	return 0;
}

void InserareListaNormala(ALista L, TLista x)
{
	//Deplasare cursor la "finalul" listei
	while(*L)
		L = &(*L)->urm;
	//reface legaturile	
	*L = x;
}

int CreeazaMasa(ALista Lmese, char* nume_masa, TLista Ljucatori, int nrJuc, int NrMaxJuc)
{
	// Functia returneaza:
	// 	0->succes
	// 	1->nu a reusit alocarea structurii jucator
	// 	2->nu a reusit alocarea celulei

	Masa structMasa = (Masa) malloc(sizeof(struct masa));
	if(!structMasa)
		return 1;
	structMasa->numeMasa = nume_masa;
	structMasa->jucatori = Ljucatori;
	structMasa->nrCrtJucatori = nrJuc;
	structMasa->nrMaxJucatori = NrMaxJuc;

	//Creeaza celula pentru masa
	TLista ptrMasa = AlocaCelula(structMasa);
	if(ptrMasa == NULL)
		return 2;

	//Leaga celula in lista de mese;
	InserareListaNormala(Lmese, ptrMasa);
	return 0;
}

/*FUNCTIA PRINT*/
void print(Sala SalaJoc, FILE* out)
{
	TLista Lmese = SalaJoc->masa;
	int i;
	for(i = 0; i < SalaJoc->nrMese; ++i)
	{
		Masa masa = (Masa) Lmese->info;
		fprintf(out, "%s: ", masa->numeMasa);
		AfisareLista(masa->jucatori, AfisareJucator, out);
		if(i != SalaJoc->nrMese-1)
			fprintf(out, "\n");
		Lmese = Lmese->urm;
	}
		fprintf(out, "\n");
}

/*FUNCTIA NOROC*/
void noroc(Sala SalaJoc, FILE* out_file, char* nTable, char* nPlayer, int luck)
{
	if(luck <= 0)
		return;

	TLista ListaJuc = NULL;
	TLista ListaMese = SalaJoc->masa;
	TLista santinela = ListaMese;

	//Cauta masa in lista de mese
	while(ListaMese)
	{
		if(strcmp(((Masa)(ListaMese->info))->numeMasa, nTable) == 0)
		{
			ListaJuc = ((Masa)(ListaMese->info))->jucatori->urm;
			break;
		}
		else
			ListaMese = ListaMese->urm;
	}

	//Verifica daca s-a ajuns la sfarsitul listei fara sa gaseasca masa
	if(!ListaMese)
	{
		fprintf(out_file, "Masa %s nu exista!\n", nTable);
		return;
	}

	//Verifica daca s-a ajuns la sfarsitul listei fara sa gaseasca jucatorul
	if(ListaJuc == santinela)
	{
		fprintf(out_file, "Jucatorul %s nu exista la masa %s!\n", nPlayer, nTable);
		return;
	}

	while(ListaJuc != santinela)		
	{
		if(strcmp(((Jucator)(ListaJuc->info))->nume, nPlayer) == 0)
		{
			((Jucator)(ListaJuc->info))->nrMaini += luck;
			break;
		}
		else
			ListaJuc = ListaJuc->urm;
	}
}

/*FUNCTIA GHINION + APELURI NECESARE*/
void DistrugeJucator(Jucator strJuc)
{
	free(strJuc->nume);
	free(strJuc);
	strJuc = NULL;
}

void VerificaJucator(Sala SalaJoc, Masa strMasa)
{
	TLista santinela = strMasa->jucatori;
  TLista Ljuc = santinela->urm;
  TLista ant = santinela;
 
	//Cauta in toata lista de jucatori pe aceia cu nrMaini <= 0 si elimina
  while(Ljuc != santinela)
  {
	Jucator juc_crt = (Jucator)(Ljuc->info);
	if(juc_crt->nrMaini <= 0)
		{
	  //Scade jucatorul din evidenta
	  SalaJoc->nrLocCrt -= 1;
	  strMasa->nrCrtJucatori -= 1;
 
	  ant->urm = Ljuc->urm;
	  free(juc_crt->nume);
	  free(juc_crt);
	  free(Ljuc);
	  Ljuc = santinela;
	}
	ant = Ljuc;				//Avans
	Ljuc = Ljuc->urm;	//Avans
  }
}

void VerificaMese(Sala SalaJoc)
{
	TLista Lmese = SalaJoc->masa;
	TLista antM = NULL;
 
	while(Lmese)
	{	//Cauta masa fara jucatori
		Masa masa_crt = (Masa)(Lmese->info);
 
		//verific daca mai sunt juc la masa
		if(masa_crt->nrCrtJucatori == 0)
		{  
			if(antM)
				antM->urm = Lmese->urm;
			else	//Masa este prima in lista
				SalaJoc->masa = Lmese->urm;
				
			free(Lmese);
			//Scoate din evidenta masa 
			SalaJoc->nrMese -= 1;
		}
 
		antM = Lmese;				//Avans
		Lmese = Lmese->urm;	//Avans
	}
}
 
void VerificaSala(Sala sala)
{	//Verifica daca sala este goala pe baza nr de mese
  if(sala->nrMese == 0)
	free(sala);
}
 
void ghinion(FILE* out, Sala SalaJoc, char* Table, char* Player, int badluck)
{
   TLista Lmese = SalaJoc->masa;
   Masa strMasa = NULL;
 
  //Cauta masa tinta
  while(Lmese)
  {
	strMasa = (Masa)(Lmese->info);
	if(strcmp(Table, strMasa->numeMasa) == 0)
	  break;	//Masa gasita
	else
	  Lmese = Lmese->urm;	//Avans mese
  }
 
	TLista santinela = strMasa->jucatori;
  TLista Ljuc = santinela->urm;

  if(!Lmese)
  {
	fprintf(out, "Masa %s nu exista!\n", Table);
	return;
  }
 
  //Cauta jucatorul in masa gasita
  Jucator strJuc = NULL;
while(Ljuc != santinela)
{
	strJuc = (Jucator)(Ljuc->info);
	if(strcmp(Player, strJuc->nume) == 0)
	{
	  strJuc->nrMaini -= badluck;
	  break;
	}
	else
	  Ljuc = Ljuc->urm;
}
 
  if(Ljuc == santinela)
  {
	fprintf(out, "Jucatorul %s nu exista la masa %s!\n", Player, Table);
	return;
  }
 
  VerificaJucator(SalaJoc, strMasa);
  VerificaMese(SalaJoc);
  VerificaSala(SalaJoc);
}

/*FUNCTIA TURA + APELURI NECESARE*/

void Rotatie(TLista santinela)
{
	TLista prim = santinela->urm;
	TLista ult = prim;
	while(ult->urm != santinela)
		ult = ult->urm;	//Obtine ultimul jucator

	//Muta santinela dupa primul jucator
	ult->urm = prim;
	santinela->urm = prim->urm;
	prim->urm = santinela;
}

void Decrementare(TLista santinela)
{
	TLista Ljuc = santinela->urm;
	while(Ljuc != santinela)
	{
		((Jucator)(Ljuc -> info))->nrMaini -= 1;
		Ljuc = Ljuc->urm;
	}
}

void VerifM(Sala SalaJoc, Masa masaCrt)
{
	TLista santinela = masaCrt->jucatori;
	TLista Ljuc = santinela->urm;
	Jucator jucCrt = NULL;
	TLista ant = santinela;
	while(Ljuc != santinela)
	{	//cauta jucatorii cu nrMaini <= 0
		jucCrt = (Jucator)(Ljuc->info);
		if(jucCrt->nrMaini <= 0)
		{
			//Scade din evidenta jucatoru
			SalaJoc->nrLocCrt -= 1;
			masaCrt->nrCrtJucatori -= 1;

			//reface legaturile
			ant->urm = Ljuc->urm;
			DistrugeJucator(Ljuc->info);
			free(Ljuc);
			Ljuc = santinela;
		}

		ant = Ljuc;
		Ljuc = Ljuc->urm;
	}
}

void tura(FILE* out, Sala SalaJoc, char* nMasa)
{
	TLista Lmese = SalaJoc->masa;
	Masa masaCrt = NULL;
	TLista antMasa = SalaJoc->masa;
	while(Lmese)
	{
		masaCrt = (Masa)(Lmese->info);
		if(!strcmp(masaCrt->numeMasa, nMasa))
			break;
		antMasa = Lmese;
		Lmese = Lmese->urm;
	}// Am obtinut masa

	if(!Lmese){
		fprintf(out, "Masa %s nu exista!", nMasa);
		return;
	}

	TLista santinela = masaCrt->jucatori;
	TLista Ljuc = santinela->urm;
	Rotatie(santinela);
	Decrementare(santinela);
	while(Ljuc != santinela)
		Ljuc = Ljuc->urm;

	VerifM(SalaJoc, masaCrt);
	if(masaCrt->nrCrtJucatori == 0)
	{
		if(antMasa == SalaJoc->masa)
			SalaJoc->masa = Lmese->urm;
		else
			antMasa->urm = Lmese->urm;
		free(masaCrt->numeMasa);
		free(masaCrt);
		free(Lmese);
		SalaJoc->nrMese -= 1;
	}
}

/*FUNCTIA TURA COMPLETA*/
void tura_completa(FILE* out, Sala SalaJoc)
{
	TLista Lmese = SalaJoc->masa;
	Masa strMasa = NULL;
	while(Lmese)
	{
		strMasa = ((Masa)(Lmese->info));
		tura(out, SalaJoc, strMasa->numeMasa);
		Lmese = Lmese->urm;
	}
}

/*FUNCTIA CLASAMENT + APELURI NECESARE*/
void InserareOrdonata(TLista santinela, TLista celula)
{
	Jucator strJuc = ((Jucator)(celula->info));
	Jucator player = NULL;
	TLista Ljuc = santinela->urm;
	TLista ant = santinela;

	while(Ljuc != santinela)
	{
		player = ((Jucator)(Ljuc->info));
		if(player->nrMaini < strJuc->nrMaini)
			break;

		if(player->nrMaini == strJuc->nrMaini)
		{
			int rez = strcmp(strJuc->nume, player->nume);
			if(rez > 0)
			{
				celula->urm = Ljuc;
				ant->urm = celula;
				return;
			}
		}

		ant = Ljuc;
		Ljuc = Ljuc->urm;
	}
	celula->urm = ant->urm;
	ant->urm = celula;
}

void AfisareClasament(FILE* out, TLista santinela)
{
	TLista Ljuc = santinela->urm;
	while(Ljuc != santinela)
	{
		Jucator strJuc = (Jucator)(Ljuc->info);
		fprintf(out, "%s %d\n", strJuc->nume, strJuc->nrMaini);
		Ljuc = Ljuc->urm;
	}
}

void DistrugeListaCirculara(ALista L)
{
	TLista santinela = *L;
	TLista Ljuc = santinela->urm;
	TLista ant = santinela;
	while(Ljuc != santinela)
	{
		Jucator strJuc = (Jucator)(Ljuc->info);
		TLista aux = Ljuc;		//Retine celula
		ant->urm = Ljuc->urm; //Reface legaturile
		
		ant = Ljuc;				//Avans
		Ljuc = Ljuc->urm;	//Avans

		//Eliberari memorie
		free(strJuc->nume);
		free(strJuc);
		free(aux);
	}

	free(santinela);
	*L = NULL;
}

void clasament(FILE* out, Sala SalaJoc, char* Table)
{
	TLista Lmese = SalaJoc->masa;
	Masa strMasa = NULL;

	while(Lmese)
	{
		strMasa = ((Masa)(Lmese->info));
		if(strcmp(strMasa->numeMasa, Table) == 0)
			break;
		Lmese = Lmese->urm;
	}

	if(!Lmese)
	{
		fprintf(out, "Masa %s nu exista!\n", Table);
		return;
	}

	TLista santinela = strMasa->jucatori;
	TLista Ljuc = santinela->urm;

	//Initializare noua lista
	TLista ListaOrdonata = InitializareListaCirculara();
	if(!ListaOrdonata)
		return;

	while(Ljuc != santinela)
	{
		Jucator newJuc = malloc(sizeof(struct jucator));
		memcpy(newJuc, Ljuc->info, sizeof(struct jucator));
		TLista celula = AlocaCelula(newJuc);
		if(!celula)
			return;
		InserareOrdonata(ListaOrdonata, celula);
		Ljuc = Ljuc->urm;
	}

	fprintf(out, "Clasament %s:\n", Table);
	AfisareClasament(out, ListaOrdonata);

	//Elibereaza lista ordonata dupa ce afisezi
	DistrugeListaCirculara(&ListaOrdonata);
}

/*FUNCTIA INCHIDE*/
void inchide(FILE* out, Sala SalaJoc, char* Table)
{
	TLista closTBL = SalaJoc->masa;
	Masa mRedist = NULL;
 
	//Cauta masa tinta
	while(closTBL)
	{
		mRedist = (Masa)(closTBL->info);
		if(!strcmp(mRedist->numeMasa, Table))
			break;	//Masa gasita
		closTBL = closTBL->urm;
	}
 
 	//Verifica daca exista masa
	if(!closTBL)
	{
		fprintf(out, "Masa %s nu exista!\n", Table);
		return;
	}

	Masa strPrinc = (Masa)(closTBL->info);


	int factor1 = SalaJoc->nrLocMax - strPrinc->nrMaxJucatori;
	int factor2 = SalaJoc->nrLocCrt - strPrinc->nrCrtJucatori;

	int hm = factor1 - factor2;

 	//Verifica daca exista suficient loc in sala
	if(strPrinc->nrCrtJucatori > hm)
	{
		fprintf(out, "Nu exista suficiente locuri in sala!\n");
		return;
	}

	TLista thisTable = SalaJoc->masa;
	while(strPrinc->nrCrtJucatori > 0)
	{	//Cat timp mai sunt jucatori de redistribuit
		if(thisTable != closTBL)	//Sare peste masa tinta
		{	
			Masa strCrtMese = (Masa)(thisTable->info);
			while((strCrtMese->nrMaxJucatori != strCrtMese->nrCrtJucatori)
						 && (strPrinc->nrCrtJucatori > 0))
			{
				TLista juc = (strCrtMese->jucatori)->urm;
				TLista ant = NULL;

				while(juc != strCrtMese->jucatori)
				{
					ant = juc;
					juc = juc->urm;
				}

				TLista aux = (strPrinc->jucatori)->urm;
				ant->urm = aux;
				(strPrinc->jucatori)->urm = aux->urm;
				aux->urm = strCrtMese->jucatori;
				strCrtMese->nrCrtJucatori += 1;
				strPrinc->nrCrtJucatori -= 1;
			}
		}
		thisTable = thisTable->urm;	//Avans masa urmatoare
	}
	//Verifica daca vreo masa este goala
	//si o elimina din lista refacand legaturile
	VerificaMese(SalaJoc);
}

void DistrugeSala(Sala SalaJoc)
{
	TLista Lmese = SalaJoc->masa;
	while(Lmese)
	{	//Elibereaza memoria alocata pentru mese
		Masa m = (Masa)(Lmese->info);
		TLista santinela = m->jucatori;
		TLista Ljuc = santinela->urm;
		while(Ljuc != santinela)
		{	//Elibereaza memoria alocata pentru jucatori
			Jucator j = (Jucator)(Ljuc->info);
			free(j->nume);
			free(j);
			TLista auxJ = Ljuc;
			Ljuc = Ljuc->urm;
			free(auxJ);
		}
		free(santinela);
		free(m->numeMasa);
		TLista auxM = Lmese;
		Lmese = Lmese->urm;
		free(auxM);
	}
	free(SalaJoc);
}

int CitesteEvenimente(char* cmd)
{
	int rez = 0;

	if(strcmp(cmd, "print") == 0)
		rez = 1;
	if(strcmp(cmd, "noroc") == 0)
		rez = 2;
	if(strcmp(cmd, "ghinion") == 0)
		rez = 3;
	if(strcmp(cmd, "tura") == 0)
		rez = 4;
	if(strcmp(cmd, "tura_completa") == 0)
		rez = 5;
	if(strcmp(cmd, "clasament") == 0)
		rez = 6;
	if(strcmp(cmd, "inchide") == 0)
		rez = 7;

	return rez;
}

int main(int argc, char const *argv[])
{

	/*Se opreste executia programului daca nu se primesc parametrii*/
	if(argc < 3)
	{
		printf("Nu sunt suficienti parametri: (%s ", argv[0]);
		printf("configuratie.txt evenimente.txt out.txt)\n");
		return 1;
	}

	char *line = NULL;
	size_t len = MAX;

	/*Deschidere fisiere date ca argumente*/
	FILE *config_file = fopen(argv[config], "r");
	if(!config_file)
		return 2;

	FILE *event_file = fopen(argv[event], "r");
	if(!event_file)
		return 3;

	FILE *out_file = fopen(argv[output], "w");
	if(!out_file)
		return 4;

	/*Citire din configuratie.txt*/
	getline(&line, &len, config_file);	/*Citeste numarul de mese din SalaJoc*/
	int numar_mese = atoi(line);
	int i, j;

	//Alocare spatiu pentru SalaJoc
	Sala SalaJoc = (Sala)malloc(sizeof(struct sala));
	if(!SalaJoc)
		return 5;

	//Contorizare playeri
	int nr_juc_total = 0;
	int nr_juc_max = 0;

	TLista Lmese = NULL;	//Creare lista goala de mese
	int rez;

	/*Citeste mesele*/	
	for(i = 0; i < numar_mese; ++i)
	{
		getline(&line, &len, config_file);	//Obtine cate o linie din fisier

		//Desparte informatiile in variabile
		char *nMasa = NULL;
		int nr_jucatori, nr_max_jucatori;
		nMasa = strtok(line, DLM);

		//Aloca memorie pentru nume masa
		char* nume_masa = (char*)malloc(strlen(nMasa) + 1);
		if(!nume_masa)
		{
			DistrugeSala(SalaJoc);
			return -1;
		}

		memcpy(nume_masa, nMasa, strlen(nMasa));

		nr_jucatori = atoi(strtok(NULL, DLM));
		nr_max_jucatori = atoi(strtok(NULL, DLM));

		//Incrementare jucatori
		nr_juc_total += nr_jucatori;
		nr_juc_max += nr_max_jucatori;

		//Initializare lista vida de jucatori, cu santinela, pentru masa curenta
		TLista Ljucatori = InitializareListaCirculara();
		if(Ljucatori == NULL)
		{	
			printf("Eroare la initializarea listei circulare cu jucatori\n");
			DistrugeSala(SalaJoc);
			return 4;
		}

		//Obtine jucatorii de la fiecare masa
		for(j = 0; j < nr_jucatori; ++j)
		{
			getline(&line, &len, config_file);

			//Desparte informatiile in variabile
			char* player_Name = strtok(line, DLM);

			//Aloca memorie pentru nume jucator
			char* nume_Jucator = (char*)malloc(strlen(player_Name) + 1);
			if(!nume_Jucator)
			{
				printf("Nu s-a alocat numele\n");
				DistrugeSala(SalaJoc);
				return -1;
			}
			memcpy(nume_Jucator, player_Name, strlen(player_Name));

			int nr_Maini = atoi(strtok(NULL, DLM));

			//Leaga jucatorul curent in lista
			rez = CreeazaJucator(&Ljucatori, nume_Jucator, nr_Maini);
			if(rez != 0)
			{
				printf("Nu s-a creat jucatorul\n");
				DistrugeSala(SalaJoc);
				return -1;
			}
		}
		//Creeaza structura pentru masa si introduce informatiile
		rez = CreeazaMasa(&Lmese, nume_masa, Ljucatori, nr_jucatori, nr_max_jucatori);
		if(rez != 0)
		{
			printf("Nu s-a creat masa\n");
			DistrugeSala(SalaJoc);
			return -1;
		}		
	}
	
	//Scrie informatii in structura Sala
	SalaJoc->masa = Lmese;
	SalaJoc->nrMese = numar_mese;
	SalaJoc->nrLocCrt = nr_juc_total;
	SalaJoc->nrLocMax = nr_juc_max;

	//Executare evenimente
	char cmd[MAX], Table[MAX], Player[MAX];
	int number;
	while(fscanf(event_file, "%s", cmd) != EOF)
	{
		rez = CitesteEvenimente(cmd);
		switch(rez)
		{
			case 1:
				print(SalaJoc, out_file);
				break;
			case 2:
				fscanf(event_file, "%s", Table);
				fscanf(event_file, "%s", Player);	
				fscanf(event_file, "%d", &number);
				noroc(SalaJoc, out_file, Table, Player, number);
				break;
			case 3:
				fscanf(event_file, "%s", Table);
				fscanf(event_file, "%s", Player);	
				fscanf(event_file, "%d", &number);
				ghinion(out_file, SalaJoc, Table, Player, number);
				break;
			case 4:
				fscanf(event_file, "%s", Table);
				tura(out_file, SalaJoc, Table);
				break;
			case 5:
				tura_completa(out_file, SalaJoc);
				break;
			case 6:
				fscanf(event_file, "%s", Table);
				clasament(out_file, SalaJoc, Table);
				break;
			case 7:
				fscanf(event_file, "%s", Table);
				inchide(out_file, SalaJoc, Table);
				break;
			default:	
				break;
		}
	}
	/*Inchidere fisiere si eliberare memorie*/
	//Inchidere SalaJoc:
	DistrugeSala(SalaJoc);
	if(!SalaJoc)
		fprintf(out_file, "Sala inchisa\n");

	//Inchide fisierele date ca argumente
	fclose(config_file);
	fclose(event_file);
	fclose(out_file);
	return 0;
}