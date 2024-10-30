class Lector:
    def __init__(self, nombre, dni, libro_solicitado):
        self.nombre = nombre
        self.dni = dni
        self.libro_solicitado = libro_solicitado

class Libro:
    def __init__(self, titulo, autor, ano, editorial, isbn, numero_paginas):
        self.titulo = titulo
        self.autor = autor
        self.ano = ano
        self.editorial = editorial
        self.isbn = isbn
        self.numero_paginas = numero_paginas
        self.disponible = True
        self.siguiente = None

class ListaLibros:
    def __init__(self):
        self.cabeza = None
        self.cola_espera = []
        self.historial = []

    def agregar_libro(self, titulo, autor, ano, editorial, isbn, numero_paginas):
        nuevo_libro = Libro(titulo, autor, ano, editorial, isbn, numero_paginas)

        if not self.cabeza:
            self.cabeza = nuevo_libro
        else:
            temp = self.cabeza
            while temp.siguiente is not None:
                temp = temp.siguiente
            temp.siguiente = nuevo_libro

        self.historial.append(f"Agregar libro: {titulo}")
        print(f"Libro agregado: {titulo}")

    def cargar_libros_desde_archivo(self, biblioteca):
        try:
            with open(biblioteca, 'r') as archivo:
                while True:
                    titulo = archivo.readline().strip()
                    autor = archivo.readline().strip()
                    ano = archivo.readline().strip()
                    editorial = archivo.readline().strip()
                    isbn = archivo.readline().strip()
                    numero_paginas = archivo.readline().strip()

                    if not titulo or not autor or not ano or not editorial or not isbn or not numero_paginas:
                        break
                    
                    self.agregar_libro(titulo, autor, int(ano), editorial, isbn, int(numero_paginas))
        except Exception as e:
            print(f"Error al abrir el archivo: {biblioteca}. {e}")

    def guardar_libros_en_archivo(self, biblioteca):
        try:
            with open(biblioteca, 'w') as archivo:
                temp = self.cabeza
                while temp is not None:
                    archivo.write(f"{temp.titulo}\n")
                    archivo.write(f"{temp.autor}\n")
                    archivo.write(f"{temp.ano}\n")
                    archivo.write(f"{temp.editorial}\n")
                    archivo.write(f"{temp.isbn}\n")
                    archivo.write(f"{temp.numero_paginas}\n")
                    temp = temp.siguiente
            print("Biblioteca guardada en archivo.")
        except Exception as e:
            print(f"Error al abrir el archivo para guardar: {biblioteca}. {e}")

    def deshacer_ultima_operacion(self):
        if not self.historial:
            print("No hay operaciones para deshacer.")
            return

        ultima_operacion = self.historial.pop()
        print(f"Deshaciendo operación: {ultima_operacion}")

        if "Agregar libro: " in ultima_operacion:
            titulo = ultima_operacion[15:]
            self.eliminar_libro(titulo)
        elif "Eliminar libro: " in ultima_operacion:
            titulo = ultima_operacion[16:]
            print("Para deshacer la eliminación, debes re-agregar el libro manualmente.")

    def eliminar_libro(self, titulo):
        if not self.cabeza:
            return

        if self.cabeza.titulo == titulo:
            temp = self.cabeza
            self.cabeza = self.cabeza.siguiente
            del temp
            self.historial.append(f"Eliminar libro: {titulo}")
            return

        temp = self.cabeza
        while temp.siguiente is not None:
            if temp.siguiente.titulo == titulo:
                libro_a_eliminar = temp.siguiente
                temp.siguiente = temp.siguiente.siguiente
                del libro_a_eliminar
                self.historial.append(f"Eliminar libro: {titulo}")
                return
            temp = temp.siguiente

        print("No se encontró el libro para eliminar.")

    def solicitar_libro(self, nombre, dni, titulo):
        try:
            libro_solicitado = self.buscar_por_titulo(titulo)
            if not libro_solicitado:
                raise RuntimeError(f"Error: No se encontró un libro con el titulo '{titulo}'.")

            if libro_solicitado.disponible:
                libro_solicitado.disponible = False
                print(f"El libro '{titulo}' ha sido asignado a {nombre} (DNI: {dni}).")
                self.historial.append(f"Solicitar libro: {titulo} por {nombre}")
            else:
                print(f"El libro '{titulo}' no está disponible actualmente. {nombre} ha sido añadido a la cola de espera.")
                self.cola_espera.append(Lector(nombre, dni, titulo))
        except RuntimeError as e:
            print(e)

    def devolver_libro(self, titulo):
        try:
            libro_devuelto = self.buscar_por_titulo(titulo)
            if not libro_devuelto:
                raise RuntimeError(f"Error: No se encontró un libro con el titulo '{titulo}' para devolver.")

            libro_devuelto.disponible = True
            print(f"El libro '{titulo}' ha sido devuelto y está disponible nuevamente.")

            if self.cola_espera:
                lector_en_espera = self.cola_espera[0]
                if lector_en_espera.libro_solicitado == titulo:
                    self.cola_espera.pop(0)
                    libro_devuelto.disponible = False
                    print(f"El libro '{titulo}' ha sido asignado a {lector_en_espera.nombre} (DNI: {lector_en_espera.dni}) de la cola de espera.")

            self.historial.append(f"Devolver libro: {titulo}")
        except RuntimeError as e:
            print(e)

    def buscar_por_titulo(self, titulo):
        temp = self.cabeza
        while temp is not None:
            if temp.titulo == titulo:
                return temp
            temp = temp.siguiente
        return None

    def buscar_libro(self, titulo):
        posibles_titulos = []
        temp = self.cabeza

        while temp is not None:
            if titulo in temp.titulo:
                posibles_titulos.append(temp.titulo)
            temp = temp.siguiente

        if posibles_titulos:
            print("Posibles titulos encontrados:")
            for posible_titulo in posibles_titulos:
                print(f"- {posible_titulo}")
        else:
            print(f"No se encontró el libro con el titulo: {titulo}")

    def mostrar_lista_libros(self):
        if not self.cabeza:
            print("No hay libros en la biblioteca.")
            return

        print("Lista completa de libros:")
        temp = self.cabeza
        while temp is not None:
            print(f"- {temp.titulo} (Autor: {temp.autor}, Año: {temp.ano}, Editorial: {temp.editorial}, ISBN: {temp.isbn}, Páginas: {temp.numero_paginas}, Disponible: {'Sí' if temp.disponible else 'No'})")
            temp = temp.siguiente

    def ordenar_libros(self):
        if not self.cabeza or not self.cabeza.siguiente:
            return  # No hay nada que ordenar

        intercambio = True
        while intercambio:
            intercambio = False
            temp = self.cabeza
            while temp.siguiente is not None:
                if temp.titulo > temp.siguiente.titulo:
                    # Intercambiar titulos
                    temp.titulo, temp.siguiente.titulo = temp.siguiente.titulo, temp.titulo
                    temp.autor, temp.siguiente.autor = temp.siguiente.autor, temp.autor
                    temp.ano, temp.siguiente.ano = temp.siguiente.ano, temp.ano
                    temp.editorial, temp.siguiente.editorial = temp.siguiente.editorial, temp.editorial
                    temp.isbn, temp.siguiente.isbn = temp.siguiente.isbn, temp.isbn
                    temp.numero_paginas, temp.siguiente.numero_paginas = temp.siguiente.numero_paginas, temp.numero_paginas
                    intercambio = True
                temp = temp.siguiente

        print("Libros ordenados por titulo.")

def main():
    biblioteca = ListaLibros()
    biblioteca.cargar_libros_desde_archivo("biblioteca.txt")  # Cargar libros al inicio

    while True:
        print("\nSeleccione una opcion:")
        print("1. Agregar libro")
        print("2. Buscar libro")
        print("3. Solicitar libro")
        print("4. Devolver libro")
        print("5. Mostrar lista completa de libros")
        print("6. Deshacer última operación")
        print("7. Ordenar libros")
        print("0. Salir")
        opcion = int(input("Opcion: "))

        nombre, dni, titulo, autor, isbn, editorial = "", "", "", "", "", ""
        ano, numero_paginas = 0, 0

        if opcion == 1:  # Agregar libro
            print("Ingrese los detalles del libro:")
            titulo = input("titulo: ")
            autor = input("Autor: ")
            ano = int(input("Anio de edicion (solamente numeros): "))
            editorial = input("Editorial: ")
            isbn = input("ISBN: ")
            numero_paginas = int(input("Numero de páginas: "))
            biblioteca.agregar_libro(titulo, autor, ano, editorial, isbn, numero_paginas)

        elif opcion == 2:  # Buscar libro
            titulo = input("Ingrese el titulo del libro a buscar: ")
            biblioteca.buscar_libro(titulo)

        elif opcion == 3:  # Solicitar libro
            nombre = input("Ingrese su nombre: ")
            dni = input("Ingrese su DNI: ")
            titulo = input("Ingrese el titulo del libro a solicitar: ")
            biblioteca.solicitar_libro(nombre, dni, titulo)

        elif opcion == 4:  # Devolver libro
            titulo = input("Ingrese el titulo del libro a devolver: ")
            biblioteca.devolver_libro(titulo)

        elif opcion == 5:  # Mostrar lista completa de libros
            biblioteca.mostrar_lista_libros()

        elif opcion == 6:  # Deshacer última operación
            biblioteca.deshacer_ultima_operacion()

        elif opcion == 7:  # Ordenar libros
            biblioteca.ordenar_libros()

        elif opcion == 0:  # Salir
            biblioteca.guardar_libros_en_archivo("biblioteca.txt")  # Guardar antes de salir
            print("Adiós.")
            break

        else:
            print("Opción no válida.")

if __name__ == "__main__":
    main()
