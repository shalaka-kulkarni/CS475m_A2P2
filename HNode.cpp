#include "HNode.hpp"

float shift = 0.5;

void vertexcopy(float *src,float *dest){
  dest[0]=src[0];
  dest[1]=src[1];
  dest[2]=src[2];
}

HNode::HNode(HNode* par, int num_v, float pos_v4[][3],  float col_v4[][3]){
  flag = false;
  frame = false;
  if(par!=NULL){
    parent=par;
    parent->add_child(this);
  }
  num_vertices=num_v;
  vertex_pos = new float*[num_vertices];
  vertex_col = new float*[num_vertices];
  for(int i=0;i<num_v;i++){
    vertex_pos[i]=new(float[3]);
    vertexcopy(pos_v4[i],vertex_pos[i]);

    vertex_col[i]=new(float[3]);
    vertexcopy(col_v4[i],vertex_col[i]);
  }
  tx=ty=tz=rx=ry=rz=0;
  scalex=scaley=scalez=1.0;
}

void HNode::add_child(HNode *child){
  children.push_back(child);
}

void HNode::renderTorus()
{
  glScalef(scalex,scaley,scalez);
  glTranslatef(tx,ty,tz);
  //Rotate at translated origin
  glRotatef(rx, 1.0, 0.0, 0.0);
  glRotatef(ry, 0.0, 1.0, 0.0);
  glRotatef(rz, 0.0, 0.0, 1.0);
  glColor3fv(vertex_col[0]);
  glutSolidTorus(vertex_pos[0][0]-vertex_pos[0][1], vertex_pos[0][0],50,50);
}

GLuint textureID1;

int loadBMP_custom1(const char * imagepath)
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
  glGenTextures(1, &textureID1);

  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, textureID1);

  // Give the image to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

void HNode::render(){
  if(flag==true)
  {
    this->renderTorus();
  }
  else
  {
  glScalef(scalex,scaley,scalez);
  //Translate the origin
  glTranslatef(tx,ty,tz);
  //Rotate at translated origin
  glRotatef(rx, 1.0, 0.0, 0.0);
  glRotatef(ry, 0.0, 1.0, 0.0);
  glRotatef(rz, 0.0, 0.0, 1.0);

  if(frame)
  {
    float tex_vec[3][2] = { {0.0,0.0}, {1.0,0.0}, {1.0,1.0} };
    loadBMP_custom1("./back.bmp");
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glBindTexture(GL_TEXTURE_2D, textureID1);
  glBegin(GL_TRIANGLES);
    for (int i=0;i<num_vertices;i++){
      glTexCoord2fv(tex_vec[i%3]);
      glColor3fv(vertex_col[i]);
      glVertex3fv(vertex_pos[i]);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);

  }
  else
  {
    glBegin(GL_TRIANGLES);
    for (int i=0;i<num_vertices;i++){
      glColor3fv(vertex_col[i]);
      glVertex3fv(vertex_pos[i]);
    }
    glEnd();
  }
  }
}

void HNode::change_parameters(float tx,float ty,float tz,float rx,float ry,float rz){
  this->tx=tx;
  this->ty=ty;
  this->tz=tz;
  this->rx=rx;
  this->ry=ry;
  this->rz=rz;
}

void HNode::scale_obj(float scalex, float scaley, float scalez){
  this->scalex=scalex;
  this->scaley=scaley;
  this->scalez=scalez;
}

void HNode::render_tree(){
  glPushMatrix();
  
    render();

  for(int i=0;i<children.size();i++){
    children[i]->render_tree();
  }
  glPopMatrix();
}


void HNode::inc_rx(){
  rx+=shift;
  if(rx>360)
  rx-=360;
}
void HNode::inc_ry(){
  ry+=shift;
  if(ry>360)
  ry-=360;
}
void HNode::inc_rz(){
  rz+=shift;
  if(rz>360)
  rz-=360;
}
void HNode::dec_rx(){
  rx-=shift;
  if(rx<0)
  rx+=360;
}
void HNode::dec_ry(){
  ry-=shift;
  if(ry<0)
  ry+=360;
}
void HNode::dec_rz(){
  rz-=shift;
  if(rz<0)
  rz+=360;
}

void HNode::inc_rz_(float s){
  rz+=s;
  if(rz>360)
  rz-=360;
}

void HNode::dec_rz_(float s){
  rz-=s;
  if(rz<0)
  rz+=360;
}

void HNode::change_tx(float p){
  tx+=p;
}
void HNode::change_ty(float p){
  ty+=p;
}
void HNode::change_tz(float p){
  tz+=p;
}