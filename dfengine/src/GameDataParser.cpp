//
//  GameDataParser.cpp
//  DAParser
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "GameDataParser.hpp"
#include "OpenGL.h"

#include "Packing.h"

#include <iosfwd>
#include <string>
#include <sstream>
#include <iostream>

#include "Tile.hpp"
#include "Char.hpp"
#include "Puzzle.hpp"

#include "Palette.h"
using namespace std;

const char GameDataParser::VERSION_CATEGORY[] = "VERS";
const char GameDataParser::LOADING_CATEGORY[] = "STUP";
const char GameDataParser::END_CATEGORY[] = "ENDF";
const int GameDataParser::CATEGORY_LENGTH = 4;

GameDataParser::GameDataParser(const string& path) : GameDataParser(path.c_str())
{
}

GameDataParser::GameDataParser(const char *path)
{
    _isYodaVersion = false;

    _file = fopen(path, "r");
    if(_file)
        this->_readFileIndex();

    this->_readSetupImage(getCategory(GameDataParser::LOADING_CATEGORY));

    fseek(_file, getCategory(SoundStore::HEADER).first - 8, SEEK_SET);
    _sounds.readFromFile(_file);

    fseek(_file, getCategory(TileStore::HEADER).first - 8, SEEK_SET);
    _tiles.readFromFile(_file);

    fseek(_file, getCategory(ZoneStore::HEADER).first - 8, SEEK_SET);
    _zones.readFromFile(_file);

    _puzzles = Puzzle::ReadFromFile(_file, getCategory(Puzzle::CATEGORY));
    _chars   = Char::ReadFromFile(_file, getCategory(Char::CATEGORY));

    Char::ReadWeaponsFromFile(_file, getCategory(Char::WEAPONS_CATEGORY), _chars);
    Char::ReadAuxiliariesFromFile(_file, getCategory(Char::AUXILIARY_CATEGORY), _chars);
    Action::ReadFromFile(_file, getCategory(Action::CATEGORY), _zones);
    Action::ReadNamesFromFile(_file, getCategory(Action::NAMES_CATEGORY), _zones);
    Tile::ReadNamesFromFile(_file, getCategory(Tile::NAMES_CATEGORY), _tiles);
    Zone::ReadNamesFromFile(_file, getCategory(Zone::NAMES_CATEGORY), _zones);
    Puzzle::ReadNamesFromFile(_file, getCategory(Puzzle::NAMES_CATEGORY), _puzzles);
}

GameDataParser::~GameDataParser()
{
    fclose(_file);
}

#pragma mark - Reading
void GameDataParser::_readFileIndex()
{
    // remember current position in file
    long pos = ftell(_file);

    fseek(_file, 0, SEEK_SET);

    Category *h;
    char categoryName[5];
    long categoryStart=0;
    do
    {
        // Read header
        h = this->_readHeader();
        if(h == NULL)
        {
            printf("Could not read enough data.\n");
            delete h;
            break;
        }

        // VERS and ENDF are known categories that do not actually contain size data.
        if(memcmp(h->identifier, GameDataParser::VERSION_CATEGORY, 4) == 0)
        {
            memcpy(categoryName, h->identifier, 4);
            categoryName[4] = '\0';
            pair<string, range> p = pair<string, range>(string(categoryName), range(ftell(_file), 0));
            _fileIndex.insert(p);

            delete h;

            continue;
        }

        if(memcmp(h->identifier, GameDataParser::END_CATEGORY, 4) == 0)
        {
            memcpy(categoryName, h->identifier, 4);
            categoryName[4] = '\0';
            pair<string, range> p = pair<string, range>(string(categoryName), range(ftell(_file), 0));
            _fileIndex.insert(p);
            delete h;

            break;
        }

        // remember start position
        categoryStart = ftell(_file);
        // prepare conversion to null terminated c-string
        memcpy(categoryName, h->identifier, 4);
        categoryName[4] = '\0';

        // In Yoda Stories ZONE also does not contain valid data
        if(memcmp(h->identifier, ZoneStore::HEADER, ZoneStore::HEADER_LENGTH) == 0 && h->size < 100000)
        {
            _isYodaVersion = true;
            GameContext::CurrentContext()->switchToGameStyle(GameStyleYoda);

            uint16_t count = h->size & 0xFFFF;
            // rewind 2 bytes (we read 4 byte size, but only use 2 byte count)
            fseek(_file, -2, SEEK_CUR);

            ZoneSubheader *s;
            for(int i=0; i < count; i++)
            {
                s = this->_readZoneSUB();
                if(s != NULL)
                    fseek(_file, s->size, SEEK_CUR);
                else
                    printf("Could not read enough data! \n");
                delete s;
            }
            h->size = 0;
        }
        fseek(_file, h->size, SEEK_CUR);

        // store category range in fileIndex
        pair<string, range> p = pair<string, range>(string(categoryName), range(categoryStart, ftell(_file)-categoryStart));

        _fileIndex.insert(p);

        delete h;
    } while (1);

    // restore previous file position
    fseek(_file, pos, SEEK_SET);
}

void GameDataParser::_readSetupImage(range category_range)
{
    const size_t size = category_range.second;
    fseek(_file, category_range.first, SEEK_SET);

    _loadingImage = (unsigned char *)malloc(size);
    fread(_loadingImage, size, 1, _file);
    _loadingImagePixelCount = size;
}

void GameDataParser::_readZAUX()
{
    range range = _fileIndex.find("ZAUX")->second;
    fseek(_file, range.first, SEEK_SET);

    int i=0;
    do {
        uint32_t size;

        char identifier[4];
        fread(identifier, sizeof(char), 4, _file);
        if(memcmp("IZAX", identifier, 4) != 0)
            break;

        fread(&size, sizeof(uint32_t), 1, _file);
        // printf("ZAUX (%d) Size %d:", i, size-8);
        uint8_t *zauxData = (uint8_t*)malloc(size-8);

        // 00 00 00 00 ITEM COUNT ITEMS ...
        fread(zauxData, size-8, 1, _file);

        for(int i=0; i < size-8; i++)
        {
           // printf(" %02x", zauxData[i]);
        }

        free(zauxData);

        // printf("\n");
        i++;
        // TODO: read data, set at zone
    } while (true);
}

void GameDataParser::_readZAX2()
{
    range range = _fileIndex.find("ZAX2")->second;
    fseek(_file, range.first, SEEK_SET);

    do {
        uint32_t size;
        char identifier[4];
        fread(identifier, sizeof(char), 4, _file);
        if(memcmp("IZX2", identifier, 4) != 0)
            break;

        fread(&size, sizeof(uint32_t), 1, _file);
        // TODO: read data, set at zone
        fseek(_file, size-8, SEEK_CUR);
    } while (true);
}

void GameDataParser::_readZAX3()
{
    range range = _fileIndex.find("ZAX3")->second;
    fseek(_file, range.first, SEEK_SET);

    do {
        uint32_t size;
        char identifier[4];
        fread(identifier, sizeof(char), 4, _file);
        if(memcmp("IZX3", identifier, 4) != 0)
            break;

        fread(&size, sizeof(uint32_t), 1, _file);
        // TODO: read data, set at zone
        fseek(_file, size-8, SEEK_CUR);
    } while (true);
}

void GameDataParser::_readZAX4()
{
    range range = _fileIndex.find("ZAX4")->second;
    fseek(_file, range.first, SEEK_SET);

    do {
        uint32_t size;

        char identifier[4];
        fread(identifier, sizeof(char), 4, _file);
        if(memcmp("IZX4", identifier, 4) != 0)
            break;

        fread(&size, sizeof(uint32_t), 1, _file);
        // TODO: read data, set at zone
        fseek(_file, size, SEEK_CUR);
    } while (true);
}

#pragma mark - Writing
unsigned long long GameDataParser::getData(char *buffer)
{
    size_t bytesWritten = 0;
#define APPEND_DATA(_fn_) {   \
        bytesWritten += _fn_(buffer ? buffer + bytesWritten : NULL); \
    }

    bytesWritten += _getVersionData(buffer ? buffer + bytesWritten : NULL);
    bytesWritten += _getStupData(buffer ? buffer + bytesWritten : NULL);
    bytesWritten += _sounds.write(buffer ? buffer + bytesWritten : NULL);
    bytesWritten += _tiles.write(buffer ? buffer + bytesWritten : NULL);
    bytesWritten += _zones.write(buffer ? buffer + bytesWritten : NULL);

    if(!_isYodaVersion)
    {
        _readZAUX();
        _readZAX2();
        _readZAX3();
        _readZAX4();

        APPEND_DATA(_getZAUXData);
        APPEND_DATA(_getZAX2Data);
        APPEND_DATA(_getZAX3Data);
        APPEND_DATA(_getZAX4Data);
        APPEND_DATA(_getHTSPData);
        APPEND_DATA(_getACTNData);
    }

    APPEND_DATA(_getPUZ2Data);
    APPEND_DATA(_getCHARData);
    APPEND_DATA(_getCHWPData);
    APPEND_DATA(_getCAUXData);
    APPEND_DATA(_getTNAMData);

    if (!_isYodaVersion)
    {
        APPEND_DATA(_getZoneNameData);
        APPEND_DATA(_getPuzzleNameData);
        APPEND_DATA(_getActionNameData);
    }

    APPEND_DATA(_getENDFData);

    return bytesWritten;
}

unsigned long GameDataParser::_getVersionData(char* buffer)
{
    size_t bytesWritten = 0;

    if(buffer) memcpy(buffer+bytesWritten, GameDataParser::VERSION_CATEGORY, CATEGORY_LENGTH);
    bytesWritten += CATEGORY_LENGTH;

    if(buffer) uint32_t_pack(buffer+bytesWritten, 0x0200);
    bytesWritten += sizeof(uint32_t);

    return bytesWritten;
}

size_t GameDataParser::_getStupData(char* buffer)
{
#pragma mark Read from original file
    return this->_getOriginalData(GameDataParser::LOADING_CATEGORY, buffer);
}

size_t GameDataParser::_getZAUXData(char *outBuffer)
{
#pragma mark INDY TODO
    return this->_getOriginalData("ZAUX", outBuffer);
}

size_t GameDataParser::_getZAX2Data(char *outBuffer)
{
#pragma mark INDY TODO
    return this->_getOriginalData("ZAX2", outBuffer);
}
size_t GameDataParser::_getZAX3Data(char *outBuffer)
{
#pragma mark INDY TODO
    return this->_getOriginalData("ZAX3", outBuffer);
}
size_t GameDataParser::_getZAX4Data(char *outBuffer)
{
#pragma mark INDY TODO
    return this->_getOriginalData("ZAX4", outBuffer);
}

size_t GameDataParser::_getHTSPData(char *outBuffer)
{
#pragma mark INDY TODO
    return this->_getOriginalData("HTSP", outBuffer);
}
size_t GameDataParser::_getACTNData(char *outBuffer)
{
#pragma mark INDY TODO
    return this->_getOriginalData(Action::CATEGORY, outBuffer);
}

size_t GameDataParser::_getPUZ2Data(char *buffer){
    size_t bytesWritten = 0;

    // category header
    if(buffer) memcpy(buffer+bytesWritten, Puzzle::CATEGORY, CATEGORY_LENGTH);
    bytesWritten += CATEGORY_LENGTH;

    // category size will be written later
    char *sizeLocation = buffer+bytesWritten;
    bytesWritten += sizeof(uint32_t);

    for(int i=0; i < _puzzles.size(); i++){
        // index
        if(buffer) uint16_t_pack(buffer+bytesWritten, i);
        bytesWritten += sizeof(uint16_t);

        bytesWritten += this->_puzzles[i]->write(buffer ? buffer+bytesWritten : NULL);
    }

    if(buffer) uint16_t_pack(buffer+bytesWritten, 0xFFFF);
    bytesWritten += sizeof(uint16_t);

    if(buffer) uint32_t_pack(sizeLocation, (uint32_t)bytesWritten-8);

    return bytesWritten;
}

size_t GameDataParser::_getCHARData(char *buffer){
    long entry_size  = _isYodaVersion ? 0x54 : 0x4E;
    unsigned long bufferSize = 8 + _chars.size() * entry_size + 2;

    size_t bytesWritten = 0;
    if(buffer) memcpy(buffer+bytesWritten, Char::CATEGORY, CATEGORY_LENGTH);
    bytesWritten += CATEGORY_LENGTH;

    if(buffer) uint32_t_pack(buffer+bytesWritten, (uint32_t)bufferSize-8);
    bytesWritten += sizeof(uint32_t);

    for(int i=0; i < _chars.size(); i++)
    {
        if(buffer) uint16_t_pack(buffer+bytesWritten, (uint16_t)i);
        bytesWritten += sizeof(uint16_t);

        Char *chara = _chars.at(i);
        bytesWritten += chara->write(buffer ? buffer + bytesWritten : NULL);
    }

    if(buffer) uint16_t_pack(buffer+bytesWritten, 0xFFFF);
    bytesWritten += sizeof(uint16_t);

    if(bytesWritten != bufferSize)
        printf("ERROR!\n");
    return bytesWritten;
}

size_t GameDataParser::_getCHWPData(char *buffer){
    unsigned long bufferSize = 8 + _chars.size() * (2 + 4) + 2;

    size_t bytesWritten = 0;
    if(buffer) memcpy(buffer+bytesWritten, Char::WEAPONS_CATEGORY, CATEGORY_LENGTH);
    bytesWritten += CATEGORY_LENGTH;

    // write buffer size (without CHWP)
    if(buffer) uint32_t_pack(buffer+bytesWritten, (uint32_t)(bufferSize-8));
    bytesWritten += sizeof(uint32_t);

    for(int i=0; i < _chars.size(); i++){
        if(buffer) uint16_t_pack(buffer+bytesWritten, i);
        bytesWritten += sizeof(uint16_t);
        bytesWritten += _chars.at(i)->writeWeapon(buffer ? buffer+bytesWritten : NULL);
    }

    if(buffer) uint16_t_pack(buffer+bytesWritten, 0xFFFF); // wb += sizeof(uint16_t);
    bytesWritten += sizeof(uint16_t);

    if(bytesWritten != bufferSize)
        printf("ERROR!\n");
    return bytesWritten;
}

size_t GameDataParser::_getCAUXData(char *buffer){
    size_t bytesWritten = 0;

    if(buffer) memcpy(buffer+bytesWritten, Char::AUXILIARY_CATEGORY, CATEGORY_LENGTH);
    bytesWritten += CATEGORY_LENGTH;

    // write buffer size (without CHWP)
    char * sizeLocation = buffer+bytesWritten;
    bytesWritten += sizeof(uint32_t);

    for(int i=0; i < _chars.size(); i++){
        if(buffer) uint16_t_pack(buffer+bytesWritten, i);
        bytesWritten += sizeof(uint16_t);
        bytesWritten += _chars.at(i)->writeAuxiliary(buffer ? buffer + bytesWritten : NULL);
    }

    if(buffer) uint16_t_pack(buffer+bytesWritten, 0xFFFF);// wb += sizeof(uint16_t);
    bytesWritten += sizeof(uint16_t);

    if(buffer) uint32_t_pack(sizeLocation, (uint32_t)bytesWritten-8);

    return bytesWritten;
}

size_t GameDataParser::_getTNAMData(char *buffer)
{
    size_t bytesWritten = 0;

    // category identifier
    if(buffer) memcpy(buffer+bytesWritten, Tile::NAMES_CATEGORY, CATEGORY_LENGTH);
    bytesWritten += CATEGORY_LENGTH;

    // category size will be written later
    char* sizeLocation = buffer+bytesWritten;
    bytesWritten += sizeof(uint32_t);

    for(uint16_t i=0; i < GameContext::CurrentContext()->getTileCount(); i++)
    {
        Tile *t = GameContext::CurrentContext()->getTile(i);
        if(t->name.size()) {
            if(buffer) uint16_t_pack(buffer+bytesWritten, i);
            bytesWritten += sizeof(uint16_t);

            bytesWritten += t->writeName(buffer ? buffer + bytesWritten : NULL);
        }
    }

    if(buffer) uint16_t_pack(buffer+bytesWritten, 0xFFFF);
    bytesWritten += sizeof(uint16_t);

    if(buffer) uint32_t_pack(sizeLocation, (uint32_t)bytesWritten-8);

    return bytesWritten;
}


size_t GameDataParser::_getZoneNameData(char *outBuffer)
{
    if(_isYodaVersion) return 0;

    const size_t entrySize = 0x10;
    const uint16_t bufferSize = 8 + entrySize * _zones.size() + 2;

    if(!outBuffer) return bufferSize;
    char *wb = (char*)malloc(bufferSize);

    // category header
    memcpy(wb, Zone::NAMES_CATEGORY, 4);
    wb += 4;

    // category data size
    uint32_t_pack(wb, bufferSize-8);  wb += sizeof(uint32_t);

    for(int i=0; i < _zones.size(); i++)
    {
        uint16_t_pack(wb, i); wb += sizeof(uint16_t);

        memset(wb, 0, entrySize);
        sprintf(wb, "%s", _zones.at(i)->getName().c_str());
    }

    // number of pname entries
    uint16_t_pack(wb, 0xFFFF); // wb += sizeof(uint16_t);

#pragma mark INDY TODO
    return this->_getOriginalData(Zone::NAMES_CATEGORY, outBuffer);
}

size_t GameDataParser::_getPuzzleNameData(char *outBuffer)
{
    if(_isYodaVersion) return 0;

    size_t entrySize = 0x10;
    uint16_t bufferSize = 8 + entrySize * _puzzles.size() + 2;
    if(!outBuffer) return bufferSize;

    char *wb = (char*)malloc(bufferSize);

    // category header
    memcpy(wb, Puzzle::NAMES_CATEGORY, 4);
    wb += 4;

    // category data size
    uint32_t_pack(wb, bufferSize-8);  wb += sizeof(uint32_t);

    // number of pname entries
    uint16_t_pack(wb, _puzzles.size()); wb += sizeof(uint16_t);
    for(int i=0; i < _puzzles.size(); i++)
    {
        // name with padding to 0x10
        memset(wb, 0, entrySize);
        sprintf(wb, "%s", _puzzles.at(i)->getName().c_str()); wb += entrySize;
    }
    return bufferSize;
}

size_t GameDataParser::_getActionNameData(char *outBuffer)
{
    if(_isYodaVersion) return 0;

    size_t data_length = 8;
    for(int i=0; i < _zones.size(); i++)
    {
        if(_zones.at(i)->_actions.size())
            data_length += 2 + _zones.at(i)->_actions.size() * 0x12 + 2;
    }
    data_length += 2;

    char *wb = (char*)malloc(data_length);

    memcpy(wb, Action::NAMES_CATEGORY, 4); wb += 4;

    uint32_t_pack(wb, (uint32_t)data_length-8); wb += sizeof(uint32_t);

    for(int i=0; i < _zones.size(); i++)
    {
        Zone *zone = _zones.at(i);
        if(_zones.at(i)->_actions.size())
        {
            uint16_t_pack(wb, i); wb += sizeof(uint16_t);
            for(int j=0; j < zone->_actions.size(); j++)
            {
                uint16_t_pack(wb, j); wb += sizeof(uint16_t);

                memset(wb, 0, 0x10); // clear memory
                sprintf(wb, "%s", zone->_actions.at(j).getName().c_str()); wb += 0x10;
            }
            uint16_t_pack(wb, 0xFFFF); wb += sizeof(uint16_t);
        }
    }
    uint16_t_pack(wb, 0xFFFF); // wb += sizeof(uint16_t);

    return data_length;
}

size_t GameDataParser::_getENDFData(char* buffer){
    const size_t size = 8;

    size_t bytesWritten = 0;
    if(buffer) memcpy(buffer+bytesWritten, GameDataParser::END_CATEGORY, CATEGORY_LENGTH);
    bytesWritten += CATEGORY_LENGTH;

    if(buffer) memset(buffer+bytesWritten, 0, 4);
    bytesWritten += 4;

    if(size != bytesWritten)
        printf("ERROR!\n");
    return bytesWritten;
}

size_t GameDataParser::_getOriginalData(const char *category, char* buffer)
{
    range r = _fileIndex.find(category)->second;

    size_t bytesWritten = 0;
    if(buffer) sprintf(buffer, "%s", category);
    bytesWritten += CATEGORY_LENGTH;

    if(buffer) uint32_t_pack(buffer+bytesWritten, (uint32_t)r.second);
    bytesWritten += sizeof(uint32_t);

    fseek(_file, r.first, SEEK_SET);
    if(buffer) fread(buffer+bytesWritten, r.second, 1, _file);
    bytesWritten += r.second;

    if(bytesWritten != r.second +8)
        printf("ERROR!\n");

    return bytesWritten;
}
#pragma mark -
vector<Tile*> GameDataParser::getItems()
{
    vector<Tile*> items;
    copy_if(_tiles.begin(), _tiles.end(), std::back_inserter(items), [](Tile *t) {
        return t->name.length() != 0;
    });
    return items;
}

#pragma mark - OpenGL
void GameDataParser::prepareTextures()
{
}

#pragma mark - Reading Helpers
// These functions can be enhanced to fix endianess if necessary
Category* GameDataParser::_readHeader()
{
    Category *h = new Category;
    if(fread(h, sizeof(Category), 1, _file) != 1)
    {
        return NULL;
    }
    // fix endianess
    return h;
}

ZoneSubheader* GameDataParser::_readZoneSUB()
{
    ZoneSubheader *s = new ZoneSubheader;
    if(fread(s, sizeof(ZoneSubheader), 1, _file) != 1)
        return NULL;

    // fix endianess
    return s;
}

range GameDataParser::getCategory(const char* name)
{
    return _fileIndex.find(name)->second;
}