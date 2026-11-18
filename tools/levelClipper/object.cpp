#include "object.h"
#include "fileUtil.h"
#include "text.h"
#include "trace.h"
#include <stdlib.h>


const char *Object::STR_POS = "o.Pos";
const char *Object::STR_WIDTH = "o.Size.Width";
const char *Object::STR_HEIGHT = "o.Size.Height";
const char *Object::STR_DEPTH = "o.Size.Depth";
const char *Object::STR_COLOR = "o.Color";
const char *Object::STR_END = "o.End";


//************************************ utility functions *************************************************//
size_t strcollapse(char *line, const char *oldStr, const char *newStr){
    char *  tok;
    size_t  oldLen = strlen(oldStr);
    size_t  newLen = strlen(newStr);
    size_t  len = strlen(line);


    Assert(newLen <= oldLen);


    tok = strstr(line, oldStr);

    while (tok){
        memcpy(tok, newStr, newLen);

        memcpy(&tok[newLen], &tok[oldLen], len - (&tok[newLen] - line) + 1);
        len -= oldLen - newLen;

        tok = strstr(&tok[newLen], oldStr);
    }


    return len;
}


bool parseVector(char *tok, Vector *v, char separator = '(', const char *delimiters = "(,); \t\n\r"){
    char *endp;


    endp = strchr(tok, separator);

    if (endp == NULL)
        return false;


    //parse the position vector
    tok = Text::strtok2(NULL, delimiters, &endp);
    v->x = (float32)strtod(tok, NULL);

    tok = Text::strtok2(NULL, delimiters, &endp);
    v->y = (float32)strtod(tok, NULL);

    tok = Text::strtok2(NULL, delimiters, &endp);
    v->z = (float32)strtod(tok, NULL);

    return true;
}

bool parseColor(char *tok, Color *c, char separator = '(', const char *delimiters = "(,); \t\n\r"){
    char *endp;


    endp = strchr(tok, separator);

    if (endp == NULL)
        return false;


    //parse the position vector
    tok = Text::strtok2(NULL, delimiters, &endp);
    c->r = (uint8)strtol(tok, NULL, 0);

    tok = Text::strtok2(NULL, delimiters, &endp);
    c->g = (uint8)strtol(tok, NULL, 0);

    tok = Text::strtok2(NULL, delimiters, &endp);
    c->b = (uint8)strtol(tok, NULL, 0);

    tok = Text::strtok2(NULL, delimiters, &endp);
    c->a = (uint8)strtol(tok, NULL, 0);

    return true;
}

bool parseFloat(char *tok, float32 *f, char separator = '=', const char *delimiters = "=; \t\n\r"){
    char *endp;


    endp = strchr(tok, separator);

    if (endp == NULL)
        return false;


    //parse the position vector
    tok = Text::strtok2(NULL, delimiters, &endp);
    *f = (float32)strtod(tok, NULL);

    return true;
}


//************************************ constructors & destructor ***********************************//
Object::Object(){
    filename = NULL;
    size.set(0, 0, 0);
    pos.set(0, 0, 0);
    boundingBox.reset();
    fogColor.set(0, 0, 0, 0);
    fogEnd = 0.0f;
    valid = false;
}

Object::Object(const char *fname){
    filename = NULL;
    size.set(0, 0, 0);
    pos.set(0, 0, 0);
    boundingBox.reset();
    fogColor.set(0, 0, 0, 0);
    fogEnd = 0.0f;
    valid = false;

    load(fname);
}

Object::~Object(){
    clear();
}


Object &Object::operator = (const Object &o){
    Text::strcopy(&filename, o.filename);

    pos = o.pos;
    size = o.size;
    boundingBox = o.boundingBox;
    valid = o.valid;
    fogColor = o.fogColor;
    fogEnd = o.fogEnd;

    return *this;
}

void Object::clear(){
    Text::strcopy(&filename, NULL);

    size.set(0, 0, 0);
    pos.set(0, 0, 0);
    boundingBox.reset();
    valid = false;
    fogColor.set(0, 0, 0, 0);
    fogEnd = 0.0f;

    valid = false;
}



//************************************ loading & saving functions ***********************************//
bool Object::load(const char *fname){
    const int32 BUFSIZE = 4096;
    char        line[BUFSIZE];
    char *      tok;
    FILE *      fp;


    fp = fopen(fname, "rt");

    if (fp == NULL)
        return false;


    clear();


    while (!feof(fp)){
        fgets(line, BUFSIZE, fp);

        if (feof(fp))
            break;


        tok = strstr(line, STR_POS);

        //found the object's position
        if (tok){
            if (!parseVector(tok, &pos))
                TRACE("Object::load(): ERROR-> could not parse the object's position from '%s' {line = '%s'}\n", fname, line);

            else
                valid = true;

            continue;
        }


        tok = strstr(line, STR_COLOR);

        //found the object's fog color
        if (tok){
            if (!parseColor(tok, &fogColor))
                TRACE("Object::load(): ERROR-> could not parse the object's color from '%s' {line = '%s'}\n", fname, line);

            continue;
        }

        tok = strstr(line, STR_END);

        //found the object's fog end
        if (tok){
            if (!parseFloat(tok, &fogEnd))
                TRACE("Object::load(): ERROR-> could not parse the object's fog end from '%s' {line = '%s'}\n", fname, line);

            continue;
        }


        tok = strstr(line, STR_WIDTH);

        //found the object's width
        if (tok){
            if (!parseFloat(tok, &size.x))
                TRACE("Object::load(): ERROR-> could not parse the object's width from '%s' {line = '%s'}\n", fname, line);

            continue;
        }

        tok = strstr(line, STR_HEIGHT);

        //found the object's height
        if (tok){
            if (!parseFloat(tok, &size.y))
                TRACE("Object::load(): ERROR-> could not parse the object's height from '%s' {line = '%s'}\n", fname, line);

            continue;
        }

        tok = strstr(line, STR_DEPTH);

        //found the object's depth
        if (tok){
            if (!parseFloat(tok, &size.z))
                TRACE("Object::load(): ERROR-> could not parse the object's depth from '%s' {line = '%s'}\n", fname, line);

            continue;
        }
    }


    Text::strcopy(&filename, fname);
    fclose(fp);

    calcBoundingBox();

    return true;
}

bool Object::save(const char *fname) const{
    const int32 BUFSIZE = 4096;
    char        line[BUFSIZE];
    FILE *      fp;
    char *      file = NULL;
    int32       size;
    size_t      pos = 0;
    size_t      len;


    fp = fopen(fname, "rt");

    if (fp == NULL){
        TRACE("Object::save(): ERROR-> could not open the file '%s' for reading\n", fname);

        return false;
    }


    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);


    file = new char[size + 1];


    while (!feof(fp)){
        fgets(line, BUFSIZE, fp);

        if (feof(fp))
            break;


        len = strcollapse(line, "\\\\", "/");
        len = strcollapse(line, "\\", "/");

        
        memcpy(&file[pos], line, len);
        pos += len;
    }

    fclose(fp);


    //write out the file (NOTE: this will clobber the original file and will not back it up)
    fp = fopen(fname, "wt");

    if (fp == NULL){
        TRACE("Object::save(): ERROR-> could not open the file '%s' for writing\n", fname);
        delete []file;

        return false;
    }

    fwrite(file, 1, pos, fp);
    fclose(fp);

    delete []file;

    return true;
}


void Object::calcBoundingBox(){
    if (!valid)
        return;


    boundingBox.reset();

    boundingBox += pos;

    boundingBox += pos + (size * 0.5f);
    boundingBox += pos - (size * 0.5f);
}

//************************************ accessor functions *******************************************//
bool Object::isValid() const{
    return valid;
}

const Box &Object::getBoundingBox() const{
    return boundingBox;
}

const Vector &Object::getSize() const{
    return size;
}

const Vector &Object::getPosition() const{
    return pos;
}

const Color &Object::getFogColor() const{
    return fogColor;
}

const float32 Object::getFogEnd() const{
    return fogEnd;
}

const char *Object::getFilename() const{
    return filename;
}

