//
//  GameDataParser.h
//  DAParser
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __DAParser__GameDataParser__
#define __DAParser__GameDataParser__

#ifdef __cplusplus
#include <string>
#include <map>
#include <vector>
#endif



#include "Tile.hpp"
#include "Char.hpp"
#include "Zone.hpp"
#include "Puzzle.hpp"
#include "Sound.hpp"

#include "GameContext.hpp"

#include "SoundStore.hpp"
#include "TileStore.hpp"
#include "ZoneStore.hpp"

#define packed __attribute__ ((__packed__))
typedef struct packed {
    char identifier[4];
    uint32_t size;
} Category;

typedef struct packed {
    uint16_t unknown;
    uint32_t size;
} ZoneSubheader;

using namespace std;

typedef pair<string, int> item;
struct GameDataParserException: public runtime_error
{
    GameDataParserException(string const& message)
    : runtime_error(message + " was thrown")
    {}
};

class GameDataParser
{
public:
    static const char VERSION_CATEGORY[];
    static const char LOADING_CATEGORY[];
    static const char END_CATEGORY[];
    static const int  CATEGORY_LENGTH;

    GameDataParser(const string& path);
    GameDataParser(const char *path);
    ~GameDataParser();

    vector<Tile*> getItems();
private:
    FILE *_file;
    map<string,range> _fileIndex;

    void (*updateLocationIndicator)(void);

public:
    bool _isYodaVersion;

    unsigned char *_loadingImage;
    long _loadingImagePixelCount;

    SoundStore      _sounds;
    vector<Char*>   _chars;
    ZoneStore       _zones;
    vector<Puzzle*> _puzzles;
    TileStore       _tiles;

    void _readFileIndex();
    void _readSetupImage(range category_range);
    void _readZAUX();
    void _readZAX2();
    void _readZAX3();
    void _readZAX4();

    Category*      _readHeader();
    ZoneSubheader* _readZoneSUB();

    void prepareTextures();

    Zone* getZone(int i){ return _zones[i]; };
    size_t getZoneCount(){ return _zones.size(); };
    Tile*  getTile(int i){ return GameContext::CurrentContext()->getTile(i); };
    
    unsigned long long getData(char* buffer);
    size_t _getVersionData(char* buffer);
    size_t _getStupData(char* buffer);
    size_t _getZAUXData(char *buffer);
    size_t _getZAX2Data(char *buffer);
    size_t _getZAX3Data(char *buffer);
    size_t _getZAX4Data(char *buffer);
    size_t _getHTSPData(char *buffer);
    size_t _getACTNData(char *buffer);
    size_t _getPUZ2Data(char *buffer);
    size_t _getCHARData(char *buffer);
    size_t _getCHWPData(char *buffer);
    size_t _getCAUXData(char *buffer);
    size_t _getTNAMData(char *buffer);
    size_t _getZoneNameData(char *buffer);
    size_t _getPuzzleNameData(char *buffer);
    size_t _getActionNameData(char *buffer);
    void _GameDataParser_readACTN(GameDataParser &data);
    size_t _getENDFData(char* buffer);

    size_t _getOriginalData(const char *category, char* buffer);

private:
    range getCategory(const char* name);
};

#endif /* defined(__DAParser__GameDataParser__) */
