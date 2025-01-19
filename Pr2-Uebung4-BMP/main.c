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
typedef struct sBMPFileHeader {
	char bfType[2];				//2 Bytes
	uint32_t bfSize;			//4 Bytes
	uint32_t bfReserved;		//4 Bytes
	uint32_t bfOffBits;			//4 Bytes
} sBMPFileHeader;

//Informationsblock wie definiert in
//https://de.wikipedia.org/wiki/Windows_Bitmap
typedef struct sBMPFileInfoHeader {
	uint32_t biSize;			//4 Bytes
	int32_t biWidth;			//4 Bytes
	int32_t biHeight;			//4 Bytes
	uint16_t biPlanes;			//2 Bytes
	uint16_t biBitCount;		//2 Bytes
	uint32_t biCompression;		//4 Bytes
	uint32_t biSizeImage;		//4 Bytes
	int32_t biXPelsPerMeter;	//4 Bytes, wird hier nicht verwendet, nur zur Vollständigkeit
	int32_t biYPelsPerMeter;	//4 Bytes, wird hier nicht verwendet, nur zur Vollständigkeit
	uint32_t biClrUsed;			//4 Bytes, wird hier nicht verwendet, nur zur Vollständigkeit
	uint32_t biClrImportant;	//4 Bytes, wird hier nicht verwendet, nur zur Vollständigkeit
} sBMPFileInfoHeader;

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
void bmpProcessing(int filter, int effect, char dateiName[]);
int getPositiveInt();

//Funktionen für Farbfilter (drei Beispiele)
void filterGray(struct sImage image);
void filterSepia(struct sImage image);
void filterNegative(struct sImage image);

//Funktionen für Effekte (zwei Beispiele)
void effectBlurred(struct sImage image);
void effectScreen(struct sImage image);

//Hauptprogramm
int main(int argc, char* argv[]) {

	char fileName[MAXSTRING] = { 0 };

	//TODO
	// Prüfung der Kommandozeilenparameter
	if (argc != 2) {
		printf("Error: Bitte geben Sie den Namen einer BMP-Datei als Argument an.\n");
		exit(1);
	}

	//Übernahme des Dateinamen in Variable dateiName (fileName)
	if (strlen(argv[1]) >= MAXSTRING) {
		printf("Error: Dateiname ist zu lang. Maximale Länge: %d Zeichen.\n", MAXSTRING - 1);
		exit(1);
	}

	// Prüfen, ob der Dateiname auf ".bmp" endet
	if (strlen(argv[1]) < 5 || strcmp(argv[1] + strlen(argv[1]) - 4, ".bmp") != 0) {
		printf("Error: Der Dateiname muss auf '.bmp' enden.\n");
		exit(1);
	}
	// Dateiname kopieren
	strncpy(fileName, argv[1], MAXSTRING - 1);
	fileName[MAXSTRING - 1] = '\0';

	// Weitere Logik
	printf("Dateiname übernommen: %s\n", fileName);


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
Ziel: Die Funktion wendet einen vom Benutzer ausgewählten Filter und einen vom Benutzer ausgewählten
Effekt auf ein Bild im BMP-Format an.
Das Ergebnis wird in der Datei "Ergebnis.bmp" gespeichert.

Falls ein Fehler auftritt, zum Beispiel die BMP-Datei nicht existiert oder im falschen Format vorliegt,
wird das Programm sofort beendet.

Parameter:
- filter: Vom Benutzer ausgewählter Filter
- effect: Vom Benutzer ausgewählter Effekt
- fileName: Dateiname des Bilds, das verändert werden soll.
*/
void bmpProcessing(int filter, int effect, char fileName[]) {

	//TODO
	//Datei im Binärmodul zum Lesen öffnen
	FILE* fp = NULL;
	fp = fopen(fileName, "rb");
	if (fp == NULL) {
		printf("Error: Datei konnte nicht geoeffnet werden.");
		exit(1);
	}



	//Definieren der Variablen
	sBMPFileHeader header;
	sBMPFileInfoHeader infoHeader;
	sImage image;

	//Einlesen Dateikopf (Variable header)
	//Da nur 2 Character im BMP-Dateiformat als bgType erlaubt sind, 
	//C aber eigentlich ein Stringendezeichen (\0) bei Zeichenketten benötigt,
	//wird das Einlesen der Datei auf zwei fread-Statements aufgeteilt
	//1. Schritt - Die ersten beiden Buchstaben einlesen
	fread(header.bfType, 1, 2, fp);
	//2. Schritt - Die restlichen 12 Byte vom Dateikopf einlesen
	fread(&header.bfSize, sizeof(uint32_t), 3, fp);

	//TODO
	//Einlesen Informationsblock (Variable infoHeader) mit sizeof(sBFInfoHeader) Byte
	if (fread(&infoHeader, sizeof(sBMPFileInfoHeader), 1, fp) != 1) {
		printf("Error: Informationsblock konnte nicht gelesen werden.\n");
		fclose(fp);
		exit(1);
	}


	//TODO
	//Fehlerabfragen
	// 1. Ist Datei im BMP-Format (ersten beiden Buchstaben im Feld bfType von Variable header müssen Werte 'B' und 'M' besitzen)? 
	if (header.bfType[0] != 'B' || header.bfType[1] != 'M') {
		printf("Error: Datei ist kein gueltiges BMP-Format.\n");
		fclose(fp);
		exit(1);
	}
	// 2. Hat der Informationsblock (Feld biSize von Variable infoHeader) die richtige Größe (sizeof(sBFInfoHeader))?
	if (infoHeader.biSize != sizeof(sBMPFileInfoHeader)) {
		printf("Error: Groesse des Informationsblocks ist ungueltig.\n");
		fclose(fp);
		exit(1);
	}
	// 3. Ist Datei im 24-Bit Format (Feld biBitCount von Variable infoHeader gleich 24)? 
	if (infoHeader.biBitCount != 24) {
		printf("Error: Die Datei ist nicht im 24-Bit-Format. Gefunden: %u\n", infoHeader.biBitCount);
		fclose(fp);
		exit(1);
	}
	// 4. Ist Datei nicht komprimiert (Feld biCompression von Variable infoHeader gleich 0)?
	if (infoHeader.biCompression != 0) {
		printf("Error: Die Datei ist komprimiert.\n");
		fclose(fp);
		exit(1);
	}
	//Falls ein Fehler vorliegt / auftritt, wird das Programm sofort mit einer entsprechenden Fehlermeldung beendet.


	//TODO
	//Dateizeiger fp auf Offset (bfOffBits von der Variablen header) setzen
	if (fseek(fp, header.bfOffBits, SEEK_SET) != 0) {
		printf("Error: Dateizeiger konnte nicht auf Offset gesetzt werden.\n");
		fclose(fp);
		exit(1);
	}



	//Höhe und Breite aus dem Informationsblock in Bilddaten übernehmen
	image.height = infoHeader.biHeight;
	image.width = infoHeader.biWidth;

	//Neues Bild anlegen
	//Hierzu Speicher im Heap anlegen und Bild einlesen

	//Berechnung der Anzahl an Pixel pro Bildzeile
	//Siehe https://en.wikipedia.org/wiki/BMP_file_format
	//Anzahl an Bytes pro Bildzeile, Abrundung beim Teilen mit 32 gewollt
	int rowSize = ((sizeof(sRGB) * 8 * image.width + 31) / 32) * 4;

	//Zeigervektor für zur Speicherung der einzelnen Bildzeilen anlegen 
	// (entspricht sozusagen der Höhe (height) des Bildes) 
	image.rgb = (struct sRGB**)malloc(image.height * sizeof(void*));
	//TODO
	//Fehlerabfrage, ob das Anlegen des Speichers geklappt hat
	if (image.rgb == NULL) {
		printf("Error: Speicher konnte nicht angelegt werden.\n");
		fclose(fp);
		exit(1);
	}

	//In BMP-Dateien werden die Pixel von unten nach oben gespeichert
	//Siehe https://en.wikipedia.org/wiki/BMP_file_format -> "Usually pixels are stored "bottom-up"
	//Also muss die Schleife in der untersten Zeile beginnen
	for (int i = (image.height - 1); i >= 0; i--) {
		//Speicher für jede Bildzeile anlegen und auslesen
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
	fclose(fileName);

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
	//Zeiger fpw zur Ergebnisdatei "Ergebnis.bmp" im Binärmodus zum Schreiben erstellen und Datei öffnen
	FILE* fpw = NULL;
	fpw = fopen("Ergebnis.bmp", "wb");
	if (fpw == NULL) {
		printf("Error: Ergebnisdatei konnte nicht erstellt oder geoeffnet werden.\n");
		exit(1);
	}



	//Dateikopf und Informationsblock schreiben
	fwrite(header.bfType, 2 * sizeof(char), 1, fpw);
	fwrite(&header.bfSize, 3 * sizeof(int), 1, fpw);
	fwrite(&infoHeader, sizeof(struct sBMPFileInfoHeader), 1, fpw);

	//Offset für die Pixeldaten setzen
	fseek(fpw, header.bfOffBits, SEEK_SET);

	//TODO
	//Pixeldaten reihenweise in die Ergebnisdatei schreiben.
	//Die Pixel werden wieder von unten nach oben gespeichert
	// Pixeldaten reihenweise in die Ergebnisdatei schreiben
	for (int i = image.height - 1; i >= 0; i--) {
		if (fwrite(image.rgb[i], rowSize, 1, fpw) != 1) {
			printf("Error: Fehler beim Schreiben der Pixeldaten in Zeile %d.\n", i);
			fclose(fpw);
			exit(1);
		}
	}


	//TODO
	//Datei schließen
	fclose(fpw);

	//TODO
	//Speicher freigeben
	for (int i = 0; i < image.height - 1; i++)
		free(image.rgb[i]);
	free(image.rgb);

	return;
}


//TODO
/*
* Ziel: Die Funktion legt einen Graufilter über ein Bild.
* Hierzu betrachtet die Funktion jedes einzelne Pixel und führt für dieses die folgenden Aktionen durch:
* - Es werden der Rotanteil mit 0,299, der Blauanteil mit 0,114 und der Grünanteil mit 0,587 des Pixels multipliziert.
* - Anschließend wird die Summe der drei Produkte gebildet.
* - Diese Summe wird durch 3 geteilt, um den Durchschnittswert für das Pixel zu erhalten.
* - Für das Pixel werden dann der Rotanteil, der Grünanteil und der Blauanteil auf diesen Durchschnittswert gesetzt.
* https://de.wikipedia.org/wiki/Grauwert
*
* Parameter:
* - image: Das Bild, auf den der Graufilter gelegt werden soll.
*/
void filterGray(sImage image) {
	for (int i = 0; i < image.height; i++) {
		for (int j = 0; j < image.width; j++) {
			// Zugriff auf das aktuelle Pixel
			sRGB* pixel = &image.rgb[i][j];

			// Berechnung des Grauwerts
			unsigned char gray = (unsigned char)(
				(pixel->red * 0.299) +
				(pixel->green * 0.587) +
				(pixel->blue * 0.114)
				);

			// Setzen des Grauwerts für alle Farbanteile
			pixel->red = gray;
			pixel->green = gray;
			pixel->blue = gray;
		}
	}
}



//TODO
/*
* Ziel: Die Funktion legt einen Sepiafilter über ein Bild.
* Hierzu betrachtet die Funktion jedes einzelne Pixel und führt für dieses die folgenden Aktionen durch:
* - Für den neuen Rotanteil des Pixel werden der Rotanteil mit 0,320, der Grünanteil mit 0,440 und der Blauanteil mit 0,240 multipliezrt und die Produkte addiert.
* - Für den neuen Grünanteil des Pixel werden der Rotanteil mit 0,360, der Grünanteil mit 0,360 und der Blauanteil mit 0,160 multipliezrt und die Produkte addiert.
* - Für den neuen Blauanteil des Pixel werden der Rotanteil mit 0,280, der Grünanteil mit 0,284 und der Blauanteil mit 0,140 multipliezrt und die Produkte addiert.
* - Für das Pixel werden dann der Rotanteil, der Grünanteil und der Blauanteil entsprechend auf die berechneten neuen Farbanteile gesetzt.
*
* Parameter:
* - image: Das Bild, auf den der Sepiafilter gelegt werden soll.
*/
void filterSepia(sImage image) {

}


//TODO
/*
* Ziel: Die Funktion legt einen Komplimentärfilter (CMY - cyan magentha yellow) über ein Bild.
* Hierzu betrachtet die Funktion jedes einzelne Pixel und führt für dieses die folgenden Aktionen durch:
* - Für den Cyanwert (C) wird der Durchschnitt des Grünanteils und des Blauanteils gebildet.
* - Für den Magenthawert (M) wird der Durchschnitt des Rotanteils und des Blauanteils gebildet.
* - Für den Gelbewert (Y) wird der Durchschnitt des Grünanteils und des Rotanteils gebildet.
* - Für das Pixel werden dann der Cyanwert als Rotanteil, der Magenthawert als Grünanteil und der Gelbwert als Blauanteil gesetzt.
*
* Parameter:
* - image: Das Bild, auf den der Komplementärfilter gelegt werden soll.
*/
void filterNegative(sImage image) {

}


//TODO
/*
* Ziel: Die Funktion wendet ein Effekt auf ein Bild an, so dass das Bild verschwommen erscheint.
* Die Idee ist hierbei, dass der Grünanteil und der Blauanteil auf der x-Achse um 7 Pixel verschoben werden.
* Hierzu betrachtet die Funktion jedes einzelne Pixel und führt für dieses die folgenden Aktionen durch:
* - Für den Grünanteil wird die Weite (width, x-Achse) des Pixels um 7 verschoben.
* - Für den Blauanteil wird die Weite (width, x-Achse) des Pixels um 7 verschoben.
* - Für das Pixel werden dann der Grünanteil und der Blauanteil entsprechend aktualisiert.
*
* Parameter:
* - image: Das Bild, auf den der Effekt Verschwommen angewendet werden soll.
*/
void effectBlurred(sImage image) {

	return;
}


//TODO
/*
* Ziel: Die Funktion wendet einen Leinwandeffekt auf ein Bild an,
* so dass der obere und der untere Rand des Bild jeweils durch einen schwarzen Balken verdeckt werden.
* Die Idee ist hierbei, dass alle Farbanteile auf 0 gesetzt werden (also auf schwarz),
* deren Position entweder 70 Pixel unterhalb des oberen Bildrandes (height, y-Achse)
* oder 70 Pixel oberhalb des unteren Bildrandes (height, y-Achse) liegen.
* Hierzu betrachtet die Funktion jedes einzelne Pixel und
* führt je nach Lage des Pixels im Bild ggf. die beschriebene Farbänderung auf Schwarz durch.
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
* @return Gibt den eingelesenen Parameter zurück.
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
