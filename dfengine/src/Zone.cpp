//
//  Zone.cpp
//  DAParser
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "Zone.hpp"
#include "GameContext.hpp"
#include "Packing.h"

const char Zone::NAMES_CATEGORY[] = "ZNAM";
const char Zone::INLINE_MARK[] = "IZON";
const char Zone::AUXILIARY_MAKRS[] = "IZAX\0" "IZX2\0" "IZX3\0" "IZX4\0";

vector<Zone*> Zone::ReadFromFile(FILE *file, range range)
{
    fseek(file, range.first, SEEK_SET);

    if(GameContext::CurrentContext()->getGameStyle() == GameStyleYoda)
        // Yoda's file format uses category length as zone count
        fseek(file, -4, SEEK_CUR);

    uint16_t count = 0;
    fread(&count, sizeof(count), 1, file);

    vector<Zone*> zones = vector<Zone*>(count);
    for(int i=0; i < count; i++)
    {
        zones[i] = new Zone(file);
        zones[i]->index = i;
    }

    return zones;
}

void Zone::ReadNamesFromFile(FILE *file, range range, vector<Zone*> &zones)
{
    fseek(file, range.first, SEEK_SET);

    uint16_t id;
    while (fread(&id, sizeof(uint16_t), 1, file) && id != 0xFFFF)
        zones[id]->readName(file);
}

Zone::Zone(FILE* file)
{
    const bool isYodaVersion = context->getGameStyle() == GameStyleYoda;

    uint16_t planet = PlanetTypeNone;

    if(isYodaVersion)
    {
        uint32_t yoda_size;
        uint16_t yoda_index;

        fread(&planet,     sizeof(planet),     1, file);
        fread(&yoda_size,  sizeof(yoda_size),  1, file);
        fread(&yoda_index, sizeof(yoda_index), 1, file);
    }

    // read inline category
    char identifier[4];
    fread(identifier, 4, 1, file);
    if(memcmp(identifier, Zone::INLINE_MARK, 4) != 0)
        printf("Did not find identifier '%s' at 0x%4lx\n", Zone::INLINE_MARK, ftell(file)-4);

    uint32_t size;
    uint16_t zoneWidth, zoneHeight;
    uint32_t type;

    fread(&size, sizeof(size), 1, file);
    fread(&zoneWidth, sizeof(zoneWidth),  1, file);
    fread(&zoneHeight, sizeof(zoneHeight), 1, file);
    fread(&type, sizeof(type), 1, file);

    if(isYodaVersion)
    {
        uint16_t padding; // alwas 0xFFFF
        fread(&padding, sizeof(padding), 1, file);

        uint16_t planet_again;
        fread(&planet_again, sizeof(planet_again), 1, file);

        if(planet != planet_again)
            printf("Warning: planet 0x%04x != 0x%04x planet_again\n", planet, planet_again);
    }

    size_t tileSize = Zone::LAYERS * zoneWidth * zoneHeight;
    uint16_t *tileIDs = new uint16_t[tileSize]();
    fread(tileIDs, sizeof(uint16_t), tileSize, file);

    _tile_ptrs = new Tile*[tileSize];
    for(int i=0; i < tileSize; i++)
        _tile_ptrs[i] = context->getTile(tileIDs[i]);

    _size.width  = zoneWidth;
    _size.height = zoneHeight;
    _type   = (Type)type;
    _planet = (PlanetType)planet;

    if(context->getGameStyle() != GameStyleYoda) return;

    this->_readScriptTiles(file);
    this->_readAuxiliaryData(file);
    this->_readActions(file);

    initialized = justEntered = enteredByPlane = false;
}

Zone::~Zone()
{
}
#pragma mark - Deserialization
void Zone::_readScriptTiles(FILE *file)
{
    if(context->getGameStyle() != GameStyleYoda)
        return;

    // Parse script tiles
    uint16_t script_tile_count;
    fread(&script_tile_count, sizeof(script_tile_count), 1, file);
    ScriptTile *script_tiles = new ScriptTile[script_tile_count];
    fread(script_tiles, sizeof(ScriptTile), script_tile_count, file);
    this->setScriptTiles(script_tiles, script_tile_count);
}

void Zone::_readAuxiliaryData(FILE* file)
{
    if(context->getGameStyle() != GameStyleYoda)
        return;

    for(int i=0; i<4; i++)
    {
        char identifier[4];
        fread(identifier, 4, 1, file);
        if(memcmp(identifier, AUXILIARY_MAKRS+i*5, 4) != 0)
            printf("Did not find identifier '%s' at 0x%4lx\n", AUXILIARY_MAKRS+i*5, ftell(file)-4);

        uint32_t size;
        fread(&size, sizeof(size), 1, file);

        // IZX4 is different from all the others :/
        if(size > 8) size -= 8;

        auxiliaryDataLengths[i] = size;

        uint8_t *buffer = new uint8_t[size];
        fread(buffer, sizeof(uint8_t), size, file);
        auxiliaryData[i] = (char*)malloc(size);
        memcpy(auxiliaryData[i], buffer, size);
    }
}

void Zone::_readActions(FILE *file)
{
    if(context->getGameStyle() != GameStyleYoda)
        return;

    uint16_t iact_count;
    fread(&iact_count, sizeof(iact_count), 1, file);

    for(int j=0; j < iact_count; j++) {
        _actions.push_back(Action(file, this, j));
    }
}

void Zone::readName(FILE *file)
{
    const size_t nameSize = 0x10;

    char data[nameSize];
    fread(data, sizeof(char), nameSize, file);
    _name = string(data);
}
#pragma mark - Serialization
size_t Zone::write(char *buffer)
{
    const bool isYodaVersion = context->getGameStyle() == GameStyleYoda;
    size_t bytesWritten = 0;
    char *yodaSizeLocation = NULL, *sizeLocation = NULL;

    if(isYodaVersion)
    {
        if(buffer) uint16_t_pack(buffer+bytesWritten, _planet);
        bytesWritten += sizeof(uint16_t);

        // yoda size will be written later
        yodaSizeLocation = buffer ? buffer+bytesWritten : NULL;
        bytesWritten += sizeof(uint32_t);

        if(buffer) uint16_t_pack(buffer+bytesWritten, index);
        bytesWritten += sizeof(uint16_t);
    }

    if(buffer) memcpy(buffer+bytesWritten, Zone::INLINE_MARK, Zone::MARKER_LENGTH);
    bytesWritten += MARKER_LENGTH;

    // size will be written later
    sizeLocation = buffer ? buffer+bytesWritten : NULL;
    bytesWritten += sizeof(uint32_t);

    if(buffer) uint16_t_pack(buffer+bytesWritten, _size.width);
    bytesWritten += sizeof(uint16_t);

    if(buffer) uint16_t_pack(buffer+bytesWritten, _size.height);
    bytesWritten += sizeof(uint16_t);

    if(buffer) uint32_t_pack(buffer+bytesWritten, _type);
    bytesWritten += sizeof(uint32_t);

    if(isYodaVersion)
    {
        uint16_t padding = 0xFFFF;
        if(buffer) uint16_t_pack(buffer+bytesWritten, padding);
        bytesWritten += sizeof(uint16_t);

        if(buffer) uint16_t_pack(buffer+bytesWritten, _planet);
        bytesWritten += sizeof(uint16_t);
    }

    size_t tileCount = Zone::LAYERS * _size.width * _size.height;
    for(uint16_t tile =0; tile < tileCount; tile++)
    {
        if(buffer) uint16_t_pack(buffer+bytesWritten, _tile_ptrs[tile] ? _tile_ptrs[tile]->index : 0xFFFF);
        bytesWritten += sizeof(uint16_t);
    }

    if(sizeLocation) uint32_t_pack(sizeLocation, (uint32_t)bytesWritten-8);

    if(!isYodaVersion) return bytesWritten;

    bytesWritten += this->_writeScriptTiles(buffer ? buffer+bytesWritten : NULL);
    bytesWritten += this->_writeAuxiliaryData(buffer ? buffer+bytesWritten : NULL);
    bytesWritten += this->_writeActions(buffer ? buffer+bytesWritten : NULL);

    if(yodaSizeLocation) uint32_t_pack(yodaSizeLocation, (uint32_t)bytesWritten-6);

    return bytesWritten;
}

size_t Zone::_writeScriptTiles(char *buffer)
{
    size_t bytesWritten = 0;
    if(context->getGameStyle() != GameStyleYoda) return 0;

    if(!buffer) return sizeof(uint16_t) + _scriptTiles.size() * 12;

    uint16_t_pack(buffer+bytesWritten, _scriptTiles.size());
    bytesWritten += sizeof(uint16_t);

    for(int i=0; i < _scriptTiles.size(); i++)
    {
        ScriptTile &tile = _scriptTiles[i];
        uint32_t_pack(buffer+bytesWritten, tile.type);
        bytesWritten += sizeof(uint32_t);
        uint16_t_pack(buffer+bytesWritten, tile.x);
        bytesWritten += sizeof(uint16_t);
        uint16_t_pack(buffer+bytesWritten, tile.y);
        bytesWritten += sizeof(uint16_t);
        uint16_t_pack(buffer+bytesWritten, tile.arg1);
        bytesWritten += sizeof(uint16_t);
        uint16_t_pack(buffer+bytesWritten, tile.arg2);
        bytesWritten += sizeof(uint16_t);
    }

    return bytesWritten;
}

size_t Zone::_writeAuxiliaryData(char *buffer)
{
    size_t bytesWritten = 0;
    if(context->getGameStyle() != GameStyleYoda) return bytesWritten;

    for(int i=0; i<4; i++)
    {
        if(buffer) memcpy(buffer+bytesWritten, AUXILIARY_MAKRS+i*5, MARKER_LENGTH);
        bytesWritten += CATEGORY_LENGTH;

        if(buffer) uint32_t_pack(buffer+bytesWritten, auxiliaryDataLengths[i]);
        if(i != 3 && buffer) uint32_t_pack(buffer+bytesWritten, auxiliaryDataLengths[i]+8);
        bytesWritten += sizeof(uint32_t);

        if(buffer) memcpy(buffer+bytesWritten, auxiliaryData[i], auxiliaryDataLengths[i]);
        bytesWritten += auxiliaryDataLengths[i];
    }

    return bytesWritten;
}

size_t Zone::_writeActions(char *buffer)
{
    size_t bytesWritten = 0;
    if(context->getGameStyle() != GameStyleYoda) return bytesWritten;

    if(buffer) uint16_t_pack(buffer+bytesWritten, _actions.size());
    bytesWritten += sizeof(uint16_t);

    for(int i=0; i < _actions.size(); i++)
        bytesWritten += _actions[i].write(buffer ? buffer+bytesWritten : NULL);

    return bytesWritten;
}
#pragma mark -
Tile* Zone::getTile(int x, int y, int l) {
    return this->getTile((GamePoint){.x=x, .y=y, .l=l});
};

Tile* Zone::getTile(GamePoint p) {
    if(!GamePointInBounds(p, _size.width, _size.height))
        return NULL;

    return _tile_ptrs[GamePointToIndex(p, _size.width, _size.height)];
}

#pragma mark -
string Zone::getName()
{
    return _name;
}

Sizei Zone::getSize()
{
    return _size;
}

Zone::Type Zone::getType()
{
    return _type;
}

PlanetType Zone::getPlanet()
{
    return _planet;
}

bool Zone::isSolved()
{
    return _solved;
}

bool Zone::tileWalkableAt(GamePoint point)
{
    if(!GamePointInBounds(point, _size.width, _size.height))
        return false;

    GamePoint floorPoint  = GamePointOnLayer(point, 0);
    GamePoint objectPoint = GamePointOnLayer(point, 1);
    GamePoint roofPoint   = GamePointOnLayer(point, 2);

    Tile *t;
    t = this->getTile(floorPoint);
    if(t != NULL && !t->isWalkable()) return false;

    t = this->getTile(objectPoint);
    if(t != NULL) return false;

    t = this->getTile(roofPoint);
    if(t != NULL && !t->isWalkable()) return false;

    return true;
}

#pragma mark - Action Execution

unsigned short Zone::getCounter()
{
    return counter;
}
void Zone::setCounter(unsigned short newCount)
{
    counter = newCount;
}