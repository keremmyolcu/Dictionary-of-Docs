#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>

#define M 997	//hashtable size icin 1000e en yakin asal sayi secildi
#define MM 996
#define MAX_WORD_LENGTH 25
#define BUFFER_SIZE 150

typedef struct dosya {
    char dosyaAdi[MAX_WORD_LENGTH];
    struct dosya *next;
} dosya;

typedef struct Kelime{
	char word[MAX_WORD_LENGTH];		//key olarak
	dosya *dosyaRoot; //ilgili kelimenin icinde oldugu dosyalari tutan linked list yapisi
} Kelime;

FILE *openFile(char *fileName);
//Verilen isimdeki bir dosya ayni klasorde varsa file pointeri return et, yoksa null dondur
int searchHash(char *word,Kelime *hashTable[]);
//Kelime hashtable'da varsa indexini yoksa 0 dondur
int hornerMethod(char *in);
//Verilen bir string degerinin hashing icin key degerini cikartir
int loadFile(char *dosyaAdi,Kelime *hashTable[]);
//Istenilen txt'nin icindeki kelimeleri hashtable'a ekle
int hashCode(int key, int i);
//Verilen kelimenin key degerini alip indexini double-hashing ile return eder
int hash1(int key);
int hash2(int key);
//Double hashing icin fonksiyonlar
void listDocuments(char *word,Kelime *hashTable[]);
//Kullanicidan alinan kelime eger hashtable'da mevcut ise hangi dosyalarda geciyor ekrana bastirir
void listLinkedList(dosya *root);
void pushEnd(dosya *ekle,dosya **root);
//Ilgili dosyayi dosyalar linked listinin sonuna ekler
int searchLL(dosya *root,char *dosyaAdi);
//Bir dosya, dosyalar linked listinde geciyorsa 1 gecmiyorsa 0 dondurur
float loadFactor(Kelime *hashTable[]);
//Hashtable'in doluluk oranini return eder
int insertKelime(char *word,Kelime *hashTable[]);
//dictionary'den veri cekerken kullanilacak
int loadWordUp(char *word,Kelime *hashTable[],char *dosyaAdi);
//dosyadan okunan kelimeyi eger yoksa HT'ye ekle, varsa ilgili dokümaný kelimenin dosyalar LL'sine ekler
//kelime ilgili dosyada daha once gectiyse hicbir sey yapmaz
float loadDict(Kelime *hashTable[]);
//Program acildiginda dictionary.txt dosyasindan HT'ye veri ceker, loadFactoru dondurur
int htToDict(Kelime *hashTable[]);
//Program kapanirken guncel HT'deki verileri dictionary.txt'ye yazacak
void dosyaEkle(char *dosyaAdi, Kelime* kelime);
//Kelimenin dosyalar LL'sinde ilgili dosya yoksa LL'ye dosya ismini ekleme islemi, varsa bir sey yapmaz
char *convertToLower(char *word);
//Collision'u engellemek icin okunulan kelimeyi tamamen lowercase hale getirip oyle hashcode'unu cikaracagiz

int main(){
	int i;	
	Kelime *hashTable[M];
	char inputDosya[MAX_WORD_LENGTH],inword[MAX_WORD_LENGTH];
	
	for(i=0; i<M; i++){
		hashTable[i] = (Kelime*)malloc(sizeof(Kelime));
		strcpy(hashTable[i]->word,"bosbos");		//hashTable initializing
		hashTable[i]->dosyaRoot = NULL; 		//bosbos stringi ilgili kýsma herhangi bir kelime eklenmedigini
	} 											//belirtiyor
	
	//loadWordUp("denemebir",hashTable,"denemebir.txt");
																//ilk kullanimda dictionary'nin yuklenmeden once
	//htToDict(hashTable);										//initiliaze edilmesi icin deneme kelimesi kullaniliyor
																//ilk kullanimdan sonra commentlenebilir
																
	loadDict(hashTable);
	printf("Yuklemek istediginiz dosyanin ismini sonuna .txt koyarak yaziniz:\n");
	scanf("%s",inputDosya);
	loadFile(inputDosya,hashTable);	//kullanicinin girdigi input dosyasi hashTable'a yuklendi
	printf("Bilgi almak istediginiz kelimeyi giriniz:\n");
	scanf("%s",inword);
	
	listDocuments(inword,hashTable);	
			
	htToDict(hashTable);	//yapilan degisikliklerle beraber yeni hashtable'i dictionary.txt'ye yukle 
	return 0;
}


FILE *openFile(char *fileName){
	FILE* fp;
	fp = fopen(fileName,"r");
	if(fp != NULL){
		return fp;
	}
	else{
		return NULL;
	}
}
char *convertToLower(char *word){
	int i,n;
	char *ret;
	n =strlen(word);
	ret = (char*)malloc(n*sizeof(char));	
	for(i=0; i<n+1; i++){
		ret[i] = tolower(word[i]);
	}	
	return ret;
}
int hornerMethod(char *in){
	int i,hash=0;
	int n = strlen(in);
	char *word = convertToLower(in);
	for(i=0; i<n; i++){
		hash = (2*hash + (word[i]-'A'+1));
	}
	return hash % M;
}
int hash1(int key){
	return key % M;
}
int hash2(int key){
	return 1 + (key % MM);
}
int hashCode(int key, int i){
	int ret;
	ret=(hash1(key) + i*hash2(key))%M;
	return ret;
} 
void pushEnd(dosya *ekle,dosya **root){
	dosya *cur = *root;	
	while(cur->next != NULL){
		cur = cur->next;
	}
	cur->next = ekle;
	cur->next->next = NULL;
	
}
void dosyaEkle(char *dosyaAdi, Kelime* kelime){
	dosya *ekle;
	dosya *curr = kelime->dosyaRoot;
	if(curr == NULL){
		kelime->dosyaRoot = (dosya*)malloc(sizeof(dosya));
		if(kelime->dosyaRoot == NULL){
			printf("Couldnt allocate memory");
			return;
		}
		strcpy(kelime->dosyaRoot->dosyaAdi,dosyaAdi);
		kelime->dosyaRoot->next = NULL;
	}
	else
	{	
		ekle = (dosya*)malloc(sizeof(dosya));
		strcpy(ekle->dosyaAdi,dosyaAdi);
		pushEnd(ekle,&(kelime->dosyaRoot));
	}
}
int searchHash(char *word,Kelime *hashTable[]){
	int key = hornerMethod(word);
	int i = 0;
	while(i<M && strcmp(hashTable[hashCode(key,i)]->word,"bosbos") != 0){
		if(strcmp(hashTable[hashCode(key,i)]->word,word) == 0){
			return hashCode(key,i);
		}					
		i++;
	} 
	//printf("Hashte bulunamadi \n");
	return -1;
}
int insertKelime(char *word,Kelime *hashTable[]){
	int key,adr,i=0;
	key = hornerMethod(word);
	adr = hashCode(key,i);
	while(i<M && (strcmp(hashTable[adr]->word,"bosbos") != 0) && (strcmp(hashTable[adr]->word,word) != 0)){
		++i;
		adr = hashCode(key,i);
	}
	char *eklencek;
	eklencek = convertToLower(word);
	strcpy(hashTable[adr]->word,eklencek);
	return adr;
}
void listLinkedList(dosya *root){
	dosya *cur = root;
	while(cur != NULL){
		printf("%s ",cur->dosyaAdi);
		cur = cur->next;
	}
}
int searchLL(dosya *root,char *dosyaName){
	//llde varsa 1 yoksa 0 dondur
		dosya *cur = root;
		//cur = (dosya*)malloc(sizeof(dosya));
		while(cur != NULL && strcmp(cur->dosyaAdi,dosyaName) != 0){
			cur = cur->next;		
		}
		if(cur != NULL && strcmp(cur->dosyaAdi,dosyaName) == 0){
			//printf("BULDU\n");
			return 1;
		}
		else{
		//printf("BULAMADI\n");
		return 0;	
		}							
}
void listDocuments(char *word, Kelime *hashTable[]){
	int i = searchHash(word,hashTable);
	int key = hornerMethod(word);
	if(i != -1){
		listLinkedList(hashTable[i]->dosyaRoot);
		printf("\n");
	}
	else{
		printf("Boyle bir kelime mevcut degil \n");
	}
}
float loadFactor(Kelime *hashTable[]){
	int i,n=0;
	float ret;
	for(i=0; i<M; i++){
		if(strcmp(hashTable[i]->word,"bosbos") != 0){
			n++;
		}			
	}
	ret = (float)n/(float)M;
	return ret ;
}
int loadWordUp(char *word,Kelime *hashTable[], char *dosyaadi){
	int key,adr,i=0;
	key = hornerMethod(word);
	adr = hashCode(key,i);
	char *eklencek;
	eklencek = convertToLower(word);
	while(i<M && (strcmp(hashTable[adr]->word,"bosbos") != 0) && (strcmp(hashTable[adr]->word,eklencek) != 0)){
		++i;
		adr = hashCode(key,i);
	}
	if(i == M){
		if(searchHash(eklencek,hashTable) != 1){
			printf("Dosya doldu,eklenemeyen kelime : %s\n",word);	//dosya dolu ve kelime hash'te yok ise bastir
		}	
		return 0;
	}
	else{
		if(strcmp(hashTable[adr]->word,"bosbos") == 0){		//bos yer varsa ekle
			if(loadFactor(hashTable) > 0.8){
				printf("Tablo dolmak uzere ! ");
			}
			strcpy(hashTable[adr]->word,eklencek);
			dosyaEkle(dosyaadi,hashTable[adr]);
			printf("%s kelimesi eklenildi. Load Factor: %.4f\n",word,loadFactor(hashTable));
			return 1;
		}
		else{
			//printf("Zaten ekli \n");
			if(searchLL(hashTable[adr]->dosyaRoot,dosyaadi) == 0){	//zaten ekli kelime fakat kelime ilk defa bu dosyada geciyor
				//printf("Kelimeye ilgili dosya eklenildi\n");
				dosyaEkle(dosyaadi,hashTable[adr]);
			}
			else{
				//kelime de ekli, kelimede ilgili dosya da ekli, bir sey yapma
			}							
			return 1;
		}
	}
}
int loadFile(char *dosyaAdi, Kelime *hashTable[]){
	FILE *fp1;
	fp1 = openFile(dosyaAdi);
	
	if(fp1 != NULL){
		char *word;
		char line[BUFFER_SIZE];
		while(fgets(line,149,fp1) != NULL){		//her line'i kelimelerine bol ve her kelimeyi loadWordUp'a at
			word = strtok(line," \n");			
			while(word != NULL){
 				loadWordUp(word,hashTable,dosyaAdi);
   	 			word = strtok (NULL," \n");
    		} 
		}
		fclose(fp1);			
		return 1;
	}
	
	else{
		printf("Dosya yuklenemedi");
		return 0;
	}	
}
int htToDict(Kelime *hashTable[]){
	FILE *outputfp = NULL;
	int i =0;
	outputfp = fopen("dictionary.txt","w");
	if(outputfp != NULL){
		fprintf(outputfp,"%f\n",loadFactor(hashTable));			//dosyanin basina once loadfactoru bas
		for(i=0; i<M; i++){										//ht deki her eleman icin
			if(strcmp(hashTable[i]->word,"bosbos") != 0){		//eger eleman doluysa
				fprintf(outputfp,"%s",hashTable[i]->word);		//once kelimenin ismini bastýr 
				dosya *cur = hashTable[i]->dosyaRoot;
				while(cur != NULL){
					fprintf(outputfp," %s",cur->dosyaAdi);		//daha sonra icinde bulundugu dosyalari yanina bastir
					cur = cur->next;
				}
				fprintf(outputfp,"\n");							//bir satir asagi in
			}
		}
		fclose(outputfp);
		printf("Dictionary'e yazim islemi tamamlandi\n");
		return 1;
	}
	else{
		printf("Dictionary'e yazdirilamadi\n");
		return 0;
	}
}
float loadDict(Kelime *hashTable[]){
	FILE *dictfp;
	float lf;
	int adr;
	char lineBuffer[BUFFER_SIZE];
	char *word;
	dictfp = fopen("dictionary.txt","r");
	if(dictfp != NULL){
		fscanf(dictfp,"%f\n",&lf);				//loadfactoru oku
		//printf("%f\n",lf);
		while(fgets(lineBuffer,149,dictfp) != NULL){		
			word = strtok(lineBuffer," \n");
			adr = insertKelime(word,hashTable);		//satirdaki ilk kelime her zaman Kelimemizdir
			//printf("%d\n",adr);	
			while(word != NULL){				//daha sonra while icinde dosyalari okuyup kelimeye atacagiz
				//printf("%s\n",word);
				if(strcmp(hashTable[adr]->word,word) != 0 && strcmp(hashTable[adr]->word,convertToLower(word)) != 0){
					dosyaEkle(word,hashTable[adr]);
				}			
   	 			word = strtok (NULL," \n");
    		} 
		}
	return lf;
	fclose(dictfp);
	}
	else{
		printf("Dictionary yuklenemedi");
	}
}




