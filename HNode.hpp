#include<iostream>
#include <GL/glut.h>
#include<vector>

#ifndef _HNODE_HPP_
#define _HNODE_HPP_

void vertexcopy(float *src,float *dest);


// A simple class that represents a node in the hierarchy tree
class HNode {
//private:
public:
  float tx,ty,tz;
  float rx,ry,rz;
  float scalex, scaley, scalez;
  int num_vertices;
  float **vertex_pos;
  float **vertex_col;

  std::vector<HNode*> children;
  HNode* parent;


  bool flag;
  bool frame;

  HNode (HNode* parent, int num_vertices, float pos_v4[][3],  float col_v4[][3]);
  void add_child(HNode*);
  void render();
  void renderTorus();
  void scale_obj(float scalex, float scaley, float scalez);
  void change_parameters(float tx,float ty,float tz,float rx,float ry,float rz);
  void render_tree();
  void inc_rx();
  void inc_ry();
  void inc_rz();
  void dec_rx();
  void dec_ry();
  void dec_rz();
  void dec_rz_(float s);
  void inc_rz_(float s);

  void change_tx(float p);
  void change_ty(float p);
  void change_tz(float p);
};

#endif