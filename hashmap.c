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
    long size; //cantidad de datos/pairs en la tabla
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

    // Buscar un lugar vacío o una clave igual
    for (long i = 0; i < map->capacity; i++) {
        long pos = (index + i) % map->capacity;
        
        if (map->buckets[pos] == NULL || map->buckets[pos]->key == NULL) {
            Pair *newPair = (Pair *)malloc(sizeof(Pair));
            newPair->key = key;  // Asumimos que strdup está disponible
            newPair->value = value;
            map->buckets[pos] = newPair;
            map->size++;
            map->current = pos;
            return;
        }
        
        // Si la clave ya existe, no la insertamos (para evitar duplicados)
        if (map->buckets[pos] != NULL && map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key, key)) {
            return;
        }
    }
}

// Función para aumentar la capacidad del mapa (rehash)
void enlarge(HashMap *map) {
    enlarge_called = 1; // Para propósitos de prueba

    Pair **old_buckets = map->buckets;
    long old_capacity = map->capacity;

    map->capacity *= 2; // Doblamos la capacidad
    map->buckets = (Pair **)malloc(sizeof(Pair *) * map->capacity);
    for (long i = 0; i < map->capacity; i++) {
        map->buckets[i] = NULL;
    }

    map->size = 0;
    map->current = -1;

    // Reinsertar las entradas en la nueva tabla
    for (long i = 0; i < old_capacity; i++) {
        if (old_buckets[i] != NULL && old_buckets[i]->key != NULL) {
            insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
            free(old_buckets[i]->key); // Liberar la memoria de la clave
            free(old_buckets[i]);      // Liberar el par
        }
    }

    free(old_buckets); // Liberar los viejos buckets
}

// Función para crear un mapa con una capacidad inicial
HashMap *createMap(long capacity) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    map->buckets = (Pair **)malloc(sizeof(Pair *) * capacity);
    
    for (long i = 0; i < capacity; i++) {
        map->buckets[i] = NULL;
    }
    
    map->capacity = capacity;
    map->size = 0;
    map->current = -1;

    return map;
}

// Función para borrar un par (clave-valor) del mapa
void eraseMap(HashMap *map, char *key) {
    long index = hash(key, map->capacity);

    for (long i = 0; i < map->capacity; i++) {
        long pos = (index + i) % map->capacity;
        Pair *pair = map->buckets[pos];

        if (pair == NULL) return; // Si no hay par en esta posición, terminamos

        if (pair->key != NULL && is_equal(pair->key, key)) {
            free(pair->key); // Liberamos la memoria de la clave
            free(pair);      // Liberamos el par
            map->buckets[pos] = NULL; // Marcamos como vacía la posición
            map->size--;
            return;
        }
    }
}

// Función para buscar un par (clave-valor) en el mapa
Pair *searchMap(HashMap *map, char *key) {
    long index = hash(key, map->capacity);

    for (long i = 0; i < map->capacity; i++) {
        long pos = (index + i) % map->capacity;
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
    for (long i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}

// Función para obtener el siguiente par del mapa
Pair *nextMap(HashMap *map) {
    for (long i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}