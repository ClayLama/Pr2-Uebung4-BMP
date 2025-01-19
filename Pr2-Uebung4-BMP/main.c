/*
Mehr Informationen finden Sie zum Beispiel unter
https://de.wikipedia.org/wiki/Windows_Bitmap
https://en.wikipedia.org/wiki/BMP_file_format

Bild in BMP-Datei (24 Bit) konvertieren zum Beispiel mit
https://www.freeconvert.com/jpg-to-bmp
https://convertio.co/de/jpg-bmp/
https://picflow.com/convert/jpg-to-bmp
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 
#include <stdint.h>
#include <string.h>


//Makros
#define MAXSTRING 50


//Definition Datentypen
//Dateikopf wie definiert in
//https://de.wikipedia.org/wiki/Windows_Bitmap
typedef struct sBFHeader {
	char bfType[2];				//2 Bytes
	uint32_t bfSize;			//4 Bytes
	uint32_t bfReserved;		//4 Bytes
	uint32_t bfOffBits;			//4 Bytes
} sBFHeader;

//Informationsblock wie definiert in
//https://de.wikipedia.org/wiki/Windows_Bitmap
typedef struct sBFInfoHeader {
	uint32_t biSize;			//4 Bytes
	int32_t biWidth;			//4 Bytes
	int32_t biHeight;			//4 Bytes
	uint16_t biPlanes;			//2 Bytes
	uint16_t biBitCount;		//2 Bytes
	uint32_t biCompression;		//4 Bytes
	uint32_t biSizeImage;		//4 Bytes
	int32_t biXPelsPerMeter;	//4 Bytes, wird hier nicht verwendet, nur zur Vollst ndigkeit
	int32_t biYPelsPerMeter;	//4 Bytes, wird hier nicht verwendet, nur zur Vollst ndigkeit
	uint32_t biClrUsed;			//4 Bytes, wird hier nicht verwendet, nur zur Vollst ndigkeit
	uint32_t biClrImportant;	//4 Bytes, wird hier nicht verwendet, nur zur Vollst ndigkeit
} sBFInfoHeader;

//RGB-Farbschema
typedef struct sRGB {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
} sRGB;

//Bilddaten wie definiert in
//https://de.wikipedia.org/wiki/Windows_Bitmap
typedef struct sImage {
	int height;
	int width;
	struct sRGB** rgb;
} sImage;


// Funktionsprototypen
int bmpProcessing(int filter, int effect, char dateiName[]);
int getPositiveInt();

//Funktionen f r Farbfilter (drei Beispiele)
void filterGray(struct sImage image);
void filterSepia(struct sImage image);
void filterNegative(struct sImage image);

//Funktionen f r Effekte (zwei Beispiele)
void effectBlurred(struct sImage image);
void effectScreen(struct sImage image);

//Hauptprogramm
int main(int argc, char* argv[]) {

	char fileName[MAXSTRING] = { 0 };

	if (argc != 2) {
		printf("F gen Sie ein Dateiname als Argument hinzu");
		return -1;
	}

	//TODO
	//Pr fung der Kommandozeilenparameter
	// bernahme des Dateinamen in Variable dateiName
	strncpy(fileName, argv[1], MAXSTRING - 1);
	fileName[MAXSTRING - 1] = '\0';


	printf("Herzlich willkommen zur Bearbeitung von BMP-Datein\n");
	printf("Gueltiges Format: 24-Bit Bitmap (nicht komprimiert)\n");
	printf("Rufen Sie das Programm mit dem Bildnamen als Argument auf.\n\n\n");
	printf("Folgende Filter und Effekte stehen zur Verfuegung:\n");
	printf("Filter:\n 1: Kein Filter\n 2: Graustufen\n 3: Sepia\n 4: Komplementaer\n");
	printf("Effekte:\n 1: Kein Effekt\n 2: Verschwommen\n 3: Leinwand\n");
	printf("\nUm einen Filter/Effekt auszuwaehlen, tippen Sie die davorstehende Nummer ein.\n\n");

	//Filterauswahl
	printf("Waehlen Sie einen Filter: ");
	int filter = 0;
	filter = getPositiveInt();

	//Effektauswahl
	printf("Waehlen Sie einen Effekt: ");
	int effect = 0;
	effect = getPositiveInt();

	//Bildverarbeitung starten
	bmpProcessing(filter, effect, fileName);

	printf("Die Bildbearbeitung wurde erfolgreich durchgefuehrt.\n\n");
	printf("Das Ergebnis wurde in der Datei \"Ergebnis.bmp\" wurde im Repository erstellt\n\n\n\n\n");

	return 0;
}


/*
Ziel: Die Funktion wendet einen vom Benutzer ausgew hlten Filter und einen vom Benutzer ausgew hlten
Effekt auf ein Bild im BMP-Format an.
Das Ergebnis wird in der Datei "Ergebnis.bmp" gespeichert.

Falls ein Fehler auftritt, zum Beispiel die BMP-Datei nicht existiert oder im falschen Format vorliegt,
wird das Programm sofort beendet.

Parameter:
- filter: Vom Benutzer ausgew hlter Filter
- effect: Vom Benutzer ausgew hlter Effekt
- fileName: Dateiname des Bilds, das ver ndert werden soll.
*/
int bmpProcessing(int filter, int effect, char fileName[]) {

	//TODO
	//Datei im Bin rmodul zum Lesen  ffnen
	FILE* fp = fopen(fileName, "rb");
	if (!fp) {
		printf("Fehler beim  ffnen der Datei\n");
		exit(-1);
	}


	//Definieren der Variablen
	sBFHeader header;
	sBFInfoHeader infoHeader;
	sImage image;

	//Einlesen Dateikopf (Variable header)
	//Da nur 2 Character im BMP-Dateiformat als bgType erlaubt sind, 
	//C aber eigentlich ein Stringendezeichen (\0) bei Zeichenketten ben tigt,
	//wird das Einlesen der Datei auf zwei fread-Statements aufgeteilt
	//1. Schritt - Die ersten beiden Buchstaben einlesen
	fread(header.bfType, 1, 2, fp);
	//2. Schritt - Die restlichen 12 Byte vom Dateikopf einlesen
	fread(&header.bfSize, sizeof(uint32_t), 3, fp);

	//TODO
	//Einlesen Informationsblock (Variable infoHeader) mit sizeof(sBFInfoHeader) Byte
	fread(&infoHeader, sizeof(sBFInfoHeader), 1, fp);

	//TODO
	//Fehlerabfragen
	// 1. Ist Datei im BMP-Format (ersten beiden Buchstaben im Feld bfType von Variable header m ssen Werte 'B' und 'M' besitzen)? 
	// 2. Hat der Informationsblock (Feld biSize von Variable infoHeader) die richtige Gr  e (sizeof(sBFInfoHeader))?
	// 3. Ist Datei im 24-Bit Format (Feld biBitCount von Variable infoHeader gleich 24)? 
	// 4. Ist Datei nicht komprimiert (Feld biCompression von Variable infoHeader gleich 0)?
	//Falls ein Fehler vorliegt / auftritt, wird das Programm sofort mit einer entsprechenden Fehlermeldung beendet.
	if (header.bfType[0] != 'B' || header.bfType[1] != 'M')
	{
		printf("Die Datei ist kein BMP-Format");
		fclose(fp);
		return -1;
	}

	printf("%d\n", (int)sizeof(sBFInfoHeader));
	printf("%d\n", infoHeader.biSize);


	if (infoHeader.biSize != 40 && infoHeader.biSize != 108 && infoHeader.biSize != 124) {
		printf("Error: Groesse des Informationsblocks ist ungueltig. \n");
		fclose(fp);
		exit(1);
	}

	if (infoHeader.biBitCount != 24 || infoHeader.biCompression != 0) {
		printf("Bitte nur Dateien im 24-Bit Format ohne Kompression");
		fclose(fp);
		return -1;
	}


	//TODO
	//Dateizeiger fp auf Offset (bfOffBits von der Variablen header) setzen 
	fseek(fp, header.bfOffBits, SEEK_SET);



	//H he und Breite aus dem Informationsblock in Bilddaten  bernehmen
	image.height = infoHeader.biHeight;
	image.width = infoHeader.biWidth;

	//Neues Bild anlegen
	//Hierzu Speicher im Heap anlegen und Bild einlesen
	//Berechnung der Anzahl an Pixel pro Bildzeile
	//Siehe https://en.wikipedia.org/wiki/BMP_file_format
	//Anzahl an Bytes pro Bildzeile, Abrundung beim Teilen mit 32 gewollt
	int rowSize = (sizeof(sRGB) * 8 * image.width + 31) / 8;

	//Zeigervektor f r zur Speicherung der einzelnen Bildzeilen anlegen 
	// (entspricht sozusagen der H he (height) des Bildes) 
	image.rgb = (struct sRGB**)malloc(image.height * sizeof(void*));
	//TODO
	//Fehlerabfrage, ob das Anlegen des Speichers geklappt hat
	if (!image.rgb) {
		printf("Fehler bei Speicherzuweisung");
		fclose(fp);
		return -1;
	}


	//In BMP-Dateien werden die Pixel von unten nach oben gespeichert
	//Siehe https://en.wikipedia.org/wiki/BMP_file_format -> "Usually pixels are stored "bottom-up"
	//Also muss die Schleife in der untersten Zeile beginnen
	for (int i = (image.height - 1); i >= 0; i--) {
		//Speicher f r jede Bildzeile anlegen und auslesen
		image.rgb[i] = (sRGB*)malloc(rowSize);
		if (image.rgb[i] != 0)
			fread(image.rgb[i], rowSize, 1, fp);
		else {
			printf("Error: Kein Speicher vorhanden.\n");
			fclose(fp);
			exit(-1);
		}
	}

	//TODO
	//Datei schliessen
	fclose(fp);

	//Filter auf Bild anwenden
	switch (filter) {
	case 1:
		printf("Kein Filter\n");
		break;
	case 2:
		filterGray(image);
		break;
	case 3:
		filterSepia(image);
		break;
	case 4:
		filterNegative(image);
		break;
	default:
		printf("Error: Ungueltiger Filter\n");
		break;
	}

	//Effekt auf Bild anwenden
	switch (effect) {
	case 1:
		printf("Kein Effekt\n");
		break;
	case 2:
		effectBlurred(image);
		break;
	case 3:
		effectScreen(image);
	default:
		printf("Error: Ungueltiger Effekt\n");
		break;
	}

	//TODO
	//Zeiger fpw zur Ergebnisdatei "Ergebnis.bmp" im Bin rmodus zum Schreiben erstellen und Datei  ffnen
	FILE* fpw = fopen("Ergebnis.bmp", "wb");
	if (!fpw) {
		printf("Fehler beim Speichern der Datei");
		return -1;
	}

	//Dateikopf und Informationsblock schreiben
	fwrite(header.bfType, 2 * sizeof(char), 1, fpw);
	fwrite(&header.bfSize, 3 * sizeof(int), 1, fpw);
	fwrite(&infoHeader, sizeof(struct sBFInfoHeader), 1, fpw);

	//Offset f r die Pixeldaten setzen
	fseek(fpw, header.bfOffBits, SEEK_SET);

	//TODO
	//Pixeldaten reihenweise in die Ergebnisdatei schreiben.
	//Die Pixel werden wieder von unten nach oben gespeichert
	for (int i = image.height - 1; i >= 0; i--) {
		fwrite(image.rgb[i], rowSize, 1, fpw);
		free(image.rgb[i]);
	}

	//TODO
	//Datei schlie en
	fclose(fpw);

	//TODO
	//Speicher freigeben 


	return 0;
}


//TODO
/*
* Ziel: Die Funktion legt einen Graufilter  ber ein Bild.
* Hierzu betrachtet die Funktion jedes einzelne Pixel und f hrt f r dieses die folgenden Aktionen durch:
* - Es werden der Rotanteil, der Blauanteil und der Gr nanteil des Pixels mit jeweils 0,3 multipliziert.
* - Anschlie end wird die Summe der drei Produkte gebildet.
* - Diese Summe wird durch 3 geteilt, um den Durchschnittswert f r das Pixel zu erhalten.
* - F r das Pixel werden dann der Rotanteil, der Gr nanteil und der Blauanteil auf diesen Durchschnittswert gesetzt.
*
* Parameter:
* - image: Das Bild, auf den der Graufilter gelegt werden soll.
*/
void filterGray(sImage image) {
	for (int i = 0; i < image.height; i++) {
		for (int j = 0; j < image.width; j++) {
			unsigned char gray = (image.rgb[i][j].red + image.rgb[i][j].green + image.rgb[i][j].blue) / 3;
			image.rgb[i][j].red = gray;
			image.rgb[i][j].green = gray;
			image.rgb[i][j].blue = gray;
		}
	}
}


//TODO
/*
* Ziel: Die Funktion legt einen Sepiafilter  ber ein Bild.
* Hierzu betrachtet die Funktion jedes einzelne Pixel und f hrt f r dieses die folgenden Aktionen durch:
* - F r den neuen Rotanteil des Pixel werden der Rotanteil mit 0,320, der Gr nanteil mit 0,440 und der Blauanteil mit 0,240 multipliezrt und die Produkte addiert.
* - F r den neuen Gr nanteil des Pixel werden der Rotanteil mit 0,360, der Gr nanteil mit 0,360 und der Blauanteil mit 0,160 multipliezrt und die Produkte addiert.
* - F r den neuen Blauanteil des Pixel werden der Rotanteil mit 0,280, der Gr nanteil mit 0,284 und der Blauanteil mit 0,140 multipliezrt und die Produkte addiert.
* - F r das Pixel werden dann der Rotanteil, der Gr nanteil und der Blauanteil entsprechend auf die berechneten neuen Farbanteile gesetzt.
*
* Parameter:
* - image: Das Bild, auf den der Sepiafilter gelegt werden soll.
*/
void filterSepia(sImage image) {
	for (int i = 0; i < image.height; i++) {
		for (int j = 0; j < image.width; j++) { {}
		unsigned char red = (unsigned char)((image.rgb[i][j].red * 0.320) + (image.rgb[i][j].green * 0.440) + (image.rgb[i][j].blue * 0.240));
		unsigned char green = (unsigned char)((image.rgb[i][j].red * 0.360) + (image.rgb[i][j].green * 0.360) + (image.rgb[i][j].blue * 0.160));
		unsigned char blue = (unsigned char)((image.rgb[i][j].red * 0.280) + (image.rgb[i][j].green * 0.284) + (image.rgb[i][j].blue * 0.140));
		image.rgb[i][j].red = (red > 255) ? 255 : red;
		image.rgb[i][j].green = (green > 255) ? 255 : green;
		image.rgb[i][j].blue = (blue > 255) ? 255 : blue;
		}
	}
}

//TODO
/*
* Ziel: Die Funktion legt einen Kompliment rfilter (CMY - cyan magentha yellow)  ber ein Bild.
* Hierzu betrachtet die Funktion jedes einzelne Pixel und f hrt f r dieses die folgenden Aktionen durch:
* - F r den Cyanwert (C) wird der Durchschnitt des Gr nanteils und des Blauanteils gebildet.
* - F r den Magenthawert (M) wird der Durchschnitt des Rotanteils und des Blauanteils gebildet.
* - F r den Gelbewert (Y) wird der Durchschnitt des Gr nanteils und des Rotanteils gebildet.
* - F r das Pixel werden dann der Cyanwert als Rotanteil, der Magenthawert als Gr nanteil und der Gelbwert als Blauanteil gesetzt.
*
* Parameter:
* - image: Das Bild, auf den der Komplement rfilter gelegt werden soll.
*/
void filterNegative(sImage image) {

}


//TODO
/*
* Ziel: Die Funktion wendet ein Effekt auf ein Bild an, so dass das Bild verschwommen erscheint.
* Die Idee ist hierbei, dass der Gr nanteil und der Blauanteil auf der x-Achse um 7 Pixel verschoben werden.
* Hierzu betrachtet die Funktion jedes einzelne Pixel und f hrt f r dieses die folgenden Aktionen durch:
* - F r den Gr nanteil wird die Weite (width, x-Achse) des Pixels um 7 verschoben.
* - F r den Blauanteil wird die Weite (width, x-Achse) des Pixels um 7 verschoben.
* - F r das Pixel werden dann der Gr nanteil und der Blauanteil entsprechend aktualisiert.
*
* Parameter:
* - image: Das Bild, auf den der Effekt Verschwommen angewendet werden soll.
*/
void effectBlurred(sImage image) {
	int shift = 7;
	for (int i = 0; i < image.height; i++) {
		for (int j = 0; j < image.width - shift; j++) {
			image.rgb[i][j].green = image.rgb[i][j + shift].green;
			image.rgb[i][j].blue = image.rgb[i][j + shift].blue;
		}
	}
}


//TODO
/*
* Ziel: Die Funktion wendet einen Leinwandeffekt auf ein Bild an,
* so dass der obere und der untere Rand des Bild jeweils durch einen schwarzen Balken verdeckt werden.
* Die Idee ist hierbei, dass alle Farbanteile auf 0 gesetzt werden (also auf schwarz),
* deren Position entweder 70 Pixel unterhalb des oberen Bildrandes (height, y-Achse)
* oder 70 Pixel oberhalb des unteren Bildrandes (height, y-Achse) liegen.
* Hierzu betrachtet die Funktion jedes einzelne Pixel und
* f hrt je nach Lage des Pixels im Bild ggf. die beschriebene Farb nderung auf Schwarz durch.
*
* Parameter:
* - image: Das Bild, auf den der Leinwandeffekt angewendet werden soll.
*/
void effectScreen(struct sImage image) {

}


/**
* @brief Funktion liest einen positiven Integer von der Tastatur sicher ein.
*
* @param Parameter
* None.
*
* @return Gibt den eingelesenen Parameter zur ck.
*/
int getPositiveInt() {
	int val = 0;
	while (1) {
		if ((scanf("%d", &val) != 1) || (val < 0)) {
			while (getchar() != '\n');
			printf("Not a valid integer.\n");
		}
		else {
			while (getchar() != '\n');
			break;
		}
	}
	return val;
}