#include <iostream>
#include "compress.h"

using namespace std;

int main(){
	int option;

	do{
        cout << "\t" << "COMPRESSOR A BINARIO" << "\n" << endl;

		cout << "1.- Comprimir CFG" << "\n"
			 << "2.- Comprimir Fonts" << "\n"
			 << "3.- Comprimir Bodys" << "\n"
			 << "4.- Comprimir Mapa" << "\n"
			 << "5.- Comprimir indice de graficos" << "\n"
			 << "6.- Salir" << "\n"
			 << "Ingrese una opcion: ";
		cin >> option;

		system("cls");
	} while (option < 1 || option > 6);

	switch(option){
		case 1:
			comprimirCFG();
            cout << "Compression CFG exitosa!" << endl;
			break;
		case 2:
		    comprimirFonts();
			cout << "Compression Fonts exitosa!" << endl;
			break;

        case 3:
		    comprimirBodys();
			cout << "Compression Bodys exitosa!" << endl;
			break;

        case 4:
            comprimirMapa();
            cout << "Compression Mapa exitosa!" << endl;
            break;

        case 5:
            comprimirGraficos();
            cout << "Compression Graficos exitosa!" << endl;
            break;

		default:
		    system("cls");
			cout << "Exit program...." << endl;
			break;
	}

	system("pause");
	return 0;
}
