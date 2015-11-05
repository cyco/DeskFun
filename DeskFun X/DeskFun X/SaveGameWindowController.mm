//
//  SaveGameWindowController.m
//  DeskFun X
//
//  Copyright © 2015 chris. All rights reserved.
//

#import "SaveGameWindowController.h"

#define PRINT_STATE 1
#define PRINT_ZONEHEADER 0
#define PRINT_ZONE 1

#if PRINT_ZONEHEADER
#define printheader(...) printf(__VA_ARGS__);
#else
#define printheader(...) do{} while(0);
#endif

#if PRINT_ZONE
#define printzone(...) { char indent[indentCount+1]; \
    memset(indent, 0, indentCount+1); \
    memset(indent, '\t', indentCount); \
    printf("%s", indent); \
    printf(__VA_ARGS__); }
#else
#define printzone(...) do{} while(0);
#endif

#if PRINT_STATE
#define printstate(...) printf(__VA_ARGS__);
#else
#define printstate(...) do{} while(0);
#endif

@interface SaveGameWindowController ()
@end
void parseWorld(int zoneCount, FILE *file, GameDataParser* _data);
void parseZoneHeader(FILE* file, GameDataParser *_data);
void parseZone(FILE* file, GameDataParser *_data);
ScriptTile *parseHotspots(FILE *file, Zone *zone, GameDataParser* _data, uint32_t *outCount);
void parsePuzzles(FILE *file, GameDataParser* _data);
void parseState(FILE *file, GameDataParser* _data);

@implementation SaveGameWindowController
static int indentCount = 0;
- (void)windowDidLoad {
    [super windowDidLoad];

    [self _parseFile];
}

- (NSString*)windowNibName
{
    return @"SaveGameWindowController";
}

- (void)_parseFile
{
#define REPORT() { \
char indent[indentCount+1];\
memset(indent, 0,   indentCount+1);\
memset(indent, '\t', indentCount);\
printf("%s-> at 0x%02lx now\n", indent, ftell(file)); }

#define ABORT() { printf("\naborting at 0x%02lx\n", ftell(file)); return; }

    uint8_t unknownByte; uint32_t unknownInt;

    NSFileHandle *handle = [NSFileHandle fileHandleForReadingFromURL:self.fileURL error:nil];
    FILE *file = fdopen([handle fileDescriptor], "r");

    // check file header
    void* buffer = (void *)malloc(9);
    fread(buffer, 9, 1, file);
    if(memcmp(buffer, "YODASAV44", 9) != 0)
    {
        NSLog(@"Unexpected file magic!");
        return;
    }
    printf("found header YODASAV44\n");

    // skip over some unknown bytes
    for(int i=0; i < 4; i++){
        fread(&unknownByte, sizeof(uint8_t), 1, file);
        printf("%02x ", unknownByte);
    }
    printf("\n");

    fread(&unknownInt, sizeof(uint32_t), 1, file);
    // printf("unknown: 0x%02x (%d)\n", unknownInt, unknownInt);

    fread(&unknownInt, sizeof(uint32_t), 1, file);
    // printf("unknown: 0x%02x (%d)\n", unknownInt, unknownInt);

    parsePuzzles(file, _data);
    parsePuzzles(file, _data);

    parseWorld(4, file, _data);
    parseWorld(10*10, file, _data);

    parseState(file, _data);
}

void parseWorld(int zoneCount, FILE *file, GameDataParser* _data){
    for(int i=0; i < zoneCount; i++){
        parseZoneHeader(file, _data);
    }

    printf("\n");

    uint32_t y,x;
    while(fread(&y, sizeof(uint32_t), 1, file) && fread(&x, sizeof(uint32_t), 1, file) && (uint32_t)~x && (uint32_t)~y) {
        parseZone(file, _data);
    }
}

void parseZoneHeader(FILE *file, GameDataParser* _data){
    uint8_t visited;
    fread(&visited, sizeof(uint8_t), 1, file);

    uint8_t unknownBytes[19];
    fread(&unknownBytes, sizeof(uint8_t), 19, file);

    uint16_t zoneID;
    fread(&zoneID, sizeof(uint16_t), 1, file);

    uint16_t unknownPuzzleByte, loseItem, gainItem;
    fread(&unknownPuzzleByte, sizeof(uint16_t), 1, file);
    fread(&loseItem, sizeof(uint16_t), 1, file);
    fread(&gainItem, sizeof(uint16_t), 1, file);

    uint16_t unknownGoalByte, loseItem2, goalItem;
    fread(&unknownGoalByte, sizeof(uint16_t), 1, file);
    fread(&loseItem2, sizeof(uint16_t), 1, file);
    fread(&goalItem, sizeof(uint16_t), 1, file);

    uint8_t moreUnknownBytes[8];
    fread(&moreUnknownBytes, sizeof(uint8_t), 8, file);

    printheader("parseZoneHeader: 0x%03x\n", zoneID);
    printheader(" -> visited: %s", visited ? "YES" : "NO");
    if((uint16_t)~loseItem) printheader(" -> lose 0x%0x (%s)\n", loseItem, _data->_tiles[loseItem]->name.c_str());
    if((uint16_t)~gainItem) printheader(" -> gain 0x%0x (%s)\n", gainItem, _data->_tiles[gainItem]->name.c_str());
    if((uint16_t)~loseItem2) printheader(" -> also lose 0x%0x (%s)\n", loseItem2, _data->_tiles[loseItem2]->name.c_str());
    if((uint16_t)~goalItem) printheader(" -> gain goal 0x%0x (%s)\n", goalItem, _data->_tiles[goalItem]->name.c_str());

    if((uint16_t)~zoneID && zoneID < _data->_zones.size()){
        Zone *z = _data->_zones[zoneID];
        z->_visited = visited;
    }
}

void parseZone(FILE* file, GameDataParser *_data){
    char indent[indentCount+1];
    memset(indent, 0,   indentCount+1);
    memset(indent, '\t', indentCount);

    uint16_t zoneID;
    fread(&zoneID, sizeof(uint16_t), 1, file);
    printzone("%szone: 0x%03x\n", indent, zoneID);
    printf("parse zone %hx at 0x%0lx\n", zoneID, ftell(file));
    Zone *zone = _data->_zones[zoneID];
    if(!zone){
        printzone("%s[ERROR] parsed wrong zone id\n", indent);
        return;
    }

    uint32_t visited, maybeSolved;
    fread(&visited, sizeof(uint32_t), 1, file);
    fread(&maybeSolved, sizeof(uint32_t), 1, file);
    ScriptTile *hotspots = NULL;
    uint32_t hotspotCount = (uint32_t)~0;

    hotspots = parseHotspots(file, zone, _data, &hotspotCount);

    if(zone->_visited){
        if(hotspots == NULL){
            uint8_t unknown[0xC];
            fread(&unknown, sizeof(uint8_t), 0xC, file);
        }

        NSUInteger tileCount = zone->getSize().width * zone->getSize().height * 3;
        printzone("-> tile data (0x%lx bytes) at 0x%lx\n", tileCount*2, ftell(file));

        for(int i=0; i < tileCount; i++){
            uint16_t tileID;
            fread(&tileID, sizeof(uint16_t), 1, file);
        }

        uint32_t unknown2;
        fread(&unknown2, sizeof(uint32_t), 1, file);
        printzone("-> unknown2: %0x\n", unknown2);

        hotspots = parseHotspots(file, zone, _data, &hotspotCount);

        uint32_t unknownCount;
        fread(&unknownCount, sizeof(uint32_t), 1, file);
        for(int i=0; i < unknownCount; i++){
            uint32_t length = 92;

            uint8_t unknownData[length];
            fread(&unknownData, sizeof(uint8_t), length, file);
        }

        uint32_t actionCounterCount;
        fread(&actionCounterCount, sizeof(uint32_t), 1, file);

        uint32_t counterValue;
        for(int i=0; i < actionCounterCount; i++) {
            fread(&counterValue, sizeof(uint32_t), 1, file);
        }
    }

    for(int i=0; i < hotspotCount && hotspots != NULL; i++){
        ScriptTile &spot = hotspots[i];
        if(spot.type == DoorIn && (uint16_t)~spot.arg2){
            Zone *room = _data->_zones[spot.arg2];
            room->_visited = zone->_visited;
            indentCount ++;
            parseZone(file, _data);
            indentCount --;
        }
    }

    delete [] hotspots;
}

ScriptTile *parseHotspots(FILE *file, Zone *zone, GameDataParser* _data, uint32_t* outCount){
    uint32_t hotspotCount;
    fread(&hotspotCount, sizeof(uint32_t), 1, file);

    ScriptTile *hotspots = NULL;

    if((uint32_t)~hotspotCount){
        hotspots = new ScriptTile[hotspotCount];
        for(int i=0; i < hotspotCount; i++){
            fread(&(hotspots[i].arg1), sizeof(uint16_t), 1, file);
            fread(&(hotspots[i].arg2), sizeof(uint16_t), 1, file);
            fread(&(hotspots[i].type), sizeof(uint32_t), 1, file);
            fread(&(hotspots[i].x), sizeof(uint16_t), 1, file);
            fread(&(hotspots[i].y), sizeof(uint16_t), 1, file);
        }
    }

    *outCount = hotspotCount;
    return hotspots;
}

void parsePuzzles(FILE *file, GameDataParser* _data){
    uint16_t puzzleCount;
    fread(&puzzleCount, sizeof(uint16_t), 1, file);
    // printf("0x%02x puzzles in first batch\n", puzzleCount);
    while(puzzleCount--){
        uint16_t puzzleID;
        fread(&puzzleID, sizeof(uint16_t), 1, file);
        // printf("puzzle: 0x%02x\n", puzzleID);
    }
}

void parseState(FILE *file, GameDataParser* _data)
{
    long fileSize, currentPosition;

    currentPosition = ftell(file);
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    // printf(" with %ld bytes\n", fileSize);
    fseek(file, currentPosition, SEEK_SET);

    // parse inventory
    uint32_t inventorySize;
    fread(&inventorySize, sizeof(uint32_t), 1, file);
    printstate("inventory contains %d items\n", inventorySize);
    while(inventorySize--){
        uint16_t itemID;
        fread(&itemID, sizeof(uint16_t), 1, file);
        Tile *tile = _data->getTile(itemID);
        printstate("item: %s (0x%04x)\n", tile->name.c_str(), itemID);
    }

    // parse current location on world zone
    uint16_t currentZone, worldX, worldY;
    fread(&currentZone, sizeof(uint16_t), 1, file);
    fread(&worldX, sizeof(uint32_t), 1, file);
    fread(&worldY, sizeof(uint32_t), 1, file);
    printstate("current zone: %03x (world x: %d, y: %d\n", currentZone, worldX, worldY);

    // current weapon
    uint16_t weapon;
    fread(&weapon, sizeof(uint16_t), 1, file);
    printstate("equipped weapon: %s\n", weapon != 0xFFFF ? _data->_chars[weapon]->getName().c_str() : "<none>");

    // current weapon
    uint16_t ammo;
    uint16_t maxAmmo = 0x1e;
    fread(&ammo, sizeof(uint16_t), 1, file);
    printstate("ammo: %02x (%2.2f%%)\n", ammo, ammo / (double)maxAmmo);

    // unknown ints
    printf("0x%02x\n", ftell(file));

    uint32_t z, x, y;
    fread(&z, sizeof(uint32_t), 1, file);
    fread(&x, sizeof(uint32_t), 1, file);
    fread(&y, sizeof(uint32_t), 1, file);
    printstate("hero location: %dx%dx%d (original %dx%dx%d)\n", x/32, y/32, z/32, x, y, z);

    // health
    uint32_t health1, health2;
    fread(&health1, sizeof(uint32_t), 1, file);
    fread(&health2, sizeof(uint32_t), 1, file);
    printstate("health: %d, %d\n", health1, health2);

    // 0x18 unknown bytes left
    uint32_t unknown[4];
    fread(unknown, sizeof(uint32_t), 4, file);
    for(int i=0; i < 4; i++)
        printstate("unknown: %d (0x%02x)\n", unknown[i], unknown[i]);

    if(unknown[0] != 0x32)  printstate("unkonwn[0] is interesting\n");
    if(unknown[1] != 0x00)  printstate("unkonwn[1] is interesting\n");
    if(unknown[2] != 0x0b)  printstate("unkonwn[2] is interesting\n");
    if(unknown[3] != 0x00)  printstate("unkonwn[3] is interesting\n");

    uint32_t endPuzzle1, endPuzzle2;
    fread(&endPuzzle1, sizeof(uint32_t), 1, file);
    fread(&endPuzzle2, sizeof(uint32_t), 1, file);
    printstate("ending: 0x%02x\n", endPuzzle1);
    if(endPuzzle1 != endPuzzle2)
        printstate("Found two different endings %02x & %02x\n", endPuzzle1, endPuzzle2);
    printstate("done (%ld bytes left)\n", fileSize- ftell(file));
}
@end
