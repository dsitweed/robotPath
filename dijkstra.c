#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define INFINITIVE_VALUE 10000000
#include "jrb.h"
#include "dllist.h"

typedef struct {
  JRB edges;
  JRB vertices;
  JRB run;
} Graph;

Graph createGraph();
void addVertex(Graph graph, int v1, int v2,double r);
int getID(Graph graph,char *s);
void addEdge(Graph graph, int v1, int v2, double weight);
double getEdgeValue(Graph graph,int v1, int v2);

int outdegree(Graph graph, int v, int* output);
int outdegreeRun (Graph graph, int v, int* output);
void dropGraph(Graph graph);

double shortestPath(Graph graph, int start, int stop, int* path, int*length);//length = so phan tu cua path
void setRunVer(int v1, int v2,char s[]);
int haveRunEdge(Graph graph,int id1,int id2);
void addRunEdge(Graph graph,int id1,int id2);
void addRun(Graph graph,double r);
int BFS(Graph graph, int start, int stop,int *path,int *length);
void itoa(int n, char s[]);

int ID;//ID cho graph.run ()

int main(int argc,char *argv[])
{
  int start, stop, path[100], length;
  int n,m,a,b,c,d;
  //Di chuyen theo huong: a->c ; b->d
  //r khoang cach toi thieu cua 2 robot
  int v1,v2,output[100];
  double weight,distance,r;
  char s1[20] = {}, s2[20] = {};
  Graph graph = createGraph();
  JRB node,tree,buff;
  if (argc != 2)
    {
      printf("Nhap theo cu phap: ten_chuong_trinh bo_test\n");
      return 0;
    }
  FILE *file = fopen(argv[1],"r");
  if (file == NULL)
    {
      printf("ERROR INPUT\n");return 0;
    }
  printf("IN PUTDATA from %s\n",argv[1]);
  fscanf(file,"%d %d%*c",&n,&m);//read n,m number of vertices, number of edges
  printf("%d %d\n",n,m);
  for (int i = 0;i < m;i++)
    {
      fscanf(file,"%d %d %lf%*c",&v1,&v2,&weight);
      printf("%d %d %.2lf\n",v1,v2,weight);
      addEdge(graph,v1,v2,weight);
    }
  fscanf(file,"%d %d%*c",&a,&b);//start point
  fscanf(file,"%d %d%*c",&c,&d);//stop point
  fscanf(file,"%lf",&r);
  fclose(file);
  printf("%d %d\n%d %d\n%lf\n",a,b,c,d,r);
  //add in graph
  ID = 1;
  for (int i = 0;i < n;i++)
    {
      for (int j = 0;j < n;j++)
	{
	  if (i != j) addVertex(graph,i,j,r);
	}
    }
  
  addRun(graph,r);
  //printf("OK\n");
  setRunVer(a,b,s1);//start Vertex
  setRunVer(c,d,s2);//stop Vertex
  start = 0; stop = 0;//ID = 0 no have't mean
 
  jrb_traverse(node,graph.vertices)
    {
      if(strcmp(s1,jval_s(node->val)) == 0)
	start = jval_i(node->key);
      if(strcmp(s2,jval_s(node->val)) == 0)
	stop = jval_i(node->key);
      if ((start != 0) && (stop != 0)) break;
    }
  // jrb_traverse(node,graph.vertices)
  //printf("%d %s\n",jval_i(node->key),jval_s(node->val));
  //printf("%d %d\n",start,stop);
  printf("BFS: %d\n",BFS(graph,start,stop,path,&length));
  printf("Duong di tu %s ==> %s\n",s1,s2);
  printf("%-20s %s\n","Lich di chuyen", "Khoang cach");
  for (int i = 0;i < length;i++)
    {
      node = jrb_find_int(graph.vertices,path[i]);
      strcpy(s1,jval_s(node->val));
      sscanf(s1,"%d %d",&v1,&v2);
      weight = shortestPath(graph,v1,v2,output,&m);
      printf("%-20s %.2lf\n",jval_s(node->val),weight);
    }
}//end main


Graph createGraph()
{
   Graph g; 
   g.edges = make_jrb();  
   g.vertices = make_jrb();
   g.run = make_jrb();
   return g;
}

void addVertex(Graph graph, int v1, int v2,double r)
{
  char s[20];
  JRB node;
  double weight;
  int length, path[1000];
  setRunVer(v1,v2,s);
  jrb_traverse(node,graph.vertices)
    {
      if (strcmp(s,jval_s(node->val)) == 0) return;
    }
  weight = shortestPath(graph,v1,v2,path,&length);
  if (weight > r)
    {
      jrb_insert_int(graph.vertices,ID,
		     new_jval_s(strdup(s)));
      ID++;
      //printf("%s\n",s);
    }
}
int getID(Graph g,char *s)
{
  JRB node;
  
  jrb_traverse(node,g.vertices)
    {
      if (strcmp(s,jval_s(node->val)) == 0)
	return jval_i(node->key);
    }
  return 0;
}     

void addEdge(Graph graph, int v1, int v2, double weight)
{
     JRB node, tree;
     if (getEdgeValue(graph, v1, v2) == INFINITIVE_VALUE)
     {
        node = jrb_find_int(graph.edges, v1);
        if (node==NULL) {
           tree = make_jrb();
           jrb_insert_int(graph.edges, v1, new_jval_v(tree));
        } else
        {
           tree = (JRB) jval_v(node->val);   
        }
        jrb_insert_int(tree, v2, new_jval_d(weight));
     }//V1->V2
     if (getEdgeValue(graph, v2, v1) == INFINITIVE_VALUE)
       {
	 node = jrb_find_int(graph.edges, v2);
	 if (node==NULL) {
	   tree = make_jrb();
	   jrb_insert_int(graph.edges, v2, new_jval_v(tree));
	 } else
	   {
	     tree = (JRB) jval_v(node->val);   
	   }
	 jrb_insert_int(tree, v1, new_jval_d(weight));
       }//V2->V1
}

double getEdgeValue(Graph graph,int v1, int v2)
{
  JRB node,tree;
  node = jrb_find_int(graph.edges,v1);
  if (node == NULL) return INFINITIVE_VALUE;
  tree = (JRB) jval_v(node->val);
  node = jrb_find_int(tree,v2);
  if (node == NULL) return INFINITIVE_VALUE;
  else return jval_d(node->val);
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

double shortestPath(Graph graph, int start, int stop, int* path, int*length)
{
  int output[100],number,u,v,buff;
  int prev[1000],tmp[1000];
  double value[1000],min,total;
  Dllist queue,node,prt;
  int check[1000] = {};
  for (int i = 0;i < 1000;i++)
    value[i] = INFINITIVE_VALUE;
  value[start] = 0;
  check[start] = 1;
  prev[start] = start;//Prev do lai duong di
  queue = new_dllist();
  dll_append(queue,new_jval_i(start));
  
  while (dll_empty(queue) == 0)
    {
      min = INFINITIVE_VALUE;
      dll_traverse(node,queue)
	{
	  buff = jval_i(node->val);
	  if (min > value[buff])
	    {
	      min = value[buff];
	      prt = node;//node se xet
	      u = buff;//u la id se duoc xu li
	    }
	}//queue traverse
      dll_delete_node(prt);
      
      if (u == stop) break;//stop at 'stop'
      
      number = outdegree(graph,u,output);
      for (int i = 0;i < number;i++)
	{
	  v = output[i];
	  min = value[u] + getEdgeValue(graph,u,v);
	  if (value[v] > min)
	    {
	      value[v] = min;
	      prev[v] = u;//Cha cua v la u
	    }
	  //printf("%d %d\n",v,check[v]);
	  if (check[v] == 0)
	    {
	      dll_append(queue,new_jval_i(v));
	      check[v] = 1;
	    }
	}
    }//end while

  total = value[stop];
  if (total != INFINITIVE_VALUE)
    {
      tmp[0] = stop;
      number = 1;
      while (stop != start)
	{
	  stop = prev[stop];
	  tmp[number++] = stop;
	}
      for (int i = number - 1;i >= 0;i--)
	path[number-1-i] = tmp[i];
      *length = number;
    }
  return total;
}//end function

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

void setRunVer(int v1, int v2,char s[])
{
  char string[20] = {},chuoi[20]={};
  long len;
  itoa(v1,string);
  len = strlen(string);
  string[len] = ' ';
  strcpy(s,string);
  itoa(v2,chuoi);
  strcat(s,chuoi);
  //printf("%d %d ==> %s\n",v1,v2,s);
}

int haveRunEdge(Graph graph,int id1,int id2)
{
  JRB node,tree;
  node = jrb_find_int(graph.run,id1);
  if (node == NULL) return 0;
  tree = (JRB) jval_v(node->val);
  node = jrb_find_int(tree,id2);
  if (node == NULL) return 0;
  else return 1;
}

void addRunEdge(Graph graph,int id1,int id2)
{
  JRB node,tree;
  if (haveRunEdge(graph,id1,id2) == 0)
    {
      node = jrb_find_int(graph.run,id1);
      if (node == NULL)
	{
	  tree = make_jrb();
	  jrb_insert_int(graph.run,id1,new_jval_v(tree));
	}
      else tree = (JRB) jval_v(node->val);
    }
  jrb_insert_int(tree,id2,new_jval_i(1));
}

void addRun(Graph graph,double r)
{
  JRB node,tree;
  int robot1,robot2,path[1000];
  int output[1000],n,u,length;
  char s1[20],s2[20];
  int id1,id2;
  double weight;
  
  jrb_traverse(node,graph.vertices)
    {
      strcpy(s1,jval_s(node->val));
      sscanf(s1,"%d %d",&robot1,&robot2);
      //printf("%d %d\n",robot1,robot2);
      id1 = jval_i(node->key);
      //xet hang xom cua robot1
      n = outdegree(graph,robot1,output);
      for (int i = 0;i < n;i++)
	{
	  u = output[i];
	  // printf("%d\n",u);
	  weight = shortestPath(graph,u,robot2,path,&length);
	  if (weight > r)
	    {
	      setRunVer(u,robot2,s2);
	      id2 = getID(graph,s2);
	      addRunEdge(graph,id1,id2);
	    }
	}
       
      //Xet hang xom cua robot2
      n = outdegree(graph,robot2,output);
      for (int i = 0;i < n;i++)
	{
	  u = output[i];
	  weight = shortestPath(graph,robot1,u,path,&length);
	  if (weight > r)
	    {
	      setRunVer(robot1,u,s2);
	      id2 = getID(graph,s2);
	      addRunEdge(graph,id1,id2);
	    }
	}
    }//end traverse
}

void itoa(int n, char s[])
 {
   int i,j, sign;
   char c;
   
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     
     for (i = 0, j = strlen(s)-1; i<j; i++, j--)
       {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
       }
 }

int outdegreeRun (Graph graph, int v, int* output)
{
    JRB tree, node;
    int total;
    node = jrb_find_int(graph.run, v);
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

int BFS(Graph graph, int start, int stop,int *path,int *length)
{
  int tmp[1000],prev[1000],visited[1000] = {};
  int output[100],u,v,number;
  Dllist node, queue = new_dllist();
   
   visited[start] = 1;
   prev[start] = start;
   dll_append(queue,new_jval_i(start));
   while (dll_empty(queue) == 0)
     {
       node = dll_first(queue);
       u = jval_i(node->val);
       //printf("%d\n",u);
       dll_delete_node(node);
       if (u == stop) break;
       number = outdegreeRun(graph,u,output);
       for (int i = 0;i < number;i++)
	 {
	   v = output[i];
	   if (visited[v] == 0)
	     {
	       prev[v] = u;
	       visited[v] = 1;
	       dll_append(queue,new_jval_i(v));
	     }
	 }//end for
     }//end while
   if (u == stop)
     {
       tmp[0] = stop;
       number = 1;
       while (stop != start)
	 {
	   //printf("%d\n",stop);
	   stop = prev[stop];
	   tmp[number++] = stop;
	 }
       for (int i = number - 1;i >= 0;i--)
	 path[number-1-i] = tmp[i];
       *length = number;
       return 1;
     }
   else return 0;
}
