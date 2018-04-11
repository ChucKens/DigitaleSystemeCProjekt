#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int *in;	/*0 Anzahl der Plattformen. 1  in.length /2*/
int cHistory = 0;
int** graphMat; //2D-Matrix mit allen Graphen 0 == kein Graph, 1 == ein Graph , 2 == zwei ...
int** usedMat; //2D-Matrix mit benutzen Graphen

int nOdd = -1;

 
FILE* inF;
int use;
int i;
int j;
int v = 0;
int c;
int fileSize;
int result;
char* buffer;

int parse(char* file){
	inF = fopen(file, "r");
	fseek(inF, 0, SEEK_END);
	fileSize = ftell(inF); 	//Aktuelle Stelle in der Datei
	rewind(inF); 			//zurueck zu Dateianfang
	buffer = malloc(fileSize);

	fread(buffer, sizeof buffer, fileSize ,inF);
	
	/**
	L:
		G:
		<num> 	-> 0,1,2,3,4,5,6,7,8,9,<num><num>. 	(int)buffer[i] >=48 && (int)buffer[i] <=57
		<ws> 		-> ' '. 							(int)buffer[i] ==32 
		<newln> -> \n. 								(int)buffer[i] ==10
		<1stln> -> <num><newln><edge>.
		<edge>	-> <num><ws><num><newln> | <num><ws><num><newln><edge>.
		state = '1' expecting a number.
		state = '2' expecting a number or a newline. 
		state = '3' expecting a	number.
		state = '4' expecting a number or whitespace.
		state = '5' expecting a number.
		state = '6' expecting a number or a newline.
		state = '7' expecting a edge or nothing.
		final state = '7'.
		Das Folgende stellt den Automaten dar, der die Sprache L akzeptiert
		Wenn er im Zustand 7 haelt dann wird die Eingabe akzeptiert,
		sonst bricht er ab, free'd den buffer und gibt -1 zurueck worauf hin das Programm mit "Ungueltiges Eingabeformat" haelt.
	*/
	char state = '1';
	for(i=0; i < fileSize; i++){		
		if(state == '7' && (int)buffer[i] >=48 && (int)buffer[i] <=57) state = '4'; 
		else if(state == '6' && (int)buffer[i] >=48 && (int)buffer[i] <=57);
		else if(state == '6' && (int)buffer[i] ==10) state = '7';
		else if(state == '5' && (int)buffer[i] >=48 && (int)buffer[i] <=57) state = '6';
		else if(state == '4' && (int)buffer[i] >=48 && (int)buffer[i] <=57);
		else if(state == '4' && (int)buffer[i] == 32) state = '5';
		else if(state == '3' && (int)buffer[i] >=48 && (int)buffer[i] <=57) state = '4';
		else if(state == '2' && (int)buffer[i] >=48 && (int)buffer[i] <=57);
		else if(state == '2' && (int)buffer[i] == 10) state = '3';
		else if(state == '1' && (int)buffer[i] >=48 && (int)buffer[i] <=57) state = '2';
		else goto fail; 		//Abbruch bei unerwartetem Zeichen
	}	
	if(state != '7') goto fail;	//Abbruch wenn der Automat nicht im Endzustand haelt 
	
	if(1 == 0){
		//aufraeumen und datei schliessen
		fail:
		rewind(inF);
		fclose(inF);
		free(buffer);
		return -1;
	}	
		
	c = 0;
	//zaehlt die newlines um die groeße fuer in[] zubestimmen
	for(i = 0; i < fileSize/(sizeof *buffer); i++)
		if(buffer[i] == '\n') c++;
		
	in = malloc(c*2*sizeof buffer);
	rewind(inF); //zurueck zu Dateinanfang


	i = 0;
	//scannt nach Integer dezimal zahlen im InputFile(inF) und fuegt sie an der richtigen stelle in in[] ein
	fscanf(inF, "%d", &in[i]);	/*write number of plattformes*/
	in[i+1] = c-1; 				/*write number of bridges*/
	i = 2;
	while(i < c*2){		
		fscanf(inF, "%d", &in[i++]);
		fscanf(inF, "%d", &in[i++]);		
	}

	free(buffer);
	rewind(inF);
	fclose(inF);
	return 1;
}
/*
 0 1 2 3 
0x
1x x
2x x x
3x x x x
*/
void toMat(){ //Liste der Graphen in Matrix umwandeln
	int x, y;
	for(i = 2; i < in[1]*2+2; i+=2){
		y = (in[i] < in[i+1]) ? in[i] : in[i+1]; //kleinerer Wert y		
		x = (in[i] > in[i+1]) ? in[i] : in[i+1]; //groesserer Wert x
		graphMat[x][y]++;
	}
}

//tested works
int isOdd(int num){ //gibt zurueck ob ein Knoten eine gerade oder eine ungerade Maechtigkeit hat 
	int counter = 0; 	
	int x,y;	
	for(y = 0; y < in[0]; y++)
		for(x = 0; x <= y; x++)
			if((y == num || x == num) && graphMat[y][x]%2 == 1 && x!=y)
				counter++;
	if(counter%2 == 1) return 1;
	else return 0;

}
//tested works
int checkUneven(){ //zaehlt die Graphen mit ungerader Maechtigkeit
	int ungerade = 0;
	for(i = 0; i < in[0]; i++){
		if(isOdd(i) ==1) ungerade++;
	}
	return ungerade;
}
//tested works
int findOdd(int next){//gibt den naechten Knoten mit ungerade Maechtigkeit zurueck, wenn next erhoeht ueberspringt es den naechsten Knoten
	int n = 0;
	int s;
	for(s = 0; s < in[0]; s++){
		if(isOdd(s)==1 && n < next)n++;
		else if(isOdd(s)==1)return s;	
	}	
}
void freeGraphMat(){ //free'd graphMat
	for(i=0; i < in[0]; i++)free(graphMat[i]);
	free(graphMat);
}
void freeUsedMat(){ //free'd usedMat
	for(i=0; i < in[0]; i++)free(usedMat[i]);
	free(usedMat);

}
/*
	kontrolliert ob es doch einen Knoten gibt der groesser ist als oben im InputFile angegeben 
	 oder ob es einen gibt der nicht so gross ist 
*/
int checkFile(){ 
	int max = 0;
	for(i = 2; i < in[1]*2+2; i++){
			if(in[i] > max) max = in[i];
			if(in[i] >= in[0])return -1;
	}
	if(max > in[0]) return -1;
	return 1;
}

int getNext(int s, int skip, int c_skip,int write){//gibt den naechsten Nachbarn aus, wenn write=1 gesetzt ist wird er als benutzt markiert
	for(i=0; i < in[0]; i++){
		for(j=0; j <= i; j++){
			if(usedMat[i][j] < graphMat[i][j] && c_skip < skip){
				c_skip++;
			}
			else if(usedMat[i][j] < graphMat[i][j] && j == s ){
				if(write > 0)usedMat[i][j]++;				
				return i;
			}
			else if(usedMat[i][j] < graphMat[i][j] && i == s){
				if(write > 0)usedMat[i][j]++;
				return j;
			}
		}
	}
	return -1;

}



int solve(int numPlattform, int cPlat, int history[in[1]][2], int path[in[1]]){
	/*
		Wenn der counter fuer History 0 ist, 
		dann wird der aktuelle Knoten ausgegeben und die Funktion terminiert.
	*/
	if(getNext(cPlat, 0, 0, 0) == -1 && cHistory == 0 && numPlattform == in[1]){ 
		//printf("%d",cPlat);
		path[v++] = cPlat;
		return 1;
	}
	/*
		Wenn es nicht weiter geht, wird abgebrochen, denn die Graphen sind nicht verbunden
	*/
	else if(getNext(cPlat, 0, 0, 0) == -1 && cHistory == 0 ){ 
		return -1;
	}
	/*
		Wenn es von dem aktuellen Knoten keine weitere Plattform mehr gibt 
		aber History nicht bei 0 ist dann wird der aktuelle Knoten in History geschrieben. 
		Und der naechste anliegende Knoten wird der naechste aktuelle Knoten.
	*/
	else if(getNext(cPlat, 0, 0, 0) != -1){
		int next = getNext(cPlat, 0, 0, 1);
		history[cHistory][0] = cPlat;
		cHistory++;
		if(solve(numPlattform, next, history, path) == 1) return 1;
		else return -1;
	}
	/*
		Wenn dies nicht der fall ist und noch etwas in History drin steht,
		wird der aktuelle Knoten ausgegeben, 
		dann wird der letze Knoten aus History genommen
		und wird der neue aktuelle Knoten
	*/
	else if(cHistory > 0){
		//printf("%d ",cPlat);
		path[v++] = cPlat;
		cHistory--;
		int next = history[cHistory][0];
		if(solve(numPlattform+1, next, history, path) == 1) return 1;
		else return -1;
	}
	/*
		Wenn das alles nicht der Fall ist, 
		dann wird die aktuelle Platform ausgegeben 
		und der Letzte Knoten auf History wird zum aktuellen Knoten. 
	*/
	else{
		//printf("%d ",cPlat);
		path[v++] = cPlat;
		cHistory--;
		int next = history[cHistory][0];
		if(solve(numPlattform+1, next, history, path) == 1) return 1;
		else return -1;
	}
}
//Gibt das Array mit dem Weg aus
void printResult(int r[in[1]]){
	for(i = 0; i < v; i++){ //in[1]
		printf("%d ", r[i]);
	}
	printf("\n");
}
int main(int argc, char* argv[]){
	if(parse(argv[1]) == -1){ //in[] fuellen
		free(in);
		free(graphMat);
		free(usedMat);
		printf("Ungueltiges Eingabeformat\n");
		return -1;
	} 
	if(checkFile()== -1){	//gucken ob in[0] stimmt
		free(in);
		free(graphMat);
		free(usedMat);
		printf("Ungueltiges Eingabeformat\n");
		return -1;	
	}
	//Speicher fuer die GraphenMatrix(in[0]x[0]) besorgen
	graphMat = (int**)malloc((sizeof(int*)) * (in[0]));
	for(i=0; i < in[0]; i++)graphMat[i] = (int*)malloc((sizeof(int)) * (in[0]));// (in[0]));
	for(i=0; i < in[0]; i++)for(j=0; j < in[0]; j++)graphMat[i][j] = 0; //alles auf null setzen
	toMat();
	//Das gleiche fuer die UsedMatrix(in[1]x in[1])
	usedMat = (int**)malloc((sizeof(int*)) * (in[0]));
	for(i=0; i < in[0]; i++)usedMat[i] = (int*)malloc((sizeof(int)) * (in[0])); //TODO i+1
	for(i=0; i < in[0]; i++)for(j=0; j < in[0]; j++)usedMat[i][j] = 0; //alles auf null setzen
	
	/*
		funktioniert aufgrund von boesem C-Voodoo-Zauber nicht,
		deshalb wird jeder funktion der lokale pointer zu history mitgegeben.
	*/

	int history[in[1]][2];
	for(i = 0; i < in[1]; i++)for(j = 0; j < 2; j++)history[i][j] = -1; //mit -1 initialisieren
	int path[in[1]];
	for(i = 0; i < in[1]; i++)path[i]=-1;
	int even = checkUneven();
	
	/*
		Wenn es mehr als 2 ungerade Knoten gibt dann gibt es
		keinen Eulerpfad und das Programm wird beendet
	*/
	if(even > 2){ 
		freeGraphMat();
		freeUsedMat();
		free(in);
		printf("-1\n");
		return -1;	
	}
	/*
		Wenn es einen oder zwei ungerade Knoten gibt
		dann wird bei einem der ungeraden Knoten gestartet
	*/
	else if(even > 0){
		if(solve(0,findOdd(0), history, path) == 1){
			freeGraphMat();
			freeUsedMat();	
			free(in);
			printResult(path);
			//printf("\n");
			return 0;
		}
		else{
			printf("-1\n");
		}
	}
	// wenn alle Knoten gerade sind dann ist der start egal
	else{
		if(solve(0,0, history, path) == 1){
			freeGraphMat();
			freeUsedMat();				
			free(in);
			printResult(path);
			//printf("\n");
			return 0;
		}
		else printf("-1\n");
	}
	freeGraphMat();
	freeUsedMat();	
	free(in);
	return -1;
}
