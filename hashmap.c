#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
    char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap *map, char *key, void *value) {
    long index = hash(key, map->capacity);

    long control;
    for (control = 0; control < map->capacity; control++) {
        long pos = (index + control) % map->capacity;
        
        if (map->buckets[pos] == NULL || map->buckets[pos]->key == NULL) {
            Pair *newPair = (Pair *)malloc(sizeof(Pair));
            newPair->key = key;
            newPair->value = value;
            map->buckets[pos] = newPair;
            map->size++;
            map->current = pos;
            return;
        }
        
        //verificamos si existe
        if (map->buckets[pos] != NULL && map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key, key)) {
            return;
        }
    }
}

// Función para aumentar la capacidad de la tabla
void enlarge(HashMap *map) {
    enlarge_called = 1; 

    Pair **old_buckets = map->buckets;
    long old_capacity = map->capacity;

    map->capacity *= 2; 
    map->buckets = (Pair **)malloc(sizeof(Pair *) * map->capacity);
    for (long i = 0; i < map->capacity; i++) {
        map->buckets[i] = NULL;
    }

    map->size = 0;
    map->current = -1;

    // Reinsertar las entradas en la nueva tabla
    long control;
    for (control = 0; control < old_capacity; control++) {
        if (old_buckets[control] != NULL && old_buckets[control]->key != NULL) {
            insertMap(map, old_buckets[control]->key, old_buckets[control]->value);
            free(old_buckets[control]->key); //
            free(old_buckets[control]);
        }
    }

    free(old_buckets); // Liberar los viejos buckets
}

// Función para crear un mapa con una capacidad inicial
HashMap *createMap(long capacity) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    map->buckets = (Pair **)malloc(sizeof(Pair *) * capacity);
    
    long control;
    for (control= 0; control < capacity; control++) {
        map->buckets[control] = NULL;
    }
    
    map->capacity = capacity;
    map->size = 0;
    map->current = -1;

    return map;
}

// Función para eliminar una key de la tabla y su valor
void eraseMap(HashMap * map, char * key) {
    
    if (map != NULL && key != NULL)
    {
        Pair * pair = searchMap(map, key);

        if (pair != NULL) {
            pair->key = NULL;
            pair->value = NULL;
            map->size--;
        }
    }
}

// Función para buscar un par (clave-valor) en el mapa
Pair *searchMap(HashMap *map, char *key) {
    long index = hash(key, map->capacity);

    long control;
    for (control = 0; control < map->capacity; control++) {
        long pos = (index + control) % map->capacity;
        Pair *pair = map->buckets[pos];

        if (pair == NULL) return NULL; // Si no hay par en esta posición, terminamos

        if (pair->key != NULL && is_equal(pair->key, key)) {
            map->current = pos;
            return pair;
        }
    }

    return NULL;
}

// Función para obtener el primer par del mapa
Pair *firstMap(HashMap *map) {
    long control;
    for (control = 0; control < map->capacity; control++) {
        if (map->buckets[control] != NULL && map->buckets[control]->key != NULL) {
            map->current = control;
            return map->buckets[control];
        }
    }
    return NULL;
}

// Función para obtener el siguiente par del mapa
Pair *nextMap(HashMap *map) {
    long control;
    for (control = map->current + 1; control < map->capacity; control++) {
        if (map->buckets[control] != NULL && map->buckets[control]->key != NULL) {
            map->current = control;
            return map->buckets[control];
        }
    }
    return NULL;
}