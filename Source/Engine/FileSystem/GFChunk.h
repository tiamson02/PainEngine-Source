// #pragma once
#ifndef _GFCHUNK_
#define _GFCHUNK_

struct GFChunkInfo
{
    String name;
    int start;
    int size;

    GFChunkInfo()
    {
        start = 0;
        size = 0;
    }
    
    friend FileObject& operator>>(GFile& file, GFChunkInfo& ch)
    {
        file >> ch.name;
        file >> ch.start;
        return file >> ch.size;
    }

    friend FileObject& operator<<(GFile& file, GFChunkInfo& ch)
    {
        file << ch.name;
        file << ch.start;
        return file << ch.size;
        
    }
};

struct GFChunkReader
{
    GFChunkInfo ci;
    int csoffset;

    GFChunkReader(GFile *f)
    {
        *f >> ci;
        csoffset = f->Tell() - ci.start;
    };
    
    void Rewind(GFile *f)
    {
        f->Seek(ci.start + csoffset);
    };

    const char * GetName() 
    {
        return ci.name.AsChar();
    }

    int GetSize() 
    {
        return ci.size - csoffset;
    }

    void Next(GFile *f)
    {
        f->Seek(ci.start+ci.size);
    };
};


struct GFChunkWriter
{
    GFChunkInfo ci;

    GFChunkWriter(GFile *f,const char *cname = NULL)
    {
        ci.start = f->Tell();
        ci.size = 0;
        if (cname)
            ci.name = cname;
        *f << ci;
    };

    void Complete(GFile *f)
    {
        long32 end = f->Tell();
        ci.size = end - ci.start;
        f->Seek(ci.start,SEEK_SET);
        *f << ci;
        f->Seek(end,SEEK_SET);
    };
};


#endif //_GFCHUNK_

