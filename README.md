# Práctica Integradora de Computación Gráfica

Aplicación interactiva desarrollada en **C++ con OpenGL y GLUT** para crear, seleccionar, transformar y rellenar polígonos. Integra la rasterización de contornos mediante **Bresenham**, las transformaciones mediante **coordenadas homogéneas** y el relleno **Scan-Line con ET y EAT**.

## Integrantes

- Calisaya Quispe, Jose Luis
- Zapana Pariapaza, Gonzalo Rodrigo
- Himan Burgos, Sebastian Felipe

## Contexto académico

- **Universidad:** Universidad Nacional de San Agustín de Arequipa
- **Escuela:** Ciencia de la Computación
- **Curso:** Computación Gráfica
- **Práctica:** Rasterización, Transformaciones y Relleno de Polígonos
- **Docente:** Juan Carlos Gutiérrez Cáceres

## Objetivo

Reunir en una sola aplicación los algoritmos estudiados en el laboratorio, relacionando la interacción con el mouse, la representación de varios polígonos, las transformaciones geométricas y el relleno de sus interiores.

OpenGL se utiliza para mostrar los píxeles. El contorno y el relleno se calculan mediante los algoritmos implementados en el código.

## Funcionalidades

- Crear varios polígonos mediante clics del mouse.
- Cerrar cada polígono cuando tenga al menos tres vértices.
- Previsualizar la siguiente arista durante su construcción.
- Seleccionar el primer o segundo polígono mediante el teclado.
- Identificar el polígono activo por su contorno rojo.
- Rasterizar los contornos mediante Bresenham y `GL_POINTS`.
- Rotar, escalar y trasladar el polígono activo con matrices homogéneas de 3 × 3.
- Seleccionar un color de relleno rojo, verde o azul.
- Activar o desactivar el relleno del polígono activo.
- Conservar el color y el estado de relleno de cada polígono.
- Limpiar la escena y salir de la aplicación.

## Requisitos y ejecución

- Compilador de C++ compatible con **C++11 o posterior**.
- Bibliotecas **OpenGL**, **GLU** y **GLUT**, o una implementación compatible de GLUT.
- Un entorno configurado para compilar y enlazar esas bibliotecas. El proyecto se ha ejecutado en Windows utilizando **Zinjai**.

### Ejecución en Zinjai

1. Crear o abrir un proyecto con soporte para OpenGL/GLUT.
2. Agregar `IntegracionFinal.cpp` al proyecto.
3. Comprobar que OpenGL, GLU y GLUT estén configuradas en las opciones de compilación y enlazado.
4. Configurar C++11 o una versión posterior si el compilador lo requiere.
5. Compilar y ejecutar.
6. Utilizar el mouse y el teclado dentro de la ventana gráfica.

**El proyecto debe contener una sola definición de `main()`.** Si el entorno crea un `main.cpp` inicial, excluirlo de la compilación o reemplazar su contenido con la aplicación integrada. Los programas base del laboratorio son aplicaciones independientes y no deben compilarse juntos sin adaptarlos.

## Controles

| Control | Acción |
|---|---|
| Clic izquierdo | Agregar un vértice al polígono en construcción |
| Clic derecho | Cerrar el polígono y comenzar uno nuevo |
| `1` | Seleccionar el primer polígono cerrado |
| `2` | Seleccionar el segundo polígono cerrado |
| `D` o `d` | Rotar 5° a la derecha |
| `I` o `i` | Rotar 5° a la izquierda |
| `S` | Aumentar el tamaño un 10 % |
| `s` | Disminuir el tamaño un 10 % |
| Flecha arriba | Trasladar 10 unidades hacia arriba |
| Flecha abajo | Trasladar 10 unidades hacia abajo |
| Flecha izquierda | Trasladar 10 unidades hacia la izquierda |
| Flecha derecha | Trasladar 10 unidades hacia la derecha |
| `r` | Seleccionar rojo como color de relleno |
| `g` | Seleccionar verde como color de relleno |
| `b` | Seleccionar azul como color de relleno |
| `P` o `p` | Activar o desactivar el relleno del polígono activo |
| `C` o `c` | Limpiar la escena |
| `ESC` | Salir |

**Seleccionar un color no activa el relleno.** Para mostrarlo, se debe pulsar `P`. Las transformaciones y los cambios de color se aplican únicamente al polígono activo cerrado.

## Ejemplo de uso

1. Dibujar un polígono con al menos tres clics izquierdos y cerrarlo con clic derecho.
2. Dibujar y cerrar un segundo polígono.
3. Pulsar `1`, luego `r` y después `P` para rellenar el primero de rojo.
4. Pulsar `2`, luego `g` y después `P` para rellenar el segundo de verde.
5. Utilizar `D`, `I`, `S`, `s` y las flechas para transformar el seleccionado.
6. Alternar entre `1` y `2` para comprobar que cada polígono conserva su color.

El polígono recién cerrado queda seleccionado automáticamente. Los polígonos que ya tienen relleno lo conservan al seleccionar otro.

## Organización del código

### Representación de los polígonos

```cpp
struct Punto {
    float x, y;
};

struct Poligono {
    vector<Punto> P;
    bool cerrado = false;
    bool rellenado = false;
    float color[3] = {1.0f, 0.0f, 0.0f};
};
```

Cada polígono almacena sus vértices, su estado de cierre, su estado de relleno y su color RGB. La colección `vector<Poligono> poligonos` permite conservar varios objetos en la escena.

- **`poligonoActual`:** identifica el polígono que se está construyendo con el mouse.
- **`poligonoActivo`:** identifica el polígono seleccionado para transformaciones, color y relleno.

### Funciones principales

| Función | Responsabilidad |
|---|---|
| `Bresenham()` | Generar los píxeles de un segmento |
| `dibujarContorno()` | Rasterizar las aristas y la arista de cierre |
| `identidad()` y `multiplicar()` | Construir y componer matrices |
| `transformarPunto()` y `transformar()` | Aplicar una matriz a los vértices |
| `rotar()`, `escalar()` y `trasladar()` | Transformar el polígono |
| `construirET()` | Organizar las aristas según su coordenada vertical mínima |
| `rellenarPoligono()` | Calcular y dibujar el relleno Scan-Line |
| `display()` | Dibujar la escena y la previsualización |
| `mouse()` y `movimiento()` | Gestionar la construcción interactiva |
| `teclado()` y `teclasEspeciales()` | Gestionar los controles |
| `inicializar()` | Configurar la proyección y la escena inicial |

## Algoritmos implementados

### Rasterización mediante Bresenham

Cada arista se interpreta como un segmento entre dos vértices. El algoritmo utiliza las diferencias `dx` y `dy`, las direcciones de avance y un parámetro de decisión para generar incrementalmente los píxeles del segmento.

Los píxeles se muestran mediante `glVertex2i()` dentro de `GL_POINTS`. Al cerrar un polígono se rasteriza también el segmento entre el último vértice y el primero.

### Transformaciones homogéneas

Los puntos se interpretan como vectores columna `(x, y, 1)`. La traslación, la rotación y la escala se representan con matrices de 3 × 3.

La rotación y la escala se realizan alrededor del promedio de las coordenadas de los vértices, calculado por `centroPoligono()`. Este punto de referencia no es necesariamente el centroide de área del polígono.

La composición sigue este orden:

```text
Rotación: M = T(C) × R(ángulo) × T(-C)
Escala:   M = T(C) × S(factor) × T(-C)
```

Primero se lleva el punto de referencia al origen, después se aplica la operación y finalmente se devuelve a su posición. La traslación utiliza directamente la matriz `T(tx, ty)`.

### Relleno Scan-Line con ET y EAT

La **ET (Edge Table)** organiza las aristas no horizontales según su `ymin`. Cada arista almacena `ymin`, `ymax`, la intersección `x` y la pendiente inversa `Δx/Δy`.

La **EAT (Active Edge Table)** mantiene las aristas que intersectan la línea horizontal actual. Para cada línea de barrido se realizan estas operaciones:

1. Incorporar las aristas que comienzan en esa línea.
2. Retirar las aristas que alcanzan su `ymax`.
3. Ordenar las intersecciones por `x`.
4. Dibujar píxeles entre pares de intersecciones.
5. Actualizar cada intersección con `x = x + Δx/Δy`.

Las aristas horizontales se omiten y el extremo superior de cada arista se excluye para evitar contar dos veces las intersecciones en los vértices compartidos. El emparejamiento de intersecciones permite rellenar polígonos simples convexos y cóncavos.

ET y EAT se reconstruyen a partir de los vértices actuales cuando se dibuja el relleno, por lo que utilizan las coordenadas resultantes de las transformaciones. No se utiliza `GL_POLYGON` para rellenar.

## Casos de prueba del laboratorio

La siguiente lista sirve para organizar la demostración y las capturas de ejecución; no constituye un registro de pruebas verificadas.

| Caso | Comprobación esperada |
|---|---|
| Polígono convexo | Contorno cerrado y relleno interior correcto |
| Polígono cóncavo | Relleno que respete la concavidad |
| Dos polígonos simultáneos | Ambos permanecen visibles y pueden seleccionarse |
| Dos colores diferentes | Cada polígono conserva su propio color |
| Rotación seguida de relleno | El relleno corresponde a los vértices rotados |
| Escalamiento seguido de relleno | El relleno corresponde al nuevo tamaño |
| Traslación seguida de relleno | El relleno acompaña el desplazamiento |

Para documentar la práctica, agregar capturas de estos casos y las respuestas de reflexión solicitadas en el laboratorio.

## Alcance y limitaciones de la versión actual

- Las teclas `1` y `2` permiten volver a seleccionar los dos primeros polígonos. Aunque se pueden crear más, no hay un control para recorrer todos los objetos.
- La aplicación utiliza una proyección de referencia de **800 × 600** y no incluye una adaptación explícita de las coordenadas del mouse al redimensionar la ventana.
- El relleno requiere que el polígono esté cerrado y tenga al menos tres vértices.
- El recorte del relleno en los límites de la ventana es incompleto: una arista cuyo `ymin` queda por debajo de cero se descarta, aunque parte de ella siga visible. Para las pruebas de esta versión, mantener los polígonos dentro del área de dibujo.

## Archivos de referencia del laboratorio

La integración adapta las ideas desarrolladas en los programas base proporcionados por el docente:

- `LineasMouse.cpp`: interacción y Bresenham.
- `Poligono.cpp`: construcción y cierre de un polígono.
- `variosPoligonos.cpp`: colección y selección de polígonos.
- `tranformaciones.cpp`: matrices homogéneas, rotación y escala.
- `Relleno.cpp`: relleno Scan-Line mediante ET y EAT.

La aplicación final reúne estas funcionalidades en un único programa y añade la traslación del polígono activo y el almacenamiento independiente de su color y estado de relleno.
