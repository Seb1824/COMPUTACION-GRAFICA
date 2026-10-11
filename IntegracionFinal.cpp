#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;
const int ANCHO = 800;
const int ALTO = 600;
const float PI = 3.14159265f;

struct Punto {
  float x, y;
};

struct Poligono {
  vector<Punto> P;
  bool cerrado = false;
  bool rellenado = false;
  float color[3] = {1.0f, 0.0f, 0.0f}; // Color RGB
};

struct Arista {
  int ymin;
  int ymax;
  float x;
  float invM;
};

vector<Poligono> poligonos;
int poligonoActual = 0;
int poligonoActivo = 0;
Punto Pmouse;
void Bresenham(int x1, int y1, int x2, int y2) {
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);

  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;

  int p = dx - dy;

  while (true) {
    glVertex2i(x1, y1);

    if (x1 == x2 && y1 == y2)
      break;

    int p2 = 2 * p;

    if (p2 > -dy) {
      p -= dy;
      x1 += sx;
    }
    if (p2 < dx) {
      p += dx;
      y1 += sy;
    }
  }
}

void identidad(float M[3][3]) {
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      M[i][j] = (i == j) ? 1.0f : 0.0f;
}

void multiplicar(float A[3][3], float B[3][3], float C[3][3]) {
  float tmp[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) {
      tmp[i][j] = 0;
      for (int k = 0; k < 3; k++)
        tmp[i][j] += A[i][k] * B[k][j];
    }
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      C[i][j] = tmp[i][j];
}

void matrizTraslacion(float tx, float ty, float T[3][3]) {
  identidad(T);
  T[0][2] = tx;
  T[1][2] = ty;
}

void matrizRotacion(float angulo, float R[3][3]) {
  identidad(R);
  float rad = angulo * PI / 180.0f;
  R[0][0] = cos(rad);
  R[0][1] = -sin(rad);
  R[1][0] = sin(rad);
  R[1][1] = cos(rad);
}

void matrizEscala(float sx, float sy, float S[3][3]) {
  identidad(S);
  S[0][0] = sx;
  S[1][1] = sy;
}

Punto transformarPunto(Punto P, float M[3][3]) {
  Punto P2;
  P2.x = M[0][0] * P.x + M[0][1] * P.y + M[0][2];
  P2.y = M[1][0] * P.x + M[1][1] * P.y + M[1][2];
  return P2;
}

void transformar(Poligono &pol, float M[3][3]) {
  for (int i = 0; i < (int)pol.P.size(); i++)
    pol.P[i] = transformarPunto(pol.P[i], M);
}

Punto centroPoligono(Poligono &pol) {
  Punto C = {0, 0};
  for (int i = 0; i < (int)pol.P.size(); i++) {
    C.x += pol.P[i].x;
    C.y += pol.P[i].y;
  }
  C.x /= pol.P.size();
  C.y /= pol.P.size();
  return C;
}

void trasladar(Poligono &pol, float tx, float ty) {
  float T[3][3];
  matrizTraslacion(tx, ty, T);
  transformar(pol, T);
}

void rotar(Poligono &pol, float angulo) {
  Punto C = centroPoligono(pol);

  float T1[3][3], R[3][3], T2[3][3];
  float M1[3][3], M[3][3];

  matrizTraslacion(-C.x, -C.y, T1);
  matrizRotacion(angulo, R);
  matrizTraslacion(C.x, C.y, T2);

  multiplicar(R, T1, M1); // M1 = R * T1
  multiplicar(T2, M1, M); // M  = T2 * R * T1

  transformar(pol, M);
}

void escalar(Poligono &pol, float escala) {
  Punto C = centroPoligono(pol);

  float T1[3][3], S[3][3], T2[3][3];
  float M1[3][3], M[3][3];

  matrizTraslacion(-C.x, -C.y, T1);
  matrizEscala(escala, escala, S);
  matrizTraslacion(C.x, C.y, T2);

  multiplicar(S, T1, M1); // M1 = S * T1
  multiplicar(T2, M1, M); // M  = T2 * S * T1

  transformar(pol, M);
}

vector<vector<Arista>> construirET(Poligono &pol) {
  vector<vector<Arista>> ET(ALTO);

  for (int i = 0; i < (int)pol.P.size(); i++) {
    int x1 = (int)round(pol.P[i].x);
    int y1 = (int)round(pol.P[i].y);
    int x2 = (int)round(pol.P[(i + 1) % pol.P.size()].x);
    int y2 = (int)round(pol.P[(i + 1) % pol.P.size()].y);

    if (y1 == y2)
      continue;

    Arista A;

    if (y1 < y2) {
      A.ymin = y1;
      A.ymax = y2;
      A.x = (float)x1;
      A.invM = (float)(x2 - x1) / (float)(y2 - y1);
    } else {
      A.ymin = y2;
      A.ymax = y1;
      A.x = (float)x2;
      A.invM = (float)(x1 - x2) / (float)(y1 - y2);
    }

    if (A.ymin >= 0 && A.ymin < ALTO)
      ET[A.ymin].push_back(A);
  }

  return ET;
}

void rellenarPoligono(Poligono &pol) {
  if (!pol.cerrado || pol.P.size() < 3)
    return;

  vector<vector<Arista>> ET = construirET(pol);
  vector<Arista> EAT;

  int ymin = ALTO;
  int ymax = 0;
  for (int i = 0; i < (int)pol.P.size(); i++) {
    int py = (int)round(pol.P[i].y);
    ymin = min(ymin, py);
    ymax = max(ymax, py);
  }

  ymin = max(ymin, 0);
  ymax = min(ymax, ALTO - 1);

  glColor3f(pol.color[0], pol.color[1], pol.color[2]);
  glBegin(GL_POINTS);

  for (int y = ymin; y < ymax; y++) {
    if (y >= 0 && y < ALTO) {
      for (int i = 0; i < (int)ET[y].size(); i++)
        EAT.push_back(ET[y][i]);
    }

    EAT.erase(remove_if(EAT.begin(), EAT.end(),
                        [y](Arista A) { return A.ymax == y; }),
              EAT.end());

    sort(EAT.begin(), EAT.end(), [](Arista A, Arista B) { return A.x < B.x; });

    for (int i = 0; i + 1 < (int)EAT.size(); i += 2) {
      int x1 = (int)ceil(EAT[i].x);
      int x2 = (int)floor(EAT[i + 1].x);

      for (int x = x1; x <= x2; x++)
        glVertex2i(x, y);
    }

    for (int i = 0; i < (int)EAT.size(); i++)
      EAT[i].x += EAT[i].invM;
  }

  glEnd();
}

void dibujarContorno(Poligono &pol, float cr, float cg, float cb) {
  glColor3f(cr, cg, cb);
  glPointSize(2.0f);
  glBegin(GL_POINTS);

  for (int i = 0; i < (int)pol.P.size() - 1; i++) {
    Bresenham((int)round(pol.P[i].x), (int)round(pol.P[i].y),
              (int)round(pol.P[i + 1].x), (int)round(pol.P[i + 1].y));
  }

  // Cerrar el poligono (ultima arista)
  if (pol.cerrado && pol.P.size() >= 3) {
    int n = pol.P.size();
    Bresenham((int)round(pol.P[n - 1].x), (int)round(pol.P[n - 1].y),
              (int)round(pol.P[0].x), (int)round(pol.P[0].y));
  }

  glEnd();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  for (int i = 0; i < (int)poligonos.size(); i++) {
    // 1) Si el poligono tiene relleno, pintarlo primero
    if (poligonos[i].rellenado) {
      glPointSize(1.0f);
      rellenarPoligono(poligonos[i]);
    }

    // 2) Dibujar contorno con Bresenham
    if (i == poligonoActivo)
      dibujarContorno(poligonos[i], 1.0f, 0.0f, 0.0f); // Rojo = activo
    else
      dibujarContorno(poligonos[i], 0.0f, 0.0f, 0.0f); // Negro
  }

  // 3) Linea de previsualizacion mientras se construye un poligono
  if (poligonoActual < (int)poligonos.size()) {
    Poligono &pol = poligonos[poligonoActual];

    if (!pol.cerrado && pol.P.size() > 0) {
      int n = pol.P.size();
      glColor3f(0.5f, 0.5f, 0.5f);
      glPointSize(1.0f);
      glBegin(GL_POINTS);
      Bresenham((int)round(pol.P[n - 1].x), (int)round(pol.P[n - 1].y),
                (int)round(Pmouse.x), (int)round(Pmouse.y));
      glEnd();
    }
  }

  glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
  if (state != GLUT_DOWN)
    return;

  y = ALTO - y;

  if (button == GLUT_LEFT_BUTTON) {
    if (poligonoActual >= (int)poligonos.size())
      return;

    Poligono &pol = poligonos[poligonoActual];

    if (pol.cerrado)
      return;

    Punto Pi;
    Pi.x = (float)x;
    Pi.y = (float)y;
    pol.P.push_back(Pi);

    cout << "Poligono " << poligonoActual + 1 << " - P" << pol.P.size()
         << " = (" << Pi.x << ", " << Pi.y << ")" << endl;

    glutPostRedisplay();
  }

  if (button == GLUT_RIGHT_BUTTON) {
    if (poligonoActual >= (int)poligonos.size())
      return;

    Poligono &pol = poligonos[poligonoActual];

    if (pol.P.size() < 3) {
      cout << "Se necesitan al menos 3 vertices." << endl;
      return;
    }

    pol.cerrado = true;

    cout << "Poligono " << poligonoActual + 1 << " cerrado. (" << pol.P.size()
         << " vertices)" << endl;

    Poligono nuevo;
    poligonos.push_back(nuevo);
    poligonoActual = poligonos.size() - 1;
    poligonoActivo = poligonoActual - 1; // Seleccionar el recien cerrado

    glutPostRedisplay();
  }
}

void movimiento(int x, int y) {
  Pmouse.x = (float)x;
  Pmouse.y = (float)(ALTO - y);

  if (poligonoActual < (int)poligonos.size() &&
      !poligonos[poligonoActual].cerrado &&
      poligonos[poligonoActual].P.size() > 0) {
    glutPostRedisplay();
  }
}

void teclado(unsigned char tecla, int x, int y) {
  if (tecla == '1') {
    if (poligonos.size() >= 1 && poligonos[0].cerrado) {
      poligonoActivo = 0;
      cout << "Poligono 1 activo" << endl;
    }
  }

  if (tecla == '2') {
    if (poligonos.size() >= 2 && poligonos[1].cerrado) {
      poligonoActivo = 1;
      cout << "Poligono 2 activo" << endl;
    }
  }

  // rotacion derecha
  if (tecla == 'd' || tecla == 'D') {
    if (poligonoActivo < (int)poligonos.size() &&
        poligonos[poligonoActivo].cerrado) {
      rotar(poligonos[poligonoActivo], -5.0f);
      cout << "Rotacion -5 grados (derecha)" << endl;
    }
  }

  // rotacion izquierda
  if (tecla == 'i' || tecla == 'I') {
    if (poligonoActivo < (int)poligonos.size() &&
        poligonos[poligonoActivo].cerrado) {
      rotar(poligonos[poligonoActivo], 5.0f);
      cout << "Rotacion +5 grados (izquierda)" << endl;
    }
  }

  // escalamiento
  if (tecla == 'S') {
    if (poligonoActivo < (int)poligonos.size() &&
        poligonos[poligonoActivo].cerrado) {
      escalar(poligonos[poligonoActivo], 1.10f);
      cout << "Escala +10%" << endl;
    }
  }

  if (tecla == 's') {
    if (poligonoActivo < (int)poligonos.size() &&
        poligonos[poligonoActivo].cerrado) {
      escalar(poligonos[poligonoActivo], 0.90f);
      cout << "Escala -10%" << endl;
    }
  }

  // asignar color de relleno
  if (tecla == 'r') {
    if (poligonoActivo < (int)poligonos.size() &&
        poligonos[poligonoActivo].cerrado) {
      poligonos[poligonoActivo].color[0] = 1.0f;
      poligonos[poligonoActivo].color[1] = 0.0f;
      poligonos[poligonoActivo].color[2] = 0.0f;
      cout << "Color de relleno: ROJO" << endl;
    }
  }

  if (tecla == 'g') {
    if (poligonoActivo < (int)poligonos.size() &&
        poligonos[poligonoActivo].cerrado) {
      poligonos[poligonoActivo].color[0] = 0.0f;
      poligonos[poligonoActivo].color[1] = 1.0f;
      poligonos[poligonoActivo].color[2] = 0.0f;
      cout << "Color de relleno: VERDE" << endl;
    }
  }

  if (tecla == 'b') {
    if (poligonoActivo < (int)poligonos.size() &&
        poligonos[poligonoActivo].cerrado) {
      poligonos[poligonoActivo].color[0] = 0.0f;
      poligonos[poligonoActivo].color[1] = 0.0f;
      poligonos[poligonoActivo].color[2] = 1.0f;
      cout << "Color de relleno: AZUL" << endl;
    }
  }

  // activar/desactivar relleno con 'P'
  if (tecla == 'P' || tecla == 'p') {
    if (poligonoActivo < (int)poligonos.size() &&
        poligonos[poligonoActivo].cerrado) {
      poligonos[poligonoActivo].rellenado =
          !poligonos[poligonoActivo].rellenado;

      if (poligonos[poligonoActivo].rellenado)
        cout << "Relleno ACTIVADO en poligono " << poligonoActivo + 1 << endl;
      else
        cout << "Relleno DESACTIVADO en poligono " << poligonoActivo + 1
             << endl;
    }
  }

  if (tecla == 'c' || tecla == 'C') {
    poligonos.clear();
    Poligono nuevo;
    poligonos.push_back(nuevo);
    poligonoActual = 0;
    poligonoActivo = 0;
    cout << "Pantalla limpiada." << endl;
  }

  // --- Salir ---
  if (tecla == 27)
    exit(0);

  glutPostRedisplay();
}

void teclasEspeciales(int key, int x, int y) {
  if (poligonoActivo >= (int)poligonos.size() ||
      !poligonos[poligonoActivo].cerrado)
    return;

  float paso = 10.0f;

  switch (key) {
  case GLUT_KEY_UP:
    trasladar(poligonos[poligonoActivo], 0.0f, paso);
    cout << "Traslacion arriba" << endl;
    break;
  case GLUT_KEY_DOWN:
    trasladar(poligonos[poligonoActivo], 0.0f, -paso);
    cout << "Traslacion abajo" << endl;
    break;
  case GLUT_KEY_LEFT:
    trasladar(poligonos[poligonoActivo], -paso, 0.0f);
    cout << "Traslacion izquierda" << endl;
    break;
  case GLUT_KEY_RIGHT:
    trasladar(poligonos[poligonoActivo], paso, 0.0f);
    cout << "Traslacion derecha" << endl;
    break;
  }

  glutPostRedisplay();
}

void inicializar() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glPointSize(1.0f);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, ANCHO, 0, ALTO);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  Poligono nuevo;
  poligonos.push_back(nuevo);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(ANCHO, ALTO);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("windows");

  inicializar();

  // Imprimir controles
  cout << " CONTROLES" << endl;
  cout << " Clic izq.       Agregar vertice" << endl;
  cout << " Clic der.       Cerrar poligono" << endl;
  cout << " 1 / 2           Seleccionar poligono" << endl;
  cout << " D / d           Rotar derecha (-5 deg)" << endl;
  cout << " I / i           Rotar izquierda (+5 deg)" << endl;
  cout << " S (mayus)       Escalar +10%" << endl;
  cout << " s (minus)       Escalar -10%" << endl;
  cout << " Flechas         Trasladar" << endl;
  cout << " r / g / b       Color de relleno" << endl;
  cout << " P / p           Activar/desactivar relleno" << endl;
  cout << " C / c           Limpiar pantalla" << endl;
  cout << " ESC             Salir" << endl;

  glutDisplayFunc(display);
  glutMouseFunc(mouse);
  glutPassiveMotionFunc(movimiento);
  glutKeyboardFunc(teclado);
  glutSpecialFunc(teclasEspeciales);

  glutMainLoop();
  return 0;
}
