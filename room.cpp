#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include "HNode.cpp"

#define PI 3.14159265

using namespace std;

 GLuint textureID;

 int camNumber = 3;
// angle of rotation for the camera direction
float camAngle=0.0;
// position of the camera
float camPos[3][3] = {{0.0, 0.0, 8.0}, {6.0, 10.0, 0.0}, {0.0, 0.0, 15.0}};
// position of point looked at
float lookAtPoint[3][3] = {{-5.0, 5.0, 0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
// up vector
float upVect[3][3] = {{-0.6, 1.0, 0}, {-1.0, 0.6, 0.0}, {0.0, 1.0, 0.0}};

float shiftCam = 0.5;

void initCamera();

//GLuint 
 int loadBMP_custom(const char * imagepath)
{
  // Data read from the header of the BMP file
  unsigned char header[54]; // Each BMP file begins by a 54-bytes header
  unsigned int dataPos;     // Position in the file where the actual data begins
  unsigned int width, height;
  unsigned int imageSize;   // = width*height*3
  // Actual RGB data
  unsigned char * data;
  // Open the file
  FILE * file = fopen(imagepath,"rb");
  if (!file){printf("Image could not be opened\n"); return 0;}
    if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
      printf("Not a correct BMP file\n");
      return false;
  }
  if ( header[0]!='B' || header[1]!='M' ){
    printf("Not a correct BMP file\n");
    return 0;
  }
  dataPos    = *(int*)&(header[0x0A]);
  imageSize  = *(int*)&(header[0x22]);
  width      = *(int*)&(header[0x12]);
  height     = *(int*)&(header[0x16]);
  // Some BMP files are misformatted, guess missing information
  if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
  if (dataPos==0)      dataPos=54; // The BMP header is done that way
  // Create a buffer
  data = new unsigned char [imageSize];

  // Read the actual data from the file into the buffer
  fread(data,1,imageSize,file);

  //Everything is in memory now, the file can be closed
  fclose(file);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &textureID);

  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Give the image to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

HNode *node[40];

float x_angle=0.0;
float y_angle=0.0;
float z_angle=0.0;
float offset=5.0;

float seat_vertices[24][3] = {
  {-2.0,0.0,0.0},{1.0,1.5,0.0},{1.0,-1.5,0.0},
  {-2.0,0.0,-0.4},{1.0,1.5,-0.4},{1.0,-1.5,-0.4},
  {-2.0,0.0,0.0},{1.0,1.5,0.0},{-2.0,0.0,-0.4},
  {-2.0,0.0,-0.4},{1.0,1.5,-0.4},{1.0,1.5,0.0},
  {1.0,1.5,0.0},{1.0,-1.5,0.0},{1.0,1.5,-0.4},
  {1.0,1.5,-0.4},{1.0,-1.5,-0.4},{1.0,-1.5,0.0},
  {-2.0,0.0,0.0},{1.0,-1.5,0.0},{-2.0,0.0,-0.4},
  {-2.0,0.0,-0.4},{1.0,-1.5,-0.4},{1.0,-1.5,0.0}
};

float seat_color[24][3] = {
  {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
  {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
  {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
  {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
  {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
  {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
  {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
  {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3}
};

//pedal dimensions
float l=0.75, b=0.5, h=0.2; // cuboid is 2l along x, 2b along y, 2h along z 

//pedal vertices
float cuboid_positions[36][3] = {
  {-l,b,h},  {-l,-b,h},  {l,-b,h},
  {-l,b,h},  {l,-b,h},  {l,b,h},
  {l,b,h},  {l,-b,h},  {l,-b,-h},
  {l,b,h},  {l,-b,-h},  {l,b,-h},
  {l,-b,h},  {-l,-b,h},  {-l,-b,-h},
  {l,-b,h},  {-l,-b,-h},  {l,-b,-h},
  {l,b,-h},  {-l,b,-h},  {-l,b,h},
  {l,b,-h},  {-l,b,h},  {l,b,h},
  {-l,-b,-h},  {-l,b,-h},  {l,b,-h},
  {-l,-b,-h},  {l,b,-h},  {l,-b,-h},
  {-l,b,-h},  {-l,-b,-h},  {-l,-b,h},
  {-l,b,-h},  {-l,-b,h},  {-l,b,h}
};

//pedal colors
float cuboid_colors[36][3] = {
  {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
   {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
   {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
   {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
   {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
   {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
   {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
   {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
   {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
   {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
   {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3},
   {0.3,0.3,0.3}, {0.3,0.3,0.3}, {0.3,0.3,0.3}
};

float cylinder_vertices[384][3];
float cylinder_colors[384][3];

float top_circle[32][3];
float bottom_circle[32][3];

void populateCylinderVertices(float radius, float height, float _r, float _g, float _b)
{
  for(int i=0; i<384; i++)
  {
    cylinder_colors[i][0] = _r;
    cylinder_colors[i][1] = _g;
    cylinder_colors[i][2] = _b;
  }

  for(int i=0; i<32; i++)
   {
    top_circle[i][0] = radius*cos(2*PI*i/32);
    top_circle[i][1] = radius*sin(2*PI*i/32);
    top_circle[i][2] = height/2;
   }
   
  for(int i=0; i<32; i++)
   {
    bottom_circle[i][0] = radius*cos(2*PI*i/32);
    bottom_circle[i][1] = radius*sin(2*PI*i/32);
    bottom_circle[i][2] = -1*height/2;
   }

  int index =0;
  for(int i=0; i<31; i++)
  {
    vertexcopy(bottom_circle[i], cylinder_vertices[index]);
    vertexcopy(bottom_circle[i+1],cylinder_vertices[index+1]);
    cylinder_vertices[index+2][0] = 0.0;
    cylinder_vertices[index+2][1] = 0.0;
    cylinder_vertices[index+2][2] = -1*height/2;
    index+=3;
  }
  
  vertexcopy(bottom_circle[31], cylinder_vertices[index]);
  vertexcopy(bottom_circle[0],cylinder_vertices[index+1]);
  cylinder_vertices[index+2][0] = 0.0;
  cylinder_vertices[index+2][1] = 0.0;
  cylinder_vertices[index+2][2] = -1*height/2;
  index+=3;

  for(int i=0; i<31; i++)
  {
    vertexcopy(top_circle[i],cylinder_vertices[index]);
    vertexcopy(top_circle[i+1],cylinder_vertices[index+1]);
    cylinder_vertices[index+2][0] = 0.0;
    cylinder_vertices[index+2][1] = 0.0;
    cylinder_vertices[index+2][2] = height/2;
    index+=3;
  }

  vertexcopy(top_circle[31],cylinder_vertices[index]);
  vertexcopy(top_circle[0],cylinder_vertices[index+1]);
  cylinder_vertices[index+2][0] = 0.0;
  cylinder_vertices[index+2][1] = 0.0;
  cylinder_vertices[index+2][2] = height/2;
  index+=3;

  for (int i=0;i<31;i++)
  {
    vertexcopy(top_circle[i],cylinder_vertices[index]);
    vertexcopy(top_circle[i+1],cylinder_vertices[index+1]);
    vertexcopy(bottom_circle[i],cylinder_vertices[index+2]);
    index+=3;

    vertexcopy(top_circle[i+1],cylinder_vertices[index+2]);
    vertexcopy(bottom_circle[i],cylinder_vertices[index]);
    vertexcopy(bottom_circle[i+1],cylinder_vertices[index+1]);
    index+=3;    
  }

  vertexcopy(top_circle[31],cylinder_vertices[index]);
  vertexcopy(top_circle[0],cylinder_vertices[index+1]);
  vertexcopy(bottom_circle[31],cylinder_vertices[index+2]);
  index+=3;

  vertexcopy(top_circle[0],cylinder_vertices[index+2]);
  vertexcopy(bottom_circle[0],cylinder_vertices[index+1]);
  vertexcopy(bottom_circle[31],cylinder_vertices[index]);
}



void cycleTranslate()
{
    float radius_of_rot = 9.3 / tan((node[12]->rz)*PI/180.0) ;
    
    //float centre_x = node[0]->tx+4.5;
    //float centre_y = node[0]->ty -  radius_of_rot;

    /*radius_of_rot = radius_of_rot * radius_of_rot;
    radius_of_rot += (4.5*4.5);
    radius_of_rot = sqrt(radius_of_rot);

    float x_change = -1* radius_of_rot * sin(PI*5/180.0);
    float y_change = -1* radius_of_rot * (1- cos(PI*5/180.0));

    if(node[12]->rz < 50)
      y_change *= -1.0;

    if(node[0]->rz <= 270.0)
      x_change *= -1;

    node[0]->change_tx(x_change);
    node[0]->change_ty(y_change); 
    node[0]->dec_rz_(5); */

    node[7]->dec_rz();
    node[21]->dec_rz();
    node[29]->dec_rz();

}

//Our function for processing ASCII keys
void processNormalKeys(unsigned char key, int x, int y) {
  switch(key) {
    case '1':
      camNumber = 1;
      break;
    case '2':
      camNumber = 2;
      break;
    case '3':
      camNumber = 3;
      break;
  }
  if (key == 27)
    exit(0);

  glutPostRedisplay();
}

void initCamera()
{
  /*lookAtPoint[0] -= abs(lookAtPoint[0])*(1-cos(camAngle));
  lookAtPoint[2] += abs(lookAtPoint[0])*sin(camAngle);*/
  gluLookAt(camPos[camNumber-1][0], camPos[camNumber-1][1], camPos[camNumber-1][2],
            lookAtPoint[camNumber-1][0], lookAtPoint[camNumber-1][1], lookAtPoint[camNumber-1][2],
            upVect[camNumber-1][0], upVect[camNumber-1][1], upVect[camNumber-1][2]);
  if(camNumber != 3)
    glRotatef(camAngle, 0.0, 1.0, 0.0);
}

//Our function for processing Non-ASCII keys
void processSpecialKeys(int key, int x, int y) {
  switch(key) {
    //Front wheel and handlebar lateral movement Node 12: front handlebar root node
    case GLUT_KEY_LEFT : 
    if(!(node[12]->rz>45 && node[12]->rz<315))
      node[12]->dec_rz();
    else
      node[12]->inc_rz();  
    break;
    
    case GLUT_KEY_RIGHT : 
    if(!(node[12]->rz>45 && node[12]->rz<315))
      node[12]->inc_rz();
    else
      node[12]->dec_rz();
    break;
    
    //Pedal and wheel rotation Node 7: Pedal axle, Node 21: Front wheel axle, Node 29: Back wheel axle
    case GLUT_KEY_UP : 
    cycleTranslate();
    /*node[7]->dec_rz();
    node[21]->dec_rz();
    node[29]->dec_rz();*/
    break;

    /*case GLUT_KEY_DOWN :
    node[7]->inc_rz();
    node[29]->inc_rz();
    node[21]->inc_rz();
    break;*/

    //Main frame rotation for changing view
    /*case GLUT_KEY_PAGE_UP : 
    node[0]->dec_rz();
    break;
    case GLUT_KEY_PAGE_DOWN :
    node[0]->inc_rz();
    break;*/
  }
  //Redraw
  glutPostRedisplay();
}

void render_room()
{
  glBegin(GL_TRIANGLES);
  glColor3f(0.70,0.70,0.70);

  glVertex3f(-9.0,-9.0,-9.0);
  glVertex3f(9.0,-9.0,-9.0);
  glVertex3f(9.0,9.0,-9.0);

  glVertex3f(9.0,9.0,-9.0);
  glVertex3f(-9.0,9.0,-9.0);
  glVertex3f(-9.0,-9.0,-9.0);

  glVertex3f(-9.0,-9.0,9.0);
  glVertex3f(9.0,-9.0,9.0);
  glVertex3f(9.0,9.0,9.0);

  glVertex3f(9.0,9.0,9.0);
  glVertex3f(-9.0,9.0,9.0);
  glVertex3f(-9.0,-9.0,9.0);

  glVertex3f(-9.0,-9.0,-9.0);
  glVertex3f(-9.0,-9.0,9.0);
  glVertex3f(-9.0,9.0,9.0);

  glVertex3f(-9.0,9.0,9.0);
  glVertex3f(-9.0,9.0,-9.0);
  glVertex3f(-9.0,-9.0,-9.0);

  glVertex3f(9.0,-9.0,-9.0);
  glVertex3f(9.0,-9.0,9.0);
  glVertex3f(9.0,9.0,9.0);

  glVertex3f(9.0,9.0,9.0);
  glVertex3f(9.0,9.0,-9.0);
  glVertex3f(9.0,-9.0,-9.0);

  //ceiling
  glColor3f(0.50,0.50,0.50);
  glVertex3f(-9.0,9.0,9.0);
  glVertex3f(9.0,9.0,9.0);
  glVertex3f(9.0,9.0,-9.0);

  glVertex3f(9.0,9.0,-9.0);
  glVertex3f(-9.0,9.0,-9.0);
  glVertex3f(-9.0,9.0,9.0);

  glEnd();


  //floor
  loadBMP_custom("./plank01.bmp");
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glBegin(GL_TRIANGLES);

  glTexCoord2f(0.0, 0.0);
  glVertex3f(-9.0,-9.0,9.0);
  glTexCoord2f(1.0, 0.0);
  glVertex3f(9.0,-9.0,9.0);
  glTexCoord2f(1.0, 1.0);
  glVertex3f(9.0,-9.0,-9.0);

  glTexCoord2f(1.0, 1.0);
  glVertex3f(9.0,-9.0,-9.0);
  glTexCoord2f(0.0, 1.0);
  glVertex3f(-9.0,-9.0,-9.0);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(-9.0,-9.0,9.0);

  glEnd();
  glDisable(GL_TEXTURE_2D);

  //Painting on wall
  loadBMP_custom("./sunset.bmp");
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glBegin(GL_TRIANGLES);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(-3.0,-1.0,-9.0);
  glTexCoord2f(1.0, 0.0);
  glVertex3f(3.0,-1.0,-9.0);
  glTexCoord2f(1.0, 1.0);
  glVertex3f(3.0,4.0,-9.0);

  glTexCoord2f(1.0, 1.0);
  glVertex3f(3.0,4.0,-9.0);
  glTexCoord2f(0.0, 1.0);
  glVertex3f(-3.0,4.0,-9.0);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(-3.0,-1.0,-9.0);

  glEnd();
  glDisable(GL_TEXTURE_2D);
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  initCamera();

  node[0]->render_tree();
  render_room();

  
  //Swaps front buffer with back buffer
  glutSwapBuffers();
}

void init(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   // Use depth buffering for hidden surface elimination.
  glEnable(GL_DEPTH_TEST);
  // Setup the view of the cube.
  glMatrixMode(GL_PROJECTION);
  //gluPerspective(field of view in degree, aspect ratio, Z near, Z far);
  gluPerspective(80.0, 1.0, 1.0, 20.0);

  glMatrixMode(GL_MODELVIEW);
  initCamera();
}

int main(int argc, char **argv)
{
  //Root node Frame vertical rod
  populateCylinderVertices(0.25,4.0,1.0,0.0,0.0);
  node[0] = new HNode(NULL,384,cylinder_vertices,cylinder_colors);
  node[0]->change_parameters(0.0,0.0,0.0,90.0,-5.0,0.0);
  node[0]->frame = true;

  //Seat support rod
  populateCylinderVertices(0.25,0.8,0.5,0.5,0.5);
  node[1] = new HNode(node[0],384,cylinder_vertices,cylinder_colors);
  node[1]->change_parameters(0.0,0.0,-2.4,0.0,0.0,0.0);

  //Frame rods
  populateCylinderVertices(0.25,5.0,1.0,0.0,0.0);
  node[2] = new HNode(node[0],384,cylinder_vertices,cylinder_colors);
  node[2]->change_parameters(-2.3,0.0,-1.5,0.0,90.0,0.0);
  node[2]->frame = true;

  populateCylinderVertices(0.25,5.3,1.8,0.0,0.0);
  node[3] = new HNode(node[0],384,cylinder_vertices,cylinder_colors);
  node[3]->change_parameters(-2.4,0.0,0.1,0.0,58.0,0.0);
  node[3]->frame = true;

  populateCylinderVertices(0.25,5.3,1.0,0.0,0.0);
  node[4] = new HNode(node[0],384,cylinder_vertices,cylinder_colors);
  node[4]->change_parameters(2.4,0.0,-0.4,0.0,60.0,0.0);
  node[4]->frame = true;

  populateCylinderVertices(0.25,5.0,1.0,0.0,0.0);
  node[5] = new HNode(node[0],384,cylinder_vertices,cylinder_colors);
  node[5]->change_parameters(2.4,0.0,1.4,0.0,100.0,0.0);
  node[5]->frame = true;

  //Seat
  node[6] = new HNode(node[0],24,seat_vertices, seat_color);
  node[6]->change_parameters(0.0,0.0,-2.8,0.0,0.0,0.0);

  //Axle for pedal shaft - 7
  populateCylinderVertices(0.25,1.4,0.5,0.5,0.5);
  node[7] = new HNode(node[0],384,cylinder_vertices,cylinder_colors);
  node[7]->change_parameters(0.0,0.0,2.0,90.0,0.0,0.0);

  //Pedal shafts
  populateCylinderVertices(0.15,1.4,0.5,0.5,0.5);
  node[8] = new HNode(node[7],384,cylinder_vertices,cylinder_colors);
  node[8]->change_parameters(0.8,0.0,-0.72,90.0,90.0,0.0);

  populateCylinderVertices(0.15,1.4,0.5,0.5,0.5);
  node[9] = new HNode(node[7],384,cylinder_vertices,cylinder_colors);
  node[9]->change_parameters(-0.8,0.0,0.72,90.0,90.0,0.0); 

  //Pedals
  node[10] = new HNode(node[8],36,cuboid_positions,cuboid_colors);
  node[10]->change_parameters(0.0,0.0,0.7+(b/2),0.0,90.0,90.0);

  node[11] = new HNode(node[9],36,cuboid_positions,cuboid_colors);
  node[11]->change_parameters(0.0,0.0,-0.7-(b/2),0.0,90.0,90.0);

  //Front Handlebar 
  //Axle for front handlebar movement - 12
  populateCylinderVertices(0.25,2.2,1.0,0.0,0.0);
  node[12] = new HNode(node[0],384,cylinder_vertices,cylinder_colors);
  node[12]->change_parameters(-4.8,0.0,-1.0,0.0,0.0,0.0);
  node[12]->frame = true;

  populateCylinderVertices(0.2,0.8,0.5,0.5,0.5);
  node[13] = new HNode(node[12],384,cylinder_vertices,cylinder_colors);
  node[13]->change_parameters(0.0,0.0,-1.5,0.0,0.0,0.0);

  populateCylinderVertices(0.2,0.7,0.5,0.5,0.5);
  node[14] = new HNode(node[13],384,cylinder_vertices,cylinder_colors);
  node[14]->change_parameters(-0.3,0.0,-0.35,0.0,50.0,90.0);

  //horizontal hndlebar
  populateCylinderVertices(0.15,3.0,0.5,0.5,0.5);
  node[15] = new HNode(node[12],384,cylinder_vertices,cylinder_colors);
  node[15]->change_parameters(-0.5,0.0,-2.1,90.0,0.0,0.0);

  //grips
  populateCylinderVertices(0.2,0.5,0.3,0.3,0.3);
  node[16] = new HNode(node[15],384,cylinder_vertices,cylinder_colors);
  node[16]->change_parameters(0.0,0.0,-1.5,0.0,0.0,0.0);

  populateCylinderVertices(0.2,0.5,0.3,0.3,0.3);
  node[17] = new HNode(node[15],384,cylinder_vertices,cylinder_colors);
  node[17]->change_parameters(0.0,0.0,1.5,0.0,0.0,0.0);

  //horizontal wheel support
  populateCylinderVertices(0.1,0.75,1.0,0.0,0.0);
  node[18] = new HNode(node[12],384,cylinder_vertices,cylinder_colors);
  node[18]->change_parameters(0.0,0.0,1.1,90.0,0.0,0.0);
  node[18]->frame = true;

  //Vertical wheel supports
  populateCylinderVertices(0.2,2.6,1.0,0.0,0.0);
  node[19] = new HNode(node[18],384,cylinder_vertices,cylinder_colors);
  node[19]->change_parameters(0.0,(-0.2+1.3),0.37,90.0,0.0,0.0);
  node[19]->frame = true;

  populateCylinderVertices(0.2,2.6,1.0,0.0,0.0);
  node[20] = new HNode(node[18],384,cylinder_vertices,cylinder_colors);
  node[20]->change_parameters(0.0,(-0.2+1.3),-0.37,90.0,0.0,0.0);
  node[20]->frame = true;  

  //Front Wheel
  populateCylinderVertices(0.1,1.0,0.3,0.3,0.3);
  node[21] = new HNode(node[12],384,cylinder_vertices,cylinder_colors);
  node[21]->change_parameters(0.0,0.0,3.4,90.0,0.0,0.0);  

  populateCylinderVertices(0.1,3.6,0.5,0.5,0.5);
  for(int i=22;i<28;i++)
  {
    node[i] = new HNode(node[21],384,cylinder_vertices,cylinder_colors); //cylinder_vertices, cylinder_colors);//
    node[i]->change_parameters(0,0,0,90,30*(i-1),0);
  }
  float v_tor_pos[1][3] = {2.0, 1.8, -999};
  float v_tor_col[1][3] = {0.3, 0.3, 0.3};
  node[28] = new HNode(node[21],1,v_tor_pos,v_tor_col);
  node[28]->flag = true;

  //Back wheel
  populateCylinderVertices(0.1,1.0,0.3,0.3,0.3);
  node[29] = new HNode(node[0],384,cylinder_vertices,cylinder_colors);
  node[29]->change_parameters(4.5,0.0,1.0,90.0,0.0,0.0);  

  populateCylinderVertices(0.1,4.2,0.5,0.5,0.5);
  for(int i=30;i<36;i++)
  {
    node[i] = new HNode(node[29],384,cylinder_vertices,cylinder_colors); //cylinder_vertices, cylinder_colors);//
    node[i]->change_parameters(0,0,0,90,30*(i-1),0);
  }
  float v_tor_posb[1][3] = {2.3, 2.1, -999};
  float v_tor_colb[1][3] = {0.3, 0.3, 0.3};
  node[36] = new HNode(node[29],1,v_tor_posb,v_tor_colb);
  node[36]->flag = true;  

  node[0]->scale_obj(0.20,0.20,0.20);
  node[0]->change_tx(0.0);
  node[0]->change_ty(0.0);
  node[0]->change_tz(5.0);
  //node[0]->change_parameters(2.0,-3.0,0.0,90.0,0.0,0.0);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(600,600);
  glutCreateWindow("My Bicycle");
  glutDisplayFunc(display);

  glutKeyboardFunc(processNormalKeys);
  glutSpecialFunc(processSpecialKeys);

  init();
  glutMainLoop();
  return 0;
}

// http://learnopengl.com/#!Getting-started/Camera
// https://www.cse.msu.edu/~cse872/tutorial4.html
// https://open.gl/textures