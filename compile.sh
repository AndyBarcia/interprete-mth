cd src
echo "Conviertiendo flex..."
flex -o analizador_lexico.c analizador_lexico.l
echo "Conviertiendo bison..."
bison -d analizador_sintactico.b -o analizador_sintactico.c
echo "Compilando código..."
gcc analizador_lexico.c analizador_sintactico.c codigo_fuente.c error.c evaluador.c main.c std.c string.c tabla_hash.c tabla_simbolos.c ast/ast.c ast/control_flujo.c ast/identificador.c ast/nombre_asignable.c ast/valor.c bibliotecas/biblioteca_dinamica.c -lffi -o ../mth
cd ..
