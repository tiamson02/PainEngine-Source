#include "material.h"
#include "fileUtil.h"
#include "text.h"


//******************************* constructor & destructor *******************************//
MaterialDef::MaterialDef(){
    name = NULL;
}

MaterialDef::~MaterialDef(){
    Text::strcopy(&name, NULL);
}


MaterialDef &MaterialDef::operator = (const MaterialDef &m){
    Text::strcopy(&name, m.name);

    pan = m.pan;
    tile = m.tile;

    return *this;
}


//********************************* loading & saving functions *******************************//
bool MaterialDef::load(FILE *fp){
    if (fp == NULL || ferror(fp) || feof(fp))
        return false;

    
    int32 size;


    //read the texture/material name
    INP(fp, size);
    Text::strcopy(&name, "", size);

    fread(name, 1, size, fp);


    INP(fp, pan);
    INP(fp, tile);

    return true;
}

bool MaterialDef::save(FILE *fp) const{
    if (fp == NULL || ferror(fp))
        return NULL;


    if (name == NULL){
        char *tmp = "";

        WRITESTRING(fp, tmp);
    }

    else
        WRITESTRING(fp, name);

    OUTP(fp, pan);
    OUTP(fp, tile);

    return true;
}


//*************************************** accessor functions ***********************************//
const char *MaterialDef::getName() const{
    return name;
}

const TexCoord &MaterialDef::getPan() const{
    return pan;
}

const TexCoord &MaterialDef::getTile() const{
    return tile;
}


void MaterialDef::setPan(const TexCoord &p){
    pan = p;
}

void MaterialDef::setTile(const TexCoord &t){
    tile = t;
}

