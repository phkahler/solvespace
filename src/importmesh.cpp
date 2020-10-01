//-----------------------------------------------------------------------------
// Triangle mesh file reader. Reads an STL file triangle mesh and creates
// a SovleSpace SMesh from it. Supports only Linking, not import.
//
// Copyright 2020 Paul Kahler.
//-----------------------------------------------------------------------------
#include "solvespace.h"
#include "sketch.h"

// Make a new point - type doesn't matter since we will make a copy later
static hEntity newPoint(EntityList *el, int *id, Vector p) {
    Entity en = {};
    en.type = Entity::Type::POINT_N_COPY;
    en.extraPoints = 0;
    en.timesApplied = 0;
    en.group.v = 462;
    en.actPoint = p;
    en.construction = false;
    en.style.v = Style::DATUM;
    en.actVisible = true;
    en.forceHidden = false;

    *id = *id+1;
    en.h.v = *id + en.group.v*65536;    
    el->Add(&en);
    return en.h;
}

// check if a vertex is unique and add it via newPoint if it is.
static void addVertexIfUnique(float x, float y, float z, EntityList *el) {
    if(el->n < 5000) {// no uniqueness check yet
        int id = el->n+2;
        newPoint(el, &id, Vector::From(x,y,z));
    }
}

namespace SolveSpace {

bool LinkStl(const Platform::Path &filename, EntityList *el, SMesh *m, SShell *sh) {
    dbp("\nLink STL triangle mesh.");
    el->Clear();
    std::string data;
    if(!ReadFile(filename, &data)) {
        Error("Couldn't read from '%s'", filename.raw.c_str());
        return false;
    }
    
    std::stringstream f(data);

    char str[80] = {};
    f.read(str, 80);
    
    uint32_t n;
    uint32_t color;
    
    f.read((char*)&n, 4);
    dbp("%d triangles", n);
    
    float x,y,z;
    float xn,yn,zn;
    
    for(uint32_t i = 0; i<n; i++) {
        STriangle tr = {};

        // read the triangle normal
        f.read((char*)&xn, 4);
        f.read((char*)&yn, 4);
        f.read((char*)&zn, 4);
        tr.an = Vector::From(xn,yn,zn);
        tr.bn = tr.an;
        tr.cn = tr.an;

        f.read((char*)&x, 4);
        f.read((char*)&y, 4);
        f.read((char*)&z, 4);
        tr.a.x = x;
        tr.a.y = y;
        tr.a.z = z;
        addVertexIfUnique(x,y,z, el);

        f.read((char*)&x, 4);
        f.read((char*)&y, 4);
        f.read((char*)&z, 4);
        tr.b.x = x;
        tr.b.y = y;
        tr.b.z = z;
        addVertexIfUnique(x,y,z, el);

        f.read((char*)&x, 4);
        f.read((char*)&y, 4);
        f.read((char*)&z, 4);
        tr.c.x = x;
        tr.c.y = y;
        tr.c.z = z;
        addVertexIfUnique(x,y,z, el);

        f.read((char*)&color,2);
        if(color & 0x8000) {
            tr.meta.color.red = (color >> 7) & 0xf8;
            tr.meta.color.green = (color >> 2) & 0xf8;
            tr.meta.color.blue = (color << 3);
            tr.meta.color.alpha = 255;
        } else {
            tr.meta.color.red = 90;
            tr.meta.color.green = 120;
            tr.meta.color.blue = 140;
            tr.meta.color.alpha = 255;        
        }

        m->AddTriangle(&tr);    
    }
    SK.GetGroup(SS.GW.activeGroup)->forceToMesh = true;
    
    return true;
}

}
