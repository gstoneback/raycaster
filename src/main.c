#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>
float player_speed;
float px, py, pdx, pdy, pa; //player position and angle
int mapW = 8; int mapH = 8; int mapS = 64;
int map[] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 1, 1, 0, 0, 1,
  1, 0, 0, 1, 1, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1
};
void drawPlayer() {
  glColor3f(1, 1, 0);
  glPointSize(8);
  glBegin(GL_POINTS);
  glVertex2i(px, py);
  glEnd();
}

void drawRays() {
  int r, mx, my, mp, dof;
  float rx, ry, ra, xo, yo;
  ra = pa;
  for (r = 0; r < 1; ++r) {
    //check horizontal intersections
    dof = 0;
    float aTan = -1/tan(ra);
    if (ra > M_PI) { //looking up
      ry = (((int)py>>6)<<6);
      rx = (py - ry) * aTan + px;
      yo = -64;
      xo = -yo * aTan;
    } else if (ra < M_PI) { //looking down
      ry = (((int)py>>6)<<6) + 64;
      rx = (py - ry) * aTan + px;
      yo = 64;
      xo = -yo * aTan;
    } else { //looking directly to the left or right
      rx = px;
      ry = py;
      dof = 8;
    }
    while (dof < 8) {
      mx = (int)rx >> 6;
      my = (int)ry >> 6;
      mp = my * mapW + mx;
      if (mp > 0 && mp < mapW * mapH && map[mp]) {
        dof = 8; //hit wall
      } else {
        rx += xo;
        ry += yo;
        dof += 1;
      }
    }
    glColor3f(0, 1, 0);
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2i(px, py);
    glVertex2i(rx, ry);
    glEnd();
    //check vertical intersections
    dof = 0;
    float nTan = -tan(ra);
    if (ra > M_PI / 2 && ra < 1.5 * M_PI) { //looking left
      rx = (((int)px>>6)<<6);
      ry = (px - rx) * nTan + py;
      xo = -64;
      yo = -xo * nTan;
    } else if (ra < M_PI / 2 || ra > 1.5 * M_PI) { //looking right
      rx = (((int)px>>6)<<6) + 64;
      ry = (px - rx) * nTan + py;
      xo = 64;
      yo = -xo * nTan;
    } else { //looking directly up or down
      rx = px;
      ry = py;
      dof = 8;
    }
    while (dof < 8) {
      mx = (int)rx >> 6;
      my = (int)ry >> 6;
      mp = my * mapW + mx;
      if (mp > 0 && mp < mapW * mapH && map[mp]) {
        dof = 8; //hit wall
      } else {
        rx += xo;
        ry += yo;
        dof += 1;
      }
    }
    glColor3f(1, 0, 0);
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2i(px, py);
    glVertex2i(rx, ry);
    glEnd();
  }
}

void drawMap2D() {
  int x, y, xo, yo;
  for (y = 0; y < mapH; ++y) {
    for (x = 0; x < mapW; ++x) {
      if (map[y*mapW+x]) glColor3f(1, 1, 1); else glColor3f(0, 0, 0);
      xo = x*mapS; yo = y * mapS;
      glBegin(GL_QUADS);
      glVertex2i(xo        + 1, yo        + 1);
      glVertex2i(xo        + 1, yo + mapS - 1);
      glVertex2i(xo + mapS - 1, yo + mapS - 1);
      glVertex2i(xo + mapS - 1, yo        + 1);
      glEnd();
    }
  }
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawMap2D();
  drawPlayer();
  glutSwapBuffers();
}

void handleKey(unsigned char key, int x, int y) {
  if (key == 'a') {
    pa -= 0.1;
    if (pa < 0) pa += 2*M_PI;
    pdx = cosf(pa) * player_speed;
    pdy = sinf(pa) * player_speed;
  }
  if (key == 'd') {
    pa += 0.1;
    if (pa >= 2 * M_PI) pa -= 2*M_PI;
    pdx = cosf(pa) * player_speed;
    pdy = sinf(pa) * player_speed;
  }
  if (key == 'w') { px += pdx; py += pdy };
  if (key == 's') { px -= pdx; py -= pdy };
  glutPostRedisplay();
}

void init() {
  glClearColor(0.3, 0.3, 0.3, 0);
  gluOrtho2D(0, 1024, 512, 0);
  px = 300; py = 300; pa = 0;
  pdx = cosf(pa) * player_speed; pdy = sinf(pa) * player_speed;
  player_speed = 5;
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(320, 200);
  glutCreateWindow("DEMO 3D");
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(handleKey);
  glutMainLoop();
}
