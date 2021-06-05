#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "jrb.h"
#include "dllist.h"

typedef struct {
   JRB edges;
   JRB vertices;
} Graph;

Graph createGraph();
void addVertex(Graph graph, int id, char* name);
char *getVertex(Graph graph, int id);
void addEdge(Graph graph, int v1, int v2);
int hasEdge(Graph graph, int v1, int v2);
int indegree(Graph graph, int v, int* output);
int outdegree(Graph graph, int v, int* output);
void dropGraph(Graph graph);

int DAG(Graph graph);
void topologicalSort(Graph g, int* output, int* n);

Graph createGraph()
{
   Graph g; 
   g.edges = make_jrb();  
   g.vertices = make_jrb();  
   return g;
}

void addVertex(Graph g, int id, char* name)
{
     JRB node = jrb_find_int(g.vertices, id);
     if (node==NULL) // only add new vertex 
         jrb_insert_int(g.vertices, id, new_jval_s(strdup(name)));            
}

char *getVertex(Graph g, int id)
{
     JRB node = jrb_find_int(g.vertices, id);
     if (node==NULL) 
        return NULL;
     else                
        return jval_s(node->val);
}     

void addEdge(Graph graph, int v1, int v2)
{
     JRB node, tree;
     if (!hasEdge(graph, v1, v2))
     {
        node = jrb_find_int(graph.edges, v1);
        if (node==NULL) {
           tree = make_jrb();
           jrb_insert_int(graph.edges, v1, new_jval_v(tree));
        } else
        {
           tree = (JRB) jval_v(node->val);   
        }
        jrb_insert_int(tree, v2, new_jval_i(1));
     }//V1->V2
}

int hasEdge(Graph graph, int v1, int v2)
{
    JRB node, tree;
    node = jrb_find_int(graph.edges, v1);
    if (node==NULL)
       return 0;
    tree = (JRB) jval_v(node->val);
    if (jrb_find_int(tree, v2)==NULL)
       return 0;
    else
       return 1;       
}

int indegree (Graph graph, int v, int* output)
{
    JRB tree, node;
    int total = 0;   
    jrb_traverse(node, graph.edges)
    {
       tree = (JRB) jval_v(node->val);
       if (jrb_find_int(tree, v))
       {
          output[total] = jval_i(node->key);
          total++;
       }                
    }
    return total;   
}

int outdegree (Graph graph, int v, int* output)
{
    JRB tree, node;
    int total;
    node = jrb_find_int(graph.edges, v);
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

int DAG(Graph graph)
{
  int visited[1000];
  int output[100],u,number, start;
  Dllist node, stack;
  JRB vertex;
  jrb_traverse(vertex,graph.vertices)
    {
      memset(visited,0,sizeof(visited));
      stack = new_dllist();
      start = jval_i(vertex->key);
      visited[start] = 1;
      dll_prepend(stack,new_jval_i(start));
      while (dll_empty(stack) == 0)
	{
	  node = dll_first(stack);
	  u = jval_i(node->val);
	  dll_delete_node(node);
	  number = outdegree(graph,u,output);
	  for (int i = 0;i < number;i++)
	    {
	      u = output[i];
	      if (u == start) return 0;
	      if (visited[u] == 0)
		{
		  visited[u] = 1;
		  dll_prepend(stack,new_jval_i(u));
		}
	    }//end for
	}//end while
    }
  return 1; // no cycle    
}

void dropGraph(Graph graph)
{
    JRB node, tree;
    jrb_traverse(node, graph.edges)
    {
        tree = (JRB) jval_v(node->val);
        jrb_free_tree(tree);
    }
    jrb_free_tree(graph.edges);
    jrb_free_tree(graph.vertices);
}

void topologicalSort(Graph graph, int* output, int* n)
{
  if (DAG(graph) == 0)
    {
      printf("Have cycles in the graph\n");
      printf("Have't topological sort\n");
    }
  JRB vertex;
  int indeg[1000],u,total,number;
  int tmp[100],v;
  Dllist queue,node;
  queue = new_dllist();
  jrb_traverse(vertex,graph.vertices)
    {
      u = jval_i(vertex->key);
      indeg[u] = indegree(graph,u,tmp);
      if (indeg[u] == 0)
	dll_append(queue,new_jval_i(u));
    }
  total = 0;
  while (dll_empty(queue) == 0)
    {
      node = dll_first(queue);
      u = jval_i(node->val);
      dll_delete_node(node);
      output[total++] = u;
      number = outdegree(graph,u,tmp);
      for (int i = 0;i < number;i++)
	{
	  v = tmp[i];
	  indeg[v]--;
	  if (indeg[v] == 0)
	    dll_append(queue,new_jval_i(v));
	}//end for
    }//end while
  *n = total;
}
