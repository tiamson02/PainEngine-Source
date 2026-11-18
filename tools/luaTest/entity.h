/*
    File:   entity.h
    Author: eric van beurden
    Date:   8/7/2006

    Namespaces:           <none>
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#ifndef  __entity_H_
# define __entity_H_

# include <string.h>

class Entity{
private:
    static int maxIndex;

public:
    int     GIndex;
    int     type;
    char *  filename;
    char *  name;
    float   scale;
    bool    centerGeometry;

    Entity();
    ~Entity();

    void setParams(int _type, const char *fname, const char *_name, float _scale, bool _centerGeometry);
};


Entity *createEntity(int type, const char *filename, const char *name, float scale, bool centerGeometry);
bool    deleteEntity(Entity *entity);
Entity *getEntityByIndex(int index);

#endif
