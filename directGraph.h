#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jrb.h"
#include "dllist.h"
#define max 1000
typedef struct
{
	JRB edges;
	JRB vertices;
	JRB search;
} Graph;
typedef struct
{
	int id;			//id - xac dinh diem dung xe buyt
	char name[200]; //Ten diem dung xe buyt
	char bus[20];	//Xe buyt di qua diem dung nay
} Post;				//dung de su li mang diem dung xe buyt

int ID; //ma id cho cac diem dung xe buyt;
//int -- 2^31 - 1 thoai mai
//max is define (1000 maybe enough)

//idea is buil a graph; key is bus(name of bus);Value is a BUS type have all bus bus station
//Also store time average and all diffrence time each bus route
typedef struct
{
	char busStationDi[200][100]; //chieu di
	char busStationVe[200][100]; //chieu ve
	float timeAv;				 //Time average
} BUS;

Graph createGraph();
void addVertex(Graph graph, char *vertex);
char *getBus(Graph graph, int v1, int v2);
int getId(Graph graph, char *busStation);
void addEdge(Graph graph, int v1, int v2, char *bus);
int hasEdge(Graph graph, int v1, int v2);
int indegree(Graph graph, int vertex, Post *output);
int outdegree(Graph graph, int vertex, Post *output);
void dropGraph(Graph graph);

int BFS(Graph graph, int start, int stop, int *prev);

int DAG(Graph graph);
void topologicalSort(Graph graph, int *output, int *n);
//Project2 func
void delSpace(char *string);
void inputData(Graph graph, int total, Post *post, char *bus);
int shortestPath(Graph graph, int start, int stop, int *prev);
void readData(Graph graph, char *ftext);
void inputGraphBus(JRB graphBus, char *ftext);
void printVertex(char *v)
{
	printf("%s\n", v);
}

Graph createGraph()
{
	Graph graph;
	graph.edges = make_jrb();
	graph.vertices = make_jrb();
	graph.search = make_jrb();
	return graph;
}
void addVertex(Graph graph, char *v1)
{
	JRB node;
	node = jrb_find_str(graph.vertices, v1);
	if (node == NULL)
	{
		jrb_insert_str(graph.vertices, strdup(v1), new_jval_i(ID));
		ID++; //bien toan cuc ID
	}
}

void addForSearch(Graph graph, int id, char *name)
{
	JRB node;
	node = jrb_find_int(graph.search, id);
	if (node == NULL) //Neu id chua co
		jrb_insert_int(graph.search, id, new_jval_s(strdup(name)));
}

int getId(Graph graph, char *vertex)
{
	JRB node;
	node = jrb_find_str(graph.vertices, vertex);
	if (node == NULL)
		return -999999;
	return jval_i(node->val);
}
void addEdge(Graph graph, int v1, int v2, char *bus)
{
	JRB node, treeL1, treeL2; //L1,L2 is the deep of graph
	if (hasEdge(graph, v1, v2) == 0)
	{
		node = jrb_find_int(graph.edges, v1);
		if (node == NULL)
		{
			treeL1 = make_jrb();
			jrb_insert_int(graph.edges, v1, new_jval_v(treeL1));
		}
		else //have node - v1
		{
			treeL1 = (JRB)jval_v(node->val);
		}
		treeL2 = make_jrb();
		jrb_insert_int(treeL1, v2, new_jval_v(treeL2));
	} // V1 -> V2
	else
	{
		node = jrb_find_int(graph.edges, v1);
		treeL1 = (JRB)jval_v(node->val);
		node = jrb_find_int(treeL1, v2);
		treeL2 = (JRB)jval_v(node->val);
	}
	node = jrb_find_str(treeL2, bus);
	if (node == NULL)
	{
		jrb_insert_str(treeL2, strdup(bus), new_jval_i(1));
	}
}

int hasEdge(Graph graph, int v1, int v2)
{
	JRB node, tree;
	node = jrb_find_int(graph.edges, v1);
	if (node == NULL)
		return 0;
	tree = (JRB)jval_v(node->val);
	node = jrb_find_int(tree, v2);
	if (node == NULL)
		return 0;
	else
		return 1;
}

int outdegree(Graph graph, int vertex, Post *output)
{
	JRB node, tree;
	int total = 0;
	node = jrb_find_int(graph.edges, vertex);
	if (node == NULL)
		return 0;
	tree = (JRB)jval_v(node->val);
	jrb_traverse(node, tree)
	{
		output[total].id = jval_i(node->key);
		strcpy(output[total].bus, jval_s(node->val));
		total++;
	}
	return total;
}
int indegree(Graph graph, int vertex, Post *output)
{
	JRB node, tree, u;
	int total = 0;
	jrb_traverse(node, graph.edges)
	{
		tree = (JRB)jval_v(node->val);
		u = jrb_find_int(tree, vertex);
		if (u != NULL)
		{
			output[total].id = jval_i(node->key);
			strcpy(output[total].bus, jval_s(u->val));
			total++;
		}
	} //end traverse
	return total;
}

int BFS(Graph graph, int start, int stop, int *prev)
{
	int v, u, number;
	Dllist node, queue = new_dllist();
	Post output[max]; //size co lon o day la bi kill ngay
	for (int i = 0; i < max; i++)
		prev[i] = -2;
	// -1 la ket thuc de truy vet
	dll_append(queue, new_jval_i(start));
	prev[start] = -1;
	while (dll_empty(queue) == 0)
	{
		node = dll_first(queue);
		v = jval_i(node->val); //Id cua node me
		//printf("\n%-4d",v);
		if (v == stop)
			return 1;
		dll_delete_node(node);
		number = outdegree(graph, v, output); //#####
		for (int i = 0; i < number; i++)
		{
			u = output[i].id;
			//  printf("%-3d",u);
			if (prev[u] == -2)
			{
				dll_append(queue, new_jval_i(u));
				prev[u] = v;
			}
		}
	}		  //end while
	return 0; //Can't find or can't connect
}

void inputGraphBus(JRB graphBus, char *ftext)
{
	BUS *buff;
	FILE *a = fopen(ftext, "r");
	char run[100], bus[20]; //just for save
	int total, tansuat;		//just count
	int first;				//for fist loop
	if (a == NULL)
	{
		printf("ERROR INPUT DATA\n");
		return;
	}
	first = 0;
	while (1)
	{
		buff = (BUS *)malloc(sizeof(BUS));
		if (first == 0)
		{ //Just do down func in first loop
			fgets(run, 100, a);
			delSpace(run);
			first = 1;
		} //second loop -> end each loop run == "bus"
		//printf("%s\n",run);
		if (strcmp(run, "end") == 0) //end file
		{
			fclose(a);
			return;
		} //this func has't affect, real end at below
		if (strcmp(run, "bus") == 0)
		{
			fgets(bus, 20, a);
			delSpace(bus); //Read bonus 1 line
			fgets(run, 100, a);
			delSpace(run); //Read "tan_suat"
		}				   // read name of bus
		if (strcmp(run, "tan_suat") == 0)
		{
			total = 0;
			(*buff).timeAv = 0;
			while (fscanf(a, "%d%*c", &tansuat) != 0)
			{
				(*buff).timeAv += tansuat;
				total++;
				//printf("%d\n",tansuat);
			}
			(*buff).timeAv /= total;
			fgets(run, 100, a);
			delSpace(run); //next -> chieu_di
		}				   //read time have 1 bus
		if (strcmp(run, "chieu_di") == 0)
		{
			total = 0;
			while (1)
			{
				fgets(run, 100, a);
				delSpace(run);
				//printf("%s\n",run);
				if (strcmp(run, "chieu_ve") != 0)
				{
					strcpy((*buff).busStationDi[total], run);
					total++;
				}
				else //if run == "chieu_ve"
				{
					break;
				}
			} //end while chieu_di
			total = 0;
			while (1)
			{
				fgets(run, 100, a);
				delSpace(run);
				//printf("%s\n",run);
				if ((strcmp(run, "bus") != 0) && (strcmp(run, "end") != 0))
				{
					strcpy((*buff).busStationVe[total], run);
					total++;
				}
				else
				{
					jrb_insert_str(graphBus, strdup(bus), new_jval_v(buff)); //input chieu_ve
					if (strcmp(run, "end") == 0)
					{
						fclose(a);
						return;
					}
					if (strcmp(run, "bus") == 0)
						break;
				}
			} //end while chieu_ve
		}	  //read chieu_di -> read chieu_ve one time
			  //when end 1 loop -> run == "bus"
	}		  //end while
}
void dropGraph(Graph graph)
{
	JRB node, tree;
	jrb_traverse(node, graph.edges)
	{
		tree = (JRB)jval_v(node->val);
		jrb_free_tree(tree);
	}
	jrb_free_tree(graph.vertices);
	jrb_free_tree(graph.edges);
	jrb_free_tree(graph.search);
}

void delSpace(char *name)
{
	while ((name[strlen(name) - 1] == ' ') || (name[strlen(name) - 1] == '\n') || (name[strlen(name) - 1] == '\r'))
	{
		name[strlen(name) - 1] = '\0';
	}
}
