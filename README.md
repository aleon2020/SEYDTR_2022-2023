# Sistemas Empotrados y de Tiempo Real 2022-2023

¡Bienvenido! En este repositorio se encuentran todos los materiales correspondientes a la asignatura de Sistemas Empotrados y de Tiempo Real.

A continuación se detallan brevemente todos los contenidos que se encuentran en este repositorio, con el objetivo de facilitar la preparación del examen final de la asignatura y de abordar la misma lo mejor posible.

## 1. Resumen de los contenidos de teoría

Resumen Teoría SEYDTR.pdf: Resumen de teoría en formato PDF.

Este enlace te redirigirá a un resumen en Google Docs que comprende todos los contenidos teóricos vistos en la asignatura, en el que cualquiera que tenga acceso a este documento pueda añadir cualquier contribución:

https://docs.google.com/document/d/1RDM8s-bbRYTS4ffhBa_m_h-fkaHRRio5A9Q_pPWfK9U/edit?usp=sharing

IMPORTANTE: Para hacer uso del índice interactivo que viene implementado en el resumen, debes descargar el documento en formato PDF.

## 2. Diapositivas utilizadas en clase

Directorio 'Diapositivas Teoría': Dentro de este directorio se encuentran las diapositivas utilizadas en clase para abordar los contenidos teóricos de la asignatura.

## 3. Prácticas

Directorios 'Práctica X: NombrePráctica': Cada uno de estos directorios corresponde a cada una de las prácticas realizadas en la asignatura, los cuáles incluyen código fuente, archivos de compilación, enlaces a blogs, y, en algunos casos, memorias en formato PDF que se han solicitado para mostrar datos, conclusiones, etc ... 

Además, también se tienen los enunciados de cada una de las prácticas en formato PDF.

NOTA: El enunciado de la Práctica 4 no se encuentra en formato PDF, ya que el enunciado de ésta última es un enlace a un repositorio de GitLab en la que se detalla más ampliamente su funcionamiento y especificaciones.

### 3.1 Compilación y ejecución

Si ya has visto de qué trata cada una de las prácticas, te habrás dado cuenta de que las Prácticas 1 y 2 se desarrollan en C, mientras que las Prácticas 3 y 4 se desarrollan en Arduino. 

Las Prácticas 3 y 4 son las más fáciles de compilar y ejecutar, ya que al trabajar en Arduino siempre se va a utilizar para probar su funcionamiento el IDE de Arduino, similar a MU-editor, utilizado en la asigntura de Fundamentos de la Programación.

Sin embargo, las Prácticas 1 y 2, al ser desarrolladas en C, siguen un proceso diferente. En primer lugar, vemos como en cada una de estas dos prácticas tenemos un fichero MakeFile, el cual se utiliza como fichero de compilación y en el que se define el nombre del ejecutable correpondiente a cada fichero .c.

A continuación se detalla la estructura que sigue el MakeFile más básico, y el cual se utiliza tanto en la Práctica 1 como en la Práctica 2:

```sh
<executable>: <filename>.c
	gcc -o <executable> <filename>.c
```

Por último, se detallan los pasos para compilar y ejecutar cada una de estas prácticas desarrolladas en C. 

```sh
$ cd Desktop/
$ git clone https://github.com/aleon2020/SEYDTR_2022-2023.git
$ cd SEYDTR_2022-2023/
$ cd 'Práctica 1: Multithreading en C y GNU-Linux'/
$ cd 'Práctica 2: CyclictestURJC, cálculo de latencias'/
$ cd src/
```

IMPORTANTE: Sólo haz cd de la práctica que quieras ejecutar, NO DE LAS DOS.

Una vez has clonado este repositorio en tu máquina y te encuentras dentro del directorio src/ de una de estas dos prácticas, debes ejecutar los dos siguientes comandos para ejecutar la práctica:

```sh
$ make
$ ./<executable>
```

## 4. Referencias

Librería pthread:

https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html

Tutorial de MakeFile:

https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

Guía de estilo:

https://gitlab.etsit.urjc.es/roberto.calvo/setr/-/wikis/Guia-de-Estilo-C

Repositorio GitLab de la asignatura:

https://gitlab.etsit.urjc.es/roberto.calvo/setr
