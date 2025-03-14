/*Rossi Davide 0001116890 Classe A davide.rossi47@studio.unibo.it*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>


typedef struct{

    int id; /*valore numerico per identificare i nodi del grafo, va da 0 a n nodi -1*/
    long int cost;

}HeapElement;


typedef struct{

    int * positions;/*per ogni elemento dell'heap memorizza la posizione nell'array heap, -1 se non presente*/
    HeapElement * heap;
    int n;
    int size;
}MinHeap;

/*grafo per rappresentare la matrice dell'input*/
typedef struct Edge{
    int source;
    int dest;
    int weight;
    struct Edge * next;
}Edge;

typedef struct{

    Edge ** edges;
    int n; /*per ogni cella ho un nodo*/
    int m;/*ogni cella ha un massimo di 4 archi che rappresentano le strade verso le celle adiacenti*/

    int cell_cost;
    int height_cost;
    int rows;
    int cols;

}Graph;




int get_parent(MinHeap * heap,int pos){

    assert(pos>=0 && pos<heap->n);

    if(pos == 0){
        return -1;
    }

    return (pos+1)/2-1;
}


int get_left_child(MinHeap * heap,int pos){

    assert(pos>=0 && pos<heap->n);


    return (pos*2+1>heap->n) ? -1 : pos*2+1;

}



int get_right_child(MinHeap * heap,int pos){

    assert(pos>=0 && pos<heap->n);

    return  (pos*2+2>heap->n) ? -1 : pos*2+2;
}


/*scambia due elementi nell'heap aggiornando il vettore delle posizioni*/
void swap(MinHeap * heap,int p1,int p2){

    HeapElement tmp;

    assert(p1>=0 && p1<heap->n && p2>=0 && p2<heap->n);

    tmp = heap->heap[p1];

    heap->heap[p1] = heap->heap[p2];
    heap->heap[p2] = tmp;


    heap->positions[heap->heap[p1].id] = p1;
    heap->positions[heap->heap[p2].id] = p2;
}

/*fa risalire un elemento nell'heap fino 
  alla posizione corretta*/
void move_up(MinHeap * heap,int pos){

    int pos_parent;

    assert(pos>=0 && pos<heap->n);

    /*sono alla radice, non posso salire di più*/
    if(pos == 0){
        return;
    }

    pos_parent = get_parent(heap,pos);


    if(heap->heap[pos_parent].cost>heap->heap[pos].cost){
        swap(heap,pos_parent,pos);
        move_up(heap,pos_parent);
    }

}

/*fa scendere un elemento nell'heap fino alla sua
posizione corretta*/
void move_down(MinHeap * heap,int pos){
    
    int pos_left,pos_right;
    
    assert(pos>=0 && pos<heap->n);

    pos_left = get_left_child(heap,pos);

    if(pos_left == -1) return;

    if(heap->heap[pos_left].cost<heap->heap[pos].cost){
        swap(heap,pos_left,pos);

        move_down(heap,pos_left);

    }


    pos_right = get_right_child(heap,pos);

    if(pos_right == -1) return;
    
    
    if(heap->heap[pos_right].cost<heap->heap[pos].cost){
        swap(heap,pos_right,pos);

        move_down(heap,pos_right);
    }


}

int minheap_is_empty(MinHeap * minheap){
    return (minheap->n<=0) ? 1 : 0;
}



/*Crea un heap vuoto di dimensione size*/
MinHeap * minheap_create(int size){

    MinHeap * heap;
    int i;

    heap = (MinHeap *)malloc(sizeof(MinHeap));
    assert(heap!= NULL);

    heap->n = 0;
    heap->positions = (int *)malloc(sizeof(int)*size);
    heap->heap = (HeapElement *)malloc(sizeof(HeapElement)*size);
    heap->size = size;

    assert(heap->positions!= NULL);
    assert(heap->heap!= NULL);



    for(i=0;i<heap->size;i++){
        heap->positions[i] = -1;
    }


    return heap;


}

/*inserisce un nuovo elemento nell'heap partendo dal fondo e facendolo risalire*/
void minheap_insert(HeapElement new_el, MinHeap * heap){


    assert(heap!=NULL);
    assert(heap->n<heap->size);

    heap->positions[new_el.id] = heap->n;

    heap->heap[heap->n] = new_el;

    heap->n++;
    move_up(heap,heap->n-1);

 
}


/*estrae la testa dell'heap e riaggiorna la struttura*/
HeapElement minheap_extract_min(MinHeap * heap){

    HeapElement el_return;

    el_return = heap->heap[0];

    heap->n--;

    heap->positions[heap->heap[0].id] = -1;

    if(heap->n>0){
        heap->heap[0] = heap->heap[heap->n];
        heap->positions[heap->heap[0].id] = 0;
    
        move_down(heap,0);
    }

    return el_return;
}


/*modifica la priorità di un nodo e riaggiorna la struttura*/
void change_cost(MinHeap * heap,int id,long int new_cost){
    
    int pos;
    long int tmp_cost;

    pos = heap->positions[id];

    tmp_cost = heap->heap[pos].cost;


    heap->heap[pos].cost = new_cost;



    if(new_cost>tmp_cost){

        move_down(heap,pos);

    }else{

        move_up(heap,pos);
    }


}

/*ritorna un puntatore all'elemento dell'heap con tale id*/
HeapElement * minheap_getby_id(int id, MinHeap * minheap){

    int pos;

    pos = minheap->positions[id];

    if(pos == -1){
        return NULL;
    }

    return &minheap->heap[pos];

}


void destroy_minheap(MinHeap * minheap){

    free(minheap->heap);
    free(minheap->positions);
    free(minheap);
}


/*crea un grafo con n nodi*/
Graph *  create_graph(int n){

    Graph * graph;
    int i;

    graph = (Graph *)malloc(sizeof(Graph));
    assert(graph!= NULL);

    graph->edges = (Edge **)malloc(sizeof(Edge *)*n);

    assert(graph->edges!= NULL);


    for(i=0; i<n; i++){
        graph->edges[i] = NULL;
    }

    graph->n = n;
    graph->m = 0;
    graph->cell_cost = 0;
    graph->height_cost = 0;
    graph->cols = 0;
    graph->rows = 0;

    return graph; 
}


/*aggiunge un arco alla lista di adiacenza del nodo source*/
void add_edge(Graph * G,int source,int destination, double weight){

    Edge * new_edge;

    assert(source>=0 && source < G->n);
    assert(destination>=0 && destination < G->n);

    new_edge = (Edge *)malloc(sizeof(Edge));
    assert(new_edge!=NULL);

    new_edge->source = source;
    new_edge->dest = destination;
    new_edge->weight = weight;

    new_edge->next = G->edges[source];
    G->edges[source] = new_edge;
    G->m++;
}

int get_weight(int v1,int v2,int c_height){


    return c_height*(v1-v2)*(v1-v2);

}

/*crea un grafo a partire dal file di input che descriva la matrice*/
Graph * create_fill_file(FILE * file){

    int c_cell;
    int c_height;
    int rows;
    int cols;
    Graph * g;
    int i,j;
    int weight;
    int ** support_matrix;

    assert(file!=NULL);

    fscanf(file,"%d",&c_cell);
    fscanf(file,"%d",&c_height);
    fscanf(file,"%d",&rows);
    fscanf(file,"%d",&cols);

    /*crea un grafo con n nodi = n celle*/
    g = create_graph(rows*cols);
    g->cell_cost = c_cell;
    g->height_cost = c_height;
    g->rows = rows;
    g->cols = cols;

    /*leggo la matrice dal file e la salvo*/
    support_matrix = (int **)malloc(sizeof(int *)*rows);
    assert(support_matrix!= NULL);

    for(i = 0;i<rows;i++){
        support_matrix[i] = (int *)malloc(sizeof(int)*cols);
        assert(support_matrix[i]!= NULL);
    }


    for(i=0; i<rows; i++){
        for(j=0; j<cols; j++){

            fscanf(file,"%d",&support_matrix[i][j]);
        }
    }


    /*converto la matrice in un grafo, per ogni cella guardo quali celle vi sono adiacenti e 
    aggiungo degli archi per ogni cella adiacente
    Il peso dell'arco è dato dal dislivello al quadrato *c_height */
    for(i=0; i<rows; i++){
        for(j = 0;j<cols;j++){


            if(i>0){

                weight  = get_weight(support_matrix[i][j],support_matrix[i-1][j],c_height);

                add_edge(g,cols*i+j,cols*(i-1)+j,weight);

            }

            if(i<(rows-1)){

                weight = get_weight(support_matrix[i][j],support_matrix[i+1][j],c_height);
                add_edge(g,cols*i+j,cols*(i+1)+j,weight);


            }

            if(j>0){
                weight = get_weight(support_matrix[i][j],support_matrix[i][j-1],c_height);
                add_edge(g,cols*i+j,cols*i+j-1,weight);
            }


            if(j<(cols-1)){
                weight = get_weight(support_matrix[i][j],support_matrix[i][j+1],c_height);
                add_edge(g,cols*i+j,cols*i+j+1,weight);
            }
        }
    }  


    for(i = 0;i<rows;i++){
        free(support_matrix[i]);
    }

    free(support_matrix);


    return g;
}


void destroy_edge_list(Edge *edge){

    if(edge == NULL) return;


    destroy_edge_list(edge->next);

    free(edge);


}


void destroy_graph(Graph *G){

    int i;

    for(i=0;i<G->n;i++){

         destroy_edge_list(G->edges[i]);

    }

    free(G->edges);
    free(G);


}


void relax(MinHeap * minHeap,Edge * edge,long int source_cost,int step_cost,long int ** prec){

    HeapElement * dest_heap;
    long int new_cost;

    /*trovo il nodo nell'heap per prenderne il costo*/
    dest_heap = minheap_getby_id(edge->dest,minHeap);

    if(dest_heap == NULL) return;


    /*calcolo il nuovo costo tenendo conto di c_cell*/
    new_cost = source_cost+(edge->weight)+step_cost;


    if(dest_heap->cost>new_cost){

        /*se avviene rilassamento aggiorno heap e array predecessori*/
        prec[dest_heap->id][0] = edge->source;
        prec[dest_heap->id][1] = new_cost;

        change_cost(minHeap,dest_heap->id,new_cost);

    }


}


void dijkstra(Graph * G,int source,long int ** prec){

    MinHeap * minHeap;
    int i;

    /*creo un heap con i nodi del grafo*/
    minHeap = minheap_create(G->n);

    for(i=0;i<G->n;i++){

        if(i!=source){

            HeapElement new_el = {0,INT_MAX};
            new_el.id = i;
            minheap_insert(new_el,minHeap);

        }else{

            HeapElement source_el; 
            source_el.id = source;
            source_el.cost = G->cell_cost;

            minheap_insert(source_el,minHeap);

        }

    } 



    while(!minheap_is_empty(minHeap)){

        HeapElement minimum;
        Edge * tmp;
        
        minimum = minheap_extract_min(minHeap);

        /*scorro la linked list degli archi uscenti dal nodo con costo minimo*/
        tmp = G->edges[minimum.id];

        while (tmp!=NULL)
        {

            relax(minHeap,tmp,minimum.cost,G->cell_cost,prec);

            tmp = tmp->next;
        }
    }


    destroy_minheap(minHeap);

}


/*percorre l'albero dei cammini memorizzato in prec minimi partendo dalla destinazione fino 
alla sorgente*/
void print_path_recursive(long int ** prec,int pos,int rows,int cols){

    int row,col;

    if(pos==-1){
        return;
    }


    print_path_recursive(prec,prec[pos][0],rows,cols);

    /*riconverte l'id associato a un nodo nella sua
    posizione nella matrice (riga, colonna)*/

    row = pos/cols;
    col = pos%cols;

    printf("%d %d\n",row,col);

}


void print_output(long int ** prec,int pos,int rows,int cols){

    print_path_recursive(prec,pos,rows,cols);

    printf("-1 -1\n");;
    printf("%ld\n",prec[pos][1]);
}


int main(int argc,char * argv[]){

    Graph * G;
    int i;
    FILE * f;
    long int ** prec;


    if(argc!=2){
        fprintf(stderr,"Too many or too few arguments\n");
    }

    f = fopen(argv[1],"r");
    assert(f!=NULL);

    /*creo un grafo associato alla matrice in input, i nodi sono le celle e gli archi 
    i collegamenti verso le celle adiacenti*/
    G = create_fill_file(f);

    /*array di predecessori*/
    prec = (long int **)malloc(sizeof(long int*)*G->n);

    assert(prec!=NULL);

    /*per ogni nodo i da 1 a n viene memorizzato il nodo precedente nell'albero
    dei cammini minimi e il costo per arrivare a tale nodo*/
    for(i=0; i<G->n; i++){
        prec[i] = (long int *)malloc(sizeof(long int)*2);
        assert(prec[i]!=NULL);
        prec[i] [0] = -1;
        prec[i] [1] = -1;
    }


    /*albero cammini minimi single source dalla prima cella in alto a sx*/
    dijkstra(G,0,prec);

    
    /*stampo cammino da sorgente all' ultima cella in basso a dx (nel grafo nodo con id n-1)*/
    print_output(prec,G->n-1,G->rows,G->cols);


    for(i=0; i<G->n; i++){

        free(prec[i]);
    }

    free(prec);


    destroy_graph(G);

    fclose(f);


    return EXIT_SUCCESS;
}
