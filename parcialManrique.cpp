#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <stdexcept>
#include <fstream>
#include <vector>

using namespace std;

class Lector {
public:
    string nombre;
    string dni;
    string libroSolicitado;

    Lector(string _nombre, string _dni, string _libroSolicitado)
        : nombre(_nombre), dni(_dni), libroSolicitado(_libroSolicitado) {}
};

class Libro {
public:
    string titulo;
    string autor;
    int ano;
    string editorial;
    string isbn;
    int numeroPaginas;
    bool disponible;
    Libro* siguiente;

    Libro(string _titulo, string _autor, int _ano, 
          string _editorial, string _isbn, int _numeroPaginas) 
        : titulo(_titulo), autor(_autor), ano(_ano), 
          editorial(_editorial), isbn(_isbn), numeroPaginas(_numeroPaginas), 
          disponible(true), siguiente(nullptr) {}
};

class ListaLibros {
private:
    Libro* cabeza;
    queue<Lector> colaEspera;
    stack<string> historial;

public:
    ListaLibros() : cabeza(nullptr) {}

    void agregarLibro(string titulo, string autor, int ano, 
                      string editorial, string isbn, int numeroPaginas) {
        Libro* nuevoLibro = new Libro(titulo, autor, ano, editorial, isbn, numeroPaginas);
        
        if (!cabeza) {
            cabeza = nuevoLibro;
        } else {
            Libro* temp = cabeza;
            while (temp->siguiente != nullptr) {
                temp = temp->siguiente;
            }
            temp->siguiente = nuevoLibro;
        }

        historial.push("Agregar libro: " + titulo);
        cout << "Libro agregado: " << titulo << endl;
    }

     void cargarLibrosDesdeArchivo(const string& biblioteca) {
        ifstream archivo(biblioteca);
        if (!archivo.is_open()) {
            cout << "Error al abrir el archivo: " << biblioteca << endl;
            return;
        }

        string titulo, autor, editorial, isbn;
        int ano, numeroPaginas;

        while (getline(archivo, titulo) &&
               getline(archivo, autor) &&
               archivo >> ano &&
               archivo.ignore() && 
               getline(archivo, editorial) &&
               getline(archivo, isbn) &&
               archivo >> numeroPaginas &&
               archivo.ignore()) {
            agregarLibro(titulo, autor, ano, editorial, isbn, numeroPaginas);
        }
        archivo.close();
    }
    void guardarLibrosEnArchivo(const string& biblioteca) {
        ofstream archivo(biblioteca);
        if (!archivo.is_open()) {
            cout << "Error al abrir el archivo para guardar: " << biblioteca << endl;
            return;
        }
    Libro* temp = cabeza;
        while (temp != nullptr) {
            archivo << temp->titulo << endl;
            archivo << temp->autor << endl;
            archivo << temp->ano << endl;
            archivo << temp->editorial << endl;
            archivo << temp->isbn << endl;
            archivo << temp->numeroPaginas << endl;
            temp = temp->siguiente;
        }
        archivo.close();
        cout << "Biblioteca guardada en archivo." << endl;
    }

    void deshacerUltimaOperacion() {
        if (historial.empty()) {
            cout << "No hay operaciones para deshacer." << endl;
            return;
        }

        string ultimaOperacion = historial.top();
        historial.pop();
        cout << "Deshaciendo operación: " << ultimaOperacion << endl;

        if (ultimaOperacion.find("Agregar libro: ") != string::npos) {
            string titulo = ultimaOperacion.substr(15);
            eliminarLibro(titulo);
        } else if (ultimaOperacion.find("Eliminar libro: ") != string::npos) {
            string titulo = ultimaOperacion.substr(16);
            cout << "Para deshacer la eliminación, debes re-agregar el libro manualmente." << endl;
        }
    }

    void eliminarLibro(const string& titulo) {
        if (!cabeza) return;

        if (cabeza->titulo == titulo) {
            Libro* temp = cabeza;
            cabeza = cabeza->siguiente;
            delete temp;
            historial.push("Eliminar libro: " + titulo);
            return;
        }

        Libro* temp = cabeza;
        while (temp->siguiente != nullptr) {
            if (temp->siguiente->titulo == titulo) {
                Libro* libroAEliminar = temp->siguiente;
                temp->siguiente = temp->siguiente->siguiente;
                delete libroAEliminar;
                historial.push("Eliminar libro: " + titulo);
                return;
            }
            temp = temp->siguiente;
        }

        cout << "No se encontró el libro para eliminar." << endl;
    }

    void solicitarLibro(const string& nombre, const string& dni, const string& titulo) {
        try {
            Libro* libroSolicitado = buscarPorTitulo(titulo);
            if (!libroSolicitado) {
                throw runtime_error("Error: No se encontró un libro con el titulo '" + titulo + "'.");
            }

            if (libroSolicitado->disponible) {
                libroSolicitado->disponible = false;
                cout << "El libro '" << titulo << "' ha sido asignado a " << nombre << " (DNI: " << dni << ")." << endl;
                historial.push("Solicitar libro: " + titulo + " por " + nombre);
            } else {
                cout << "El libro '" << titulo << "' no está disponible actualmente. " << nombre << " ha sido añadido a la cola de espera." << endl;
                colaEspera.push(Lector(nombre, dni, titulo));
            }
        } catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
    }

    void devolverLibro(const string& titulo) {
        try {
            Libro* libroDevuelto = buscarPorTitulo(titulo);
            if (!libroDevuelto) {
                throw runtime_error("Error: No se encontró un libro con el titulo '" + titulo + "' para devolver.");
            }

            libroDevuelto->disponible = true;
            cout << "El libro '" << titulo << "' ha sido devuelto y está disponible nuevamente." << endl;

            if (!colaEspera.empty()) {
                Lector lectorEnEspera = colaEspera.front();
                if (lectorEnEspera.libroSolicitado == titulo) {
                    colaEspera.pop();
                    libroDevuelto->disponible = false;
                    cout << "El libro '" << titulo << "' ha sido asignado a " << lectorEnEspera.nombre << " (DNI: " << lectorEnEspera.dni << ") de la cola de espera." << endl;
                }
            }

            historial.push("Devolver libro: " + titulo);
        } catch (const runtime_error& e) {
            cout << e.what() << endl;
        }
    }

    Libro* buscarPorTitulo(const string& titulo) {
        Libro* temp = cabeza;
        while (temp != nullptr) {
            if (temp->titulo == titulo) return temp;
            temp = temp->siguiente;
        }
        return nullptr;
    }

    void buscarLibro(const string& titulo) {
        vector<string> posiblesTitulos;
        Libro* temp = cabeza;

        // Buscar posibles titulos que contengan el texto ingresado
        while (temp != nullptr) {
            if (temp->titulo.find(titulo) != string::npos) {
                posiblesTitulos.push_back(temp->titulo);
            }
            temp = temp->siguiente;
        }

        if (!posiblesTitulos.empty()) {
            cout << "Posibles titulos encontrados:\n";
            for (const string& posibleTitulo : posiblesTitulos) {
                cout << "- " << posibleTitulo << endl;
            }
        } else {
            cout << "No se encontró el libro con el titulo: " << titulo << endl;
        }
    }

    void mostrarListaLibros() {
        if (!cabeza) {
            cout << "No hay libros en la biblioteca." << endl;
            return;
        }

        cout << "Lista completa de libros:\n";
        Libro* temp = cabeza;
        while (temp != nullptr) {
            cout << "- " << temp->titulo << " (Autor: " << temp->autor << ", Año: " << temp->ano 
                 << ", Editorial: " << temp->editorial << ", ISBN: " << temp->isbn 
                 << ", Páginas: " << temp->numeroPaginas 
                 << ", Disponible: " << (temp->disponible ? "Sí" : "No") << ")\n";
            temp = temp->siguiente;
        }
    }

    ~ListaLibros() {
        while (cabeza != nullptr) {
            Libro* temp = cabeza;
            cabeza = cabeza->siguiente;
            delete temp;
        }
    }
    void ordenarLibros() {
    if (!cabeza || !cabeza->siguiente) {
        return; // No hay nada que ordenar
    }

    bool intercambio;
    do {
        intercambio = false;
        Libro* temp = cabeza;
        while (temp->siguiente != nullptr) {
            if (temp->titulo > temp->siguiente->titulo) {
                // Intercambiar titulos
                swap(temp->titulo, temp->siguiente->titulo);
                swap(temp->autor, temp->siguiente->autor);
                swap(temp->ano, temp->siguiente->ano);
                swap(temp->editorial, temp->siguiente->editorial);
                swap(temp->isbn, temp->siguiente->isbn);
                swap(temp->numeroPaginas, temp->siguiente->numeroPaginas);
                intercambio = true;
            }
            temp = temp->siguiente;
        }
    } while (intercambio);

    cout << "Libros ordenados por titulo." << endl;
}
};

int main() {
    ListaLibros biblioteca;
    biblioteca.cargarLibrosDesdeArchivo("biblioteca.txt"); // Cargar libros al inicio

    while (true) {
        cout << "\nSeleccione una opcion:\n";
        cout << "1. Agregar libro\n";
        cout << "2. Buscar libro\n";
        cout << "3. Solicitar libro\n";
        cout << "4. Devolver libro\n";
        cout << "5. Mostrar lista completa de libros\n";
        cout << "6. Deshacer última operación\n";
        cout << "7. Ordenar libros\n";
        cout << "0. Salir\n";
        cout << "Opcion: ";
        int opcion;
        cin >> opcion;
        cin.ignore(); // Limpiar el buffer

        string nombre, dni, titulo, autor, isbn, editorial;
        int ano, numeroPaginas;

        switch (opcion) {
            case 1: // Agregar libro
                cout << "Ingrese los detalles del libro:\n";
                cout << "titulo: ";
                getline(cin, titulo);
                cout << "Autor: ";
                getline(cin, autor);
                cout << "Anio de edicion (solamente numeros): ";
                cin >> ano;
                cin.ignore(); // Limpiar el buffer
                cout << "Editorial: ";
                getline(cin, editorial);
                cout << "ISBN: ";
                getline(cin, isbn);
                cout << "Numero de páginas: ";
                cin >> numeroPaginas;
                biblioteca.agregarLibro(titulo, autor, ano, editorial, isbn, numeroPaginas);
                break;

            case 2: // Buscar libro
                cout << "Ingrese el titulo del libro a buscar: ";
                getline(cin, titulo);
                biblioteca.buscarLibro(titulo);
                break;

            case 3: // Solicitar libro
                cout << "Ingrese su nombre: ";
                getline(cin, nombre);
                cout << "Ingrese su DNI (sin puntos y todo junto): ";
                getline(cin, dni);
                cout << "Ingrese el titulo del libro a solicitar: ";
                getline(cin, titulo);
                biblioteca.solicitarLibro(nombre, dni, titulo);
                break;

            case 4: // Devolver libro
                cout << "Ingrese el titulo del libro a devolver: ";
                getline(cin, titulo);
                biblioteca.devolverLibro(titulo);
                break;

            case 5: // Mostrar lista completa de libros
                biblioteca.mostrarListaLibros();
                break;

            case 6: // Deshacer última operación
                biblioteca.deshacerUltimaOperacion();
                break;

            case 7: // Ordenar libros
                biblioteca.ordenarLibros(); // Llamar a la función de ordenar
                break;
            case 0: // Salir
                biblioteca.guardarLibrosEnArchivo("biblioteca");; // Guardar libros antes de salir
                return 0;

            default:
                cout << "Opcion no valida. Intente nuevamente.\n";
        }
    }

    return 0;
}