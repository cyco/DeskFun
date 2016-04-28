//
//  YodaDocument.cpp
//  DeskFun X
//
//  Created by Christoph Leimbrock on 03/04/16.
//  Copyright © 2016 chris. All rights reserved.
//

#include "YodaDocument.hpp"
#include "GameDataParser.hpp"
#include "MapGenerator.hpp"

#define Message(fmt, ...) if(this->logging) printf(fmt, ##__VA_ARGS__);

#define PRINT_ARRAY_SHUFFLE_RANDS 0
YodaDocument::YodaDocument(const char* path) {
    parser = new GameDataParser(path);
    
    zones.clear();
    for(int i=0; i < parser->getZoneCount(); i++) {
        zones.push_back(parser->getZone(i));
    }
    
    puzzles.clear();
    for(Puzzle *p : parser->_puzzles) {
        puzzles.push_back(p);
    }
    
    puzzles.clear();
    for(int i=0; i < parser->_puzzles.size(); i++) {
        Puzzle *puzzle = parser->_puzzles[i];
        if(i == 0xBD || i==0xC5)
            puzzle->type = PUZZLE_TYPE_DISABLED;
        puzzles.push_back(puzzle);
    }
    
    tiles.clear();
    for(Tile *t : parser->_tiles) {
        tiles.push_back(t);
    }
    
    puzzles_can_be_reused = -1;
    world_things.resize(100);
    for(int i=0; i < 100; i++) {
        world_things[i].zone_type = (ZONE_TYPE)0;
        world_things[i].zone_id = -1;
        world_things[i].findItemID = -1;
        world_things[i].unknown606 = -1;
        world_things[i].requiredItemID = -1;
        world_things[i].unknown612 = -1;
    }
}

YodaDocument::~YodaDocument() {
    zones.clear();
    puzzles.clear();
    tiles.clear();
    world_things.clear();
        
    delete parser;
}
