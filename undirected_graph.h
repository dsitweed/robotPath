#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "jrb.h"
#include "dllist.h"

typedef JRB Graph;

Graph createGraph();
void addEdge(Graph graph, int v1, int v2);
int adjacent(Graph graph, int v1, int v2);//kiem tra co v1 - v2 ?
int getAdjacentVertices(Graph graph, int vertex, int* output);//cac canh lien ket voi vertex
void dropGraph(Graph graph);

void BFS(Graph graph, int start, int stop, void (*func)(int));
void DFS(Graph graph, int start, int stop, void (*func)(int));

void printVertex(int v)
{
     printf("%4d", v);
}

Graph createGraph()
{
   return make_jrb();
}

void addEdge(Graph graph, int v1, int v2)
{
     JRB node, tree;
     if (!adjacent(graph, v1, v2))
     {
        node = jrb_find_int(graph, v1);
        if (node==NULL) {
           tree = make_jrb();
           jrb_insert_int(graph, v1, new_jval_v(tree));
        } else
        {
           tree = (JRB) jval_v(node->val);   
        }
        jrb_insert_int(tree, v2, new_jval_i(1));
     }//V1->V2
     
     if (!adjacent(graph, v2, v1))
     {
        node = jrb_find_int(graph, v2);
        if (node==NULL) {
           tree = make_jrb();
           jrb_insert_int(graph, v2, new_jval_v(tree));
        } else
        {
           tree = (JRB) jval_v(node->val);   
        }
        jrb_insert_int(tree, v1, new_jval_i(1));   
     }//V2->V1
}

int adjacent(Graph graph, int v1, int v2)
{
    JRB node, tree;
    node = jrb_find_int(graph, v1);
    if (node==NULL)
       return 0;
    tree = (JRB) jval_v(node->val);
    if (jrb_find_int(tree, v2)==NULL)
       return 0;
    else
       return 1;       
}

int getAdjacentVertices (Graph graph, int v, int* output)
{
    JRB tree, node;
    int total;
    node = jrb_find_int(graph, v);
    if (node==NULL)
       return 0;
    tree = (JRB) jval_v(node->val);
    total = 0;   
    jrb_traverse(node, tree)
    {
       output[total] = jval_i(node->key);
       total++;                
    }
    return total;   
}

void dropGraph(Graph graph)
{
    JRB node, tree;
    jrb_traverse(node, graph)
    {
        tree = (JRB) jval_v(node->val);
        jrb_free_tree(tree);
    }
    jrb_free_tree(graph);
}

void BFS(Graph graph, int start, int stop, void (*func)(int))
{
   int visited[1000] = {};
   int output[100],u,number;
   Dllist node, queue = new_dllist();
   
   visited[start] = 1;
   dll_append(queue,new_jval_i(start));
   while (dll_empty(queue) == 0)
     {
       node = dll_first(queue);
       u = jval_i(node->val);
       func(u);
       dll_delete_node(node);
       if (u == stop) return;
       number = getAdjacentVertices(graph,u,output);
       for (int i = 0;i < number;i++)
	 {
	   u = output[i];
	   if (visited[u] == 0)
	     {
	       visited[u] = 1;
	       dll_append(queue,new_jval_i(u));
	     }
	 }//end for
     }//end while
}

void DFS(Graph graph, int start, int stop, void (*func)(int))
{
   int visited[1000] = {};
   int output[100],u,number;
   Dllist node, stack;
   stack = new_dllist();
   
   visited[start] = 1;
   dll_prepend(stack,new_jval_i(start));
   while (dll_empty(stack) == 0)
     {
       node = dll_first(stack);
       u = jval_i(node->val);
       func(u);
       dll_delete_node(node);
       if (u == stop) return;
       number = getAdjacentVertices(graph,u,output);
       for (int i = 0;i < number;i++)
	 {
	   u = output[i];
	   if (visited[u] == 0)
	     {
	       visited[u] = 1;
	       dll_prepend(stack,new_jval_i(u));
	     }
	 }//end for
     }//end while                       
}
