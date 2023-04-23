# Introducción

Este es un lenguaje implementado con Flex y Bison que implementa 
algunos conceptos básicos de los lenguajes de programación. 
Se permite:

* Definir variables, ya sea mutables o constantes.

        x = 5
        const y = x 

  Una asignación se puede considerar una expresión que tiene como
valor el valor asignado, por lo que se pueden hacer asignaciones 
múltiples:

        x = y = z = 0
        
  Que es equivalente a
  
        x = (y = (z = 0))

* Utilizar bloques de expresiones, donde se pueden definir
variables locales sólo accesibles dentro del bloque.

        { x = 5 } 
        print(x) // x no definido

  Los bloques se pueden considerar como una expresión que toman
el valor de la última expresión, por lo que se pueden hacer cosas como

        x = {
            t1 = 5
            t2 = 3
            t1+t2
        }
        print(x) // 8

* Se pueden utilizar `if elses`.

        if x % 2 == 0 then {
            print("Par")
        } else {
            print("Impar")
        }

  Los corchetes de apertura y cierre opcionales en el caso de que se 
  utilice una única expresión en el bloque    

        if x % 2 == 0 then print("Par") else print("Impar")

  Adicionalmente, los `if elses` se pueden tratar como si fueran 
expresiones, por lo que se pueden hacer cosas como:
        
        print(if x % 2 == 0 then "Par" else "Impar")

* Se pueden utilizar bucles `while`.

        x = 1
        while x < 5 do { print(x); x = x +1 }

* Se pueden definir funciones con un número arbitrario de parámetros, con un
  cuerpo que es una expresión o un bloque de expresiones.
    
        f = \x,y => x+y
  
  Las funciones pueden "capturar" variables del entorno en el que se encuentran.

        n = 10
        sumar10 = \x => x+n
        n = 5
        print(sumar10(5)) // 15

* Se utiliza el concepto de módulo, que son archivos en los que se pueden
  definir variables que se pueden importar en otros módulos o en la
  consola interactiva.

        // ejemplo.mth
        export suma = \x,y => x+y

        // consola interactiva
        import "ejemplo.mth"
        print(suma(5,10)) // 15

* Se pueden utilizar funciones básicas aritméticas como `+`, `-`, `*`, `/` y `%`; 
operaciones de comparación como `==`, `!=`, `>`, `<`, `>=` y `<=`; y  operaciones 
sobre booleanos como `&&` y `||`. 
Adicionalmente, en el módulo por defecto `prelude.mth` se definen constantes
matemáticas y otras funciones complejas, como raíces cuadradas, senos, 
cosenos, etc.

        print(sin(5.0))
        print(sqrt(2))

* Se permiten operaciones básicas con strings, como concatenación e impresión.

        x = "Hola"
        y = "mundo"
        print(x+" "+y) // Hola mundo
        print(x, " ", y) // Hola mundo
        print("Repetido"*2) // RepetidoRepetido

* Se pueden cargar bibliotecas dinámicas de C y llamar a funciones que 
reciben tipos arbitrarios como argumentos. Esto se realiza mediante la llamada a
la función `callforeign`, que recibe como argumento la función foránea a llamar; 
el tipo que devuelve la función, y los argumentos que se pasarán a la función. 

        import foreign "libm.so" as math
      
        // double sin(double)
        callforeign(math.sin, "double", 4.5)

* Se puede saber las variables que están actualmente definidas en el espacio
  de trabajo utilizando la función `printws`, y se puede resetear el espacio 
  de trabjo con `resetws`.

        > import "math.mth"
        > printws()
        e := 2.718282
        hypot := [función]
        root := [función]
        falso := Falso
        exp := [función]
        sin := [función]
        sqrt := [función]
        log := [función]
        pi := 3.141593
        pow := [función]
        verdadero := Verdadero
        abs := [función]
        cos := [función]
        ln := [función]

* Se pueden cargar ficheros con `load`
    
        > load("README.md")
        # Introducción
        
        Este es un lenguaje implementado con Flex y Bison que implementa 
        los conceptos básicos de los lenguajes de programación. Permite:
        [...]

* Se pueden evaluar cadenas de forma dinámica con `eval`

        > eval("1+2+3")
        6
	 
  Esto, junto a la función `load`, se puede utilizar para ejecutar archivos
  como si se hubiesen ejecutado en la consola interactiva.

        > eval(load("test.mth"))
        fib(0) = 0
        fib(1) = 1
        fib(2) = 1
        fib(3) = 2
        fib(4) = 3

* Se puede pedir ayuda de una función o tipo de datos con la función `help`.

* Se puede salir del programa con la función `exit`

# Compilar

El programa se puede compilar con

        ./compile.sh

Es necesario tener instalado tanto Flex como Bison
	
        sudo apt-get install flex bison

También es posible que sea necesario instalar la biblioteca `libffi` para poder llamar funciones de bibliotecas dinámicas de C.

        sudo apt-get install libffi-dev

El código compila en el entorno probado de Ubuntu 22.04.2 LTS.

# Ejecutar

Para utilizar el programa como una consola interactiva se puede utilizar

        ./mth 

Si no, se puede ejecutar un archivo como si se hubiese introducido en la consola con

        ./mth archivo.mth

Se pueden consultar otras opciones y ayuda utilizando

        ./mth --help

## Ejemplos

* Implementación de la sucesión de Fibonacci en forma recursiva.

        fib = \n => {
            if n <= 0 then 0 else if n == 1 then 1 else fib(n-1)+fib(n-2)
        }

* Implementación de la sucesión de Fibonacci en forma cerrada.

        import "math.mth"        

        fib_closed = \n => {
            int((1/sqrt(5)) * (pow((1+sqrt(5))/2, n) - pow((1-sqrt(5))/2, n)))
        }

* Implementación de la sucesión de Fibonacci en forma iterativa.

        fib_iter = \n => {
            if n == 0 || n == 1 then return n
	  
            x = 0
            prev1 = 0
            prev2 = 1
	  
            i = 2
            while i <= n do {
                x = prev1 + prev2
                prev1 = prev2
                prev2 = x
                i = i + 1
            }
            return x
        }

# Llamar funciones externas de C

Llamar a tus propias funciones desde este lenguaje es muy sencillo. 
Dado el archivo `biblioteca.c` siguiente: 

        int sumar(int a, int b) {
            return a + b;
        }

Se puede crear una biblioteca dinámica con el comando
`gcc biblioteca.c --shared -o biblioteca.so`

Que se puede importar y ejecutar de la siguiente manera:

        > import foreign "biblioteca.so" as bib
        > callforeign(bib.sumar, "int", 1, 2)
        3

Nótese que los tipos de los argumentos que se pasan a la
función `callforeign` son importantes. Pasarle argumentos 
de un tipo que no encaje con el prototipo de la función de 
C causará resultados incorrectos, porque C estará interpretando 
la memoria incorrectamente.

        > callforeign(bib.sumar, "int", 1.0, 2.0)
        1917007888

Por esta razón es mejor utilizar casting a la hora de pasar argumentos
a funciones foráneas, para asegurarse que los tipos pasados son
correctos

        > callforeign(bib.sumar, "int", int(1.0), int(2.0))
        3

Para facilitar las llamas a funciones foráneas es más fácil esconder
la llamada a `callforeign` detrás de una función normal.

        > sumar = \x,y => callforeign(bib.sumar, "int", int(x), int(y))
        > sumar(1.0, 2.0)
        3

Los tipos de valores que se pueden pasar como argumento a una función foránea 
y su equivalente en C son los siguientes:

| Tipo Mth        | Tipo C |
|-----------------|--------|
| Entero          | int    |
| Decimal         | double |
| Booleano        | ushort |
| String          | void*  |
| Función foranea | void*  |

Mientras que los tipos de C admitidos como tipo de retorno de función
son los siguientes:

| Nombre Mth | Tipo Mth | Tipo C |
|------------|----------|--------|
| "int"      | Entero   | int    |
| "float"    | Decimal  | float  |
| "double"   | Decimal  | double |
| "void"     | Unidad   | void   |
