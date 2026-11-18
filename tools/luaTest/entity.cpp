/*
    File:   entity.cpp
    Author: eric van beurden
    Date:   8/7/2006

    Namespaces:           <none>
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#include "entity.h"
#include <vector>
#include "trace.h"


typedef std::vector<Entity *>   EntityList;

int         Entity::maxIndex = 1;
EntityList  g_entities;


//******************************* Entity Implementation ***************************************************//
Entity::Entity(){
    GIndex =            maxIndex++;
    type =              0;
    filename =          NULL;
    name =              NULL;
    scale =             1.0f;
    centerGeometry =    false;
}

Entity::~Entity(){
    if (filename)
        delete []filename;

    if (name)
        delete []name;
}


void Entity::setParams(int _type, const char *fname, const char *_name, float _scale, bool _centerGeometry){
    size_t len;


    if (fname){
        len = strlen(fname);

        if (filename)
            delete []filename;

        filename = new char[len + 1];
        strcpy(filename, fname);
    }

    if (_name){
        len = strlen(_name);

        if (name)
            delete []name;

        name = new char[len + 1];
        strcpy(name, _name);
    }

    type = _type;
    scale = _scale;
    centerGeometry = _centerGeometry;
}
//***********************************************************************************************************//

Entity *createEntity(int type, const char *filename, const char *name, float scale, bool centerGeometry){
    Entity *entity = new Entity;


    entity->setParams(type, filename, name, scale, centerGeometry);
    g_entities.push_back(entity);

    TRACE("createEntity(): created the entity '%s' from '%s' (type = %d, scale = %g, index = %d)\n", name, filename, type, scale, entity->GIndex);
    return entity;
}

bool deleteEntity(Entity *entity){
    if (entity == NULL){
        TRACE("deleteEntity(): ERROR-> invalid entity passed in!\n");

        return false;
    }


    for (EntityList::iterator i = g_entities.begin(); i != g_entities.end(); i++){
        if (*i == entity){
            g_entities.erase(i);

            TRACE("deleteEntity(): destroying the entity '%s' from '%s' (type = %d, scale = %g, index = %d)\n", entity->name, entity->filename, entity->type, entity->scale, entity->GIndex);

            delete entity;

            return true;
        }
    }

    TRACE("deleteEntity(): ERROR-> could not find the entity '%s' from '%s' (type = %d, scale = %g, index = %d)\n", entity->name, entity->filename, entity->type, entity->scale, entity->GIndex);
    return false;
}

Entity *getEntityByIndex(int index){
    for (EntityList::iterator i = g_entities.begin(); i != g_entities.end(); i++){
        if ((*i)->GIndex == index)
            return *i;
    }
    
    return NULL;
}