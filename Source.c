//Kod radjen u VisualStudio na Windows-u sa Windows header-ima. Nece raditi na Linux-u
#pragma warning (disable : 6031)  //potrebno jer izbacuje gresku "Return value ignored: 'fscanf' " ,razlog je kompajler verovatno
#pragma warning (disable : 4996)  //potrebno jer izbacuje gresku za fopen
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

#define N 20
#define M 40

int i, j, Field[N][M], x, y, Gy, Head, Tail, Game, Frogs, a, b, var, dir, score, HighScore, Speed;

FILE* f;        //fajl za cuvanje score-a

score = 0;      //cini se kao visak ali je potrebno za prvi upis score vrednosti u fajl (prvi sledeci kod ispod)
			    //takodje moramo opet score = 0 u f.-ji ispod da bi score vracao na 0 pri svakom novom igranju

void snakeInitialization() {
	f = fopen("highscore.txt", "w");
	fprintf(f, "%d", score);              //brise ono sto je prethodno bilo zapisano
	fclose(f);

	f = fopen("highscore.txt", "r");      //otvaramo fajl u read modu
	fscanf(f, "%d", &HighScore);          //trazi integer vrednost u otvorenom fajlu i smesta je u HighScore varijablu
	fclose(f);

	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++)
			Field[i][j] = 0;
	}

	x = N / 2, y = M / 2, Gy = y, Head = 5, Tail = 1, Game = 0, score = 0, Speed = 120;
	Frogs = 0;       //kada je Frogs = 0 menjamo poziciju zabe na ekranu
	dir = 'd';       //da bi zmijica pocela da se krece sama bez da ceka na pritisak tastera kada pokrenemo program prvi put

	for (i = 0; i < Head; i++) {      //postavljanje zmije na sredinu po x i y osi okrenutu na dole
		Gy++;
		Field[x][Gy - Head] = i + 1;
	}
}

void print() {
	for (i = 0; i <= M + 1; i++) {
		if (i == 0)
			printf("%c", 201);  //201 je decimalna vrednost za ASCII char znak za ivicu prozora igrice, analogno za ostale
		else if (i == M + 1)
			printf("%c", 187);
		else
			printf("%c", 205);
	}
	printf("   Current score: %d   HighScore: %d", score, HighScore);
	printf("\n");

	for (i = 0; i < N; i++) {
		printf("%c", 186);
		for (j = 0; j < M; j++) {
			if (Field[i][j] == 0)
				printf(" ");
			if (Field[i][j] > 0 && Field[i][j] != Head)
				printf("%c", 176);
			if (Field[i][j] == Head)
				printf("%c", 178);
			if (Field[i][j] == -1)      //polje ima zabu ako je -1
				printf("%c", 15);
			if (j == M - 1)
				printf("%c\n", 186);
		}
	}

	for (i = 0; i <= M + 1; i++) {
		if (i == 0)
			printf("%c", 200);
		else if (i == M + 1)
			printf("%c", 188);
		else
			printf("%c", 205);
	}
}

void ResetScreenPosition() {         //potrebna jer ce u suprotnom Game Loop ispisivati ceo sadrzaj igrice
	HANDLE hOut;                     //jedan ispod drugog beskonacno.Zato nam je potrebna da pri svakom
	COORD Position;					 //tom ispisu ovom funkcijom kursor vratimo na pocetak
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	Position.X = 0;
	Position.Y = 0;
	SetConsoleCursorPosition(hOut, Position);
}

void Random() {             //Za generisanje zabe
	srand(time(0));         //srand funkciji rand daje random pocetnu vrednost koju mi ovde definisemo funkcijom
					        //time koja uzima random momenat u vremenu komputacije racunara,za ovo nam treba time.h
					        //biblioteka
	a = rand() % 20;        //rand() generise broj od 0 do 19 ali nece menjati generisan broj pri svakom pozivu
					        //Random funkcije u while-u u main-u,zato koristimo srand
	b = rand() % 40;

	if (Frogs == 0 && Field[a][b] == 0) {
		Field[a][b] = -1;                    //polje ima vrednost -1 ako je zaba u njemu
		Frogs = 1;
		if (Speed > 10 && score != 0)        //ubrzavanje zmije kada pojede zabu
			Speed -= 15;						 
	}										 
}

int getch_noblock() {
	if (_kbhit())             //ako je uneseno nesto sa tastature,nece pauzirati program i cekati da se nesto
							  //unese sa tastature kao scanf
		return _getch();      //vrati ascii vrednost toga sto je uneseno
	else
		return -1;
}

void GameOver() {             //ova f.-ja mora biti definisana pre f.-je movement() posto je ona poziva
	printf("\a");             //za beep sound
	Sleep(1500);              //da se igracu da vremena da vidi sta se desilo
	system("Cls");            //brise ekran

	if (score > HighScore) {
		printf("  New HigScore: %d !!!!!!!\n\n",score);
		system("pause");                      //ceka da koristik pritisne taster da bi se nastavilo
		f = fopen("highscore.txt", "w");
		fprintf(f,"%d", score);               //brise ono sto je prethodno bilo zapisano
		fclose(f);
	}

	system("Cls");            //Moramo obrisati ekran pre pisanja sledeceg*/
	printf("\n\n         GAME OVER !!!!!!!\n\n");
	printf("         Score: % d \n\n", score);

	printf("         Press Enter key to play again or Esc to exit... \n");
	while (1) {
		var = getch_noblock();
		if (var == 13) {      //13 je ascii vrednost za Enter
			Game = 0;
			snakeInitialization();
			break;
		}
		else if (var == 27) {       //27 je ascii vrednost za Esc
			Game = 1;
			break;
		}
	}
	system("Cls");
}

void movement() {                  //ne koristimo scanf ili getcharacter funkcije jer one pauziraju program nakon
	var = getch_noblock();	       //sto pokupe vrednost sa tastature i drze ga pauziranog dok se ponovo ne 
							       //unese sa tastature
	var = tolower(var);	           //konvertuje vrednost var-a u mala slova da ne moramo da brinemo da li je
							       //CapsLock ukljucen ili ne , w a s d koristimo za kretanje zmijice
	if ((var == 'a' || var == 'w' || var == 's' || var == 'd') && (abs(dir - var) > 5))
		                           // && (abs(dir-var)>5)) uvodimo da zmija ne bi isla preko svog tela levo desno
		                   //gore dole kada za redom pritisnemo takvu dugmad,ascii razlika izmedju a i d kao i w i s je < 5
		dir = var;         //dir uvodimo jer se sa var samo pomeri za jedno mesto u zeljenom smeru ali ne nasta-
						   //vlja dalje sama u tom smeru nego se zaustavi
	if (dir == 'd') {
		y++;
		if (Field[x][y] != 0 && Field[x][y] != -1)
			GameOver();
		if (y == M)
			y = 0;
		if (Field[x][y] == -1) {
			Frogs = 0;
			score++;
			Tail -= 2;
		}
		Head++;
		Field[x][y] = Head;
	}

	if (dir == 'a') {
		y--;
		if (Field[x][y] != 0 && Field[x][y] != -1)
			GameOver();
		if (y == -1)
			y = M - 1;
		if (Field[x][y] == -1) {
			Frogs = 0;
			score++;
			Tail -= 2;
		}
		Head++;
		Field[x][y] = Head;
	}

	if (dir == 'w') {
		x--;
		if (Field[x][y] != 0 && Field[x][y] != -1)
			GameOver();
		if (x == -1)
			x = N - 1;
		if (Field[x][y] == -1) {
			Frogs = 0;
			score++;
			Tail -= 2;
		}
		Head++;
		Field[x][y] = Head;
	}

	if (dir == 's') {
		x++;
		if (Field[x][y] != 0 && Field[x][y] != -1)
			GameOver();
		if (x == N)
			x = 0;
		if (Field[x][y] == -1) {
			Frogs = 0;
			score++;
			Tail -= 2;
		}
		Head++;
		Field[x][y] = Head;
	}
}

void TailRemove() {
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			if (Field[i][j] == Tail)
				Field[i][j] = 0;
		}
	}
	Tail++;
}

void main() {
	snakeInitialization();

	while (Game == 0) {             //Ovo je nasa Game Loop koja updatuje promene tokom igranja
		print();
		ResetScreenPosition();
		Random();
		movement();
		TailRemove();
		Sleep(Speed);
	}
}
