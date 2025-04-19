Joaquín Cadeiras
jcadeiras@udesa.edu.ar

## Etapa 1

En esta etapa comparaba un string del input con otro en memoria.

Si eran iguales, se pasaba a la siguiente etapa.

Para encontrarlo, se ejecuto
el comando `x/s 0x4c9a60` que devolvió el string:

`Confia en el tiempo, que suele dar dulces salidas a muchas amargas dificultades`


## Etapa 2

Esta etapa tomaba como input dos enteros. Después hacian las siguiente operaciones
en la función misterio:

> op1 = input1 + input2 - 32

> op2 = input1 ^ input2

op1 debería tener exactamente 11 bits prendidos y op2 debería ser menor a 0,
es decir, input1 y input2 deberían tener el signo opuesto.

Para encontrar la solución, simplemente se hizo que input1 = 32 (de esta forma se cancelaba
con el otro 32 en la operación) e input2 sea un numero negativo que tenga en total 11 bits
prendidos (con el bit negativo incluido).
Entonces, input2 = 0x800003ff = -2147482625 que tiene 1+0+0+0+0+2+4+4 = 11 bits prendidos.
Juntando la solución:

`32 -2147482625`

## Etapa 3

En esta tercera instancia se toma dos argumentos: un string (una palabra) y un entero.

- Se llama a una función `cuenta` que busca la palabra con búsqueda binaria recursiva en el
archivo `palabras.txt` y cuenta la cantidad de divisiones que hace para encontrarla.
- Compara este valor con el entero que se le pasa como argumento. Si son iguales, se pasa a la
siguiente etapa. Si la cantidad de divisiones es mayor a 11 o menor a 7 entonces explota la bomba.

Se logró llegar a la solución de dos formas:
1) Se seleccionó una palabra del input al azar (de palabras.txt) y con el debugger se
ejecutó el programa hasta la instrucción de la comparación. Notar que algunas palabras
podían hacer que la bomba explote debido a que tomaba mas de 11 divisiones en encontrarla,
por ende se tuvo que probar con algunas hasta encontrar una posible. De esta forma,
ejecutando el comando `x/wx $rsp+0x14` se consiguió el valor esperado del input, y se
utilizó este valor como input.

2) Se utilizo un script en python `phase_3.py` que leía el archivo palabras.txt y simulaba
la función `cuenta`, guardando la cantidad de divisiones que hacía para encontrar cada
palabra. Con esto se encontraron todas las soluciones y una de estas era la misma
respuesta que en el primer método, corroborando la comprensión de la función `cuenta`.

La solucion para esta etapa es:

`abanicar 9`


## Etapa 4

En la cuarta y "ultima" etapa se pide un string de largo 6. A cada caracter del input se le aplica
un mapeo, y este mapeo debe igualar la palabra `embole` que fue encontrada en la memoria con el
comando `x/s 0x4c709f`.

El mapeo consiste en lo siguiente:
- Hay un array de 16 elementos que corresponde al codigo ASCII de cada letra, que le corresponde el
valor del indice. Lo podemos representar como un diccionario, de la forma `{letra: indice}`.
- Con esto podemos contruir una lista de 6 elementos mapeando cada letra de la palabra `embole` a su
respectivo indice. Notar que los valores de los indices van de 0 a 15.
- Para encontrar la palabra inversa, debemos construir el diccionario inverso,
de la forma `{indice: letra}`.
- Ahora, por cada letra del input, se toma el codigo ASCII y se agarran los 4 bits menos
significativos (cuyos valores justamente van de 0-15) y se toman como indice del diccionario
inverso.
- Si los indices conseguidos en el input pasados por el diccionario inverso nos dan la palabra
"embole", entonces se pasa a la siguiente etapa.

Para buscar las posibles soluciones, se utilizó un script en python `phase_4.ipynb` que buscaba en
el rango de caracteres ASCII (32-126) y si los 4 bits menos significativos eran igual al indice
buscado entonces ese caracter era una posible solución.
Con esto, se encontraron 5 soluciones posibles, de las cuales sus caracteres puden ser cambiados
entre si sin cambiar su posición, por lo que hay 5^6=15625 soluciones posibles.

La solucion elegida para esta etapa es:
```
`bmho`
```


## Etapa secreta

Esta etapa esta escondida en el código, si se pasa la etapa 4 y no se tocan los inputs, el programa
termina sin que la bomba explote. Sin embargo, si observamos el código de `bomb_defused`, despues de
la cuarta fase lee nuevamente la tercera linea del input, y se fija si tiene 3 argumentos. Si no
los tiene, entonces el programa termina sin problemas, pero no entra en la etapa secreta. Si los
tiene, entonces este tercer argumento debe ser la palabra `abrete_sesamo` (encontrada con el
comando `x/s 0x4c7156`) para empezar la etapa secreta.
Entonces, la tercera linea del input queda de la forma:

`abanicar 9 abrete_sesamo`

Ahora con `secret_phase`:
- La etapa secreta pide un entero en la ultima linea del input que debe tener un valor
entre 1 y 1000.
- Dentro de esta, ejecuta una función `fun7` que hace una busqueda recursiva en un arbol binario.
- Toma la forma `fun7(*raiz, target)` donde  `raiz` es el nodo actual (que contiene el valor, nodos
hijos izq y der) y `target` es el valor que se  busca.
- El entero dado como input sera el tar`get para la busqueda.
- Al finalizar, la funcion devuelve el camino recorrido como un valor, siendo que ir a la  derecha
es 1 y a la izquierda es 0. Por ejemplo, si toma el camino derecha->derecha->izquierda, el valor
devuelto es `0b110`.

Sin embargo, `secret_phase` pide que el valor devuelto por `fun7` sea `0`, es decir, podemos
simplemente no tomar ningun camino y el valor devuelto sera 0.
Para hacer esto, hacemos que target sea el valor de la raiz. Esto lo podemos ver haciendo:

> (gdb) x/3dw 0x4f91f0 <br>
> 0x4f91f0 \<n1\>:  36      0       5214736

Lo cual nos dice que el valor de la raiz es `36`, su nodo hijo izquierdo es 0 (`NULL`)
y el derecho es 5214736.

Entonces, conociendo el valor de la raiz tenemos la solucion:

`36`