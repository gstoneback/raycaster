#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <GLUT/glut.h>
#include "SDL_opengl.h"
#define SCR_W 320
#define SCR_H 200
float player_speed;
float px, py, pdx, pdy, pa; //player position and angle
float fov, raySpacing;
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
  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex2i(px, py);
  glVertex2i(px + pdx * 2, py + pdy * 2);
  glEnd();
}

float dist (float ax, float ay, float bx, float by, float ang) {
  return (sqrtf((bx - ax) * (bx - ax) + (by - ay) * (by - ay)));
}

void drawRays() {
  int col, mx, my, mp, dof;
  float rx, ry, ra, xo, yo, ca;
  float disH, hx, hy;
  float disV, vx, vy;
  float disT, lineH;
  float aTan, nTan;
  ra = pa - fov / 2;
  if (ra < 0) ra += 2 * M_PI;

  for (col = 0; col < SCR_W; ++col) {
    //check horizontal intersections
    dof = 0;
    disH = 1000000; hx = px; hy = py;
    aTan = -1/tanf(ra);
    if (ra > M_PI) { //looking up
      ry = (((int)py>>6)<<6) - 0.0001;
      rx = (py - ry) * aTan + px;
      yo = -64;
      xo = -yo * aTan;
    } else if (ra < M_PI) { //looking down
      ry = (((int)py>>6)<<6) + 64;
      rx = (py - ry) * aTan + px;
      yo = 64;
      xo = -yo * aTan;
    }
    if (ra == 0 || ra == M_PI) { //looking directly to the left or right
      rx = px;
      ry = py;
      dof = 8;
    }
    while (dof < 8) {
      mx = (int)(rx) >> 6;
      my = (int)(ry) >> 6;
      mp = my * mapW + mx;
      if (mp > 0 && mp < mapW * mapH && map[mp]) {
        hx = rx; hy = ry;
        disH = dist(px, py, hx, hy, ra);
        dof = 8; //hit wall
      } else {
        rx += xo;
        ry += yo;
        dof += 1;
      }
    }
    //check vertical intersections
    dof = 0;
    disV = 1000000; vx = px; vy = py;
    nTan = -tanf(ra);
    if (ra > M_PI / 2 && ra < 1.5 * M_PI) { //looking left
      rx = (((int)px>>6)<<6) - 0.0001;
      ry = (px - rx) * nTan + py;
      xo = -64;
      yo = -xo * nTan;
    } else if (ra < M_PI / 2 || ra > 1.5 * M_PI) { //looking right
      rx = (((int)px>>6)<<6) + 64;
      ry = (px - rx) * nTan + py;
      xo = 64;
      yo = -xo * nTan;
    }
    if (ra == 0 || ra == M_PI) { //looking directly up or down
      rx = px;
      ry = py;
      dof = 8;
    }
    while (dof < 8) {
      mx = (int)rx >> 6;
      my = (int)ry >> 6;
      mp = my * mapW + mx;
      if (mp > 0 && mp < mapW * mapH && map[mp]) {
        vx = rx; vy = ry;
        disV = dist(px, py, vx, vy, ra);
        dof = 8; //hit wall
      } else {
        rx += xo;
        ry += yo;
        dof += 1;
      }
    }
    if (disV > disH) {
      rx = hx;
      ry = hy;
      disT = disH;
      glColor3f(1, 1, 1);
    } else {
      rx = vx;
      ry = vy;
      disT = disV;
      glColor3f(0.8, 0.8, 0.8);
    }
    /*
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(px, py);
    glVertex2i(rx, ry);
    glEnd();
    */
    //draw walls
    ca = pa - ra;
    if (ca < 0) ca += 2 * M_PI;
    if (ca >= 2 * M_PI) ca -= 2 * M_PI;
    disT *= cosf(ca);
    lineH = (mapS * SCR_H)/disT;
    if (lineH > SCR_H) lineH = SCR_H;
    glLineWidth(glutGet(GLUT_SCREEN_WIDTH) / SCR_W);
    glBegin(GL_LINES);
    glVertex2i(col, (SCR_H - lineH) / 2);
    glVertex2i(col, (SCR_H + lineH) / 2);
    glEnd();
    //increment ray angle
    ra += raySpacing;
    if (ra > 2 * M_PI) ra -= 2 * M_PI;
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
  //drawMap2D();
  drawRays();
  //drawPlayer();
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
  if (key == 'w') { px += pdx; py += pdy; }
  if (key == 's') { px -= pdx; py -= pdy; }
  glutPostRedisplay();
}

void init() {
  glClearColor(0.3, 0.3, 0.3, 0);
  gluOrtho2D(0, SCR_W, SCR_H, 0);
  fov = M_PI / 3; raySpacing = fov / SCR_W;
  px = 100; py = 100; pa = 0;
  pdx = cosf(pa) * player_speed; pdy = sinf(pa) * player_speed;
  player_speed = 5;
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(SCR_W, SCR_H);
  glutCreateWindow("DEMO 3D");
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(handleKey);
  glutMainLoop();
}
