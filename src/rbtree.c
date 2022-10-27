#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// nil 할당
node_t nil = {RBTREE_BLACK, 0, NULL, NULL, NULL};
node_t * nil_p = &nil;

// rbtree_to_array recursive용 count 할당
int count = 0;

/*
common 함수
*/
// red체크
bool is_red(node_t *node_p) {
  if (node_p->color == RBTREE_RED) return true;
  else return false;
}

// 좌측 회전
void left_rotate(rbtree *t, node_t *p_node){
  node_t *c_node = p_node->right;
  p_node -> right = c_node ->left;
  if (c_node->left != t->nil){
    c_node -> left -> parent = p_node;
  }
  c_node ->parent = p_node -> parent;
  if (p_node->parent == t->nil){
    t->root = c_node;
  }

  else if (p_node == p_node -> parent -> left){
    p_node -> parent -> left = c_node;
  }

  else{
    p_node->parent->right = c_node;
  }

  c_node->left = p_node;
  p_node->parent = c_node;
}

// 우측 회전
void right_rotate(rbtree *t, node_t *p_node){
  node_t *c_node = p_node->left;
  p_node -> left = c_node ->right;
  if (c_node->right!= t->nil){
    c_node -> right -> parent = p_node;
  }
  c_node -> parent = p_node -> parent;
  if (p_node->parent == t->nil){
    t->root = c_node;
  }

  else if (p_node == p_node -> parent -> right){
    p_node -> parent -> right = c_node;
  }

  else{
    p_node->parent-> left = c_node;
  }
  c_node->right = p_node;
  p_node->parent = c_node;
}

/*
RB트리 생성
*/
rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p -> nil = nil_p;
  p -> root = nil_p;
  return p;
}


/*
삭제
*/
// 후위순회 후 삭제
void backorder_search(const rbtree *t, node_t *now_node){
  if (now_node == t->nil){
    return;
  }

  backorder_search(t, now_node->left);
  backorder_search(t, now_node->right);
  free(now_node);
}

// 삭제 main
void delete_rbtree(rbtree *t) {
  count = 0;
  backorder_search(t, t->root);

  free(t);
}



/*
삽입
*/
// insert 색깔 변경 내부 함수
node_t *rbtree_insert_fixup(rbtree *t, node_t * new_node){
  node_t *y;
  while (is_red(new_node -> parent)){
    if (new_node -> parent == new_node -> parent -> parent -> left){
      y = new_node->parent->parent->right;
      if (is_red(y)){
        new_node -> parent -> color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        new_node->parent->parent->color = RBTREE_RED;
        new_node = new_node ->parent->parent;
      }
      else{
        if (new_node == new_node->parent->right){
          new_node = new_node->parent;
          left_rotate(t, new_node);
        }
        new_node->parent->color = RBTREE_BLACK;
        new_node->parent->parent->color = RBTREE_RED;
        right_rotate(t, new_node->parent->parent);
      }
    }
    else{
      y = new_node->parent->parent->left;
      if (is_red(y)){
        new_node -> parent -> color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        new_node->parent->parent->color = RBTREE_RED;
        new_node = new_node ->parent->parent;
      }
      else{
        if (new_node == new_node->parent->left){
          new_node = new_node->parent;
          right_rotate(t, new_node);
        }
        new_node->parent->color = RBTREE_BLACK;
        new_node->parent->parent->color = RBTREE_RED;
        left_rotate(t, new_node->parent->parent); 
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}


// insert main
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));

  node_t *y = t->nil;
  node_t *x = t->root;

  while (x != t-> nil){
    y = x;
    if (key < x -> key){
      x = x->left;
    }
    else{
      x = x -> right;
    }
  }

  new_node -> parent = y;
  if(y == t->nil){
    t -> root = new_node;
  }

  else if (key < y -> key){
    y -> left = new_node;
  }

  else {
    y -> right = new_node;
  }

  new_node -> key = key;
  new_node -> left = t->nil;
  new_node -> right = t->nil;
  new_node -> color = RBTREE_RED;
  
  rbtree_insert_fixup(t, new_node);

  return t->root;
}


/*
FIND
*/
// find main
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *x = t ->root;
  while(x != t->nil){
    if (x->key > key){
      x = x->left;
    }
    else if (x->key < key){
      x = x->right;
    }
    else{
      return x;
    }
  }
  return NULL;
}

/*
최대 최소
*/
// 최소값
node_t *rbtree_min(const rbtree *t) {
  node_t *x = t -> root;

  do{
    x = x->left;
  }while(x->left != t->nil);
  return x;
}

// 최대값
node_t *rbtree_max(const rbtree *t) {
  node_t *x = t -> root;
  do{
    x = x->right;
  }while(x->right != t->nil);
  return x;
}

/*
erase
*/
// rb-tree 삭제후 노드 대체 함수
void rb_transplant(rbtree *t, node_t *u, node_t *v){
  if (u->parent == t->nil){
    t->root = v;
  }
  else if (u == u->parent->left){
    u->parent->left = v;
  }
  else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

// 노드 기준 우측의 최소값을 추출
node_t* tree_minimum(rbtree *t, node_t *x){
  while (x->left != t->nil){
    x = x->left;
  }
  return x;
}

// erase 색깔 변경
void rb_delete_fixup(rbtree *t, node_t *x){
  node_t *w;
  while (x != t->root && x->color==RBTREE_BLACK){
    if (x== x->parent->left){
      w = x->parent->right;
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w=x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if(w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w); 
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }

    }
    else {
      w = x->parent->left;
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w=x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if(w->left->color == RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w); 
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

// erase 메인
int rbtree_erase(rbtree *t, node_t *p) {
  node_t *y = p;
  node_t *x;
  color_t y_original_color = y->color;
  if (p->left == t->nil){
    x = p->right;
    rb_transplant(t, p, p->right);
  }
  else if(p->right== t->nil){
    x = p ->left;
    rb_transplant(t, p, p->left);
  }
  else{
    y = tree_minimum(t,p->right);
    y_original_color = y->color;
    x = y->right;

    if (y->parent == p){
      x->parent = y;
    }
    else {
      rb_transplant(t, y, y->right);
      y -> right = p -> right;
      y -> right -> parent = y;
    }
    rb_transplant(t, p, y);
    y->left = p -> left;
    y->left->parent = y;
    y->color = p -> color;
  }
  // free(p);
  if (y_original_color == RBTREE_BLACK){
    rb_delete_fixup(t, x);
  }
  free(p);
  
  return 0;
}


/*
rbtree to array
*/
// 중위순회 arr input
void inorder_search(const rbtree *t, key_t *arr, size_t n, node_t *now_node){
  if (count > n || now_node == t->nil){
    return;
  }

  inorder_search(t, arr, n, now_node->left);
  arr[count++] = now_node->key;
  inorder_search(t, arr, n, now_node->right);
}

// rbtree to array main
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  count = 0;
  inorder_search(t, arr, n, t->root);
  return 0;
}


