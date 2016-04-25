//
//  Action.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "Action.hpp"
#include "Zone.hpp"
#include "ZoneScene.hpp"
#include "Packing.h"
#include "ZoneTransitionScene.hpp"
#include "SoundMixer.hpp"
#include "Engine.hpp"

#include "GameDataParser.hpp"

const char Action::CATEGORY[] = "ACTN";
const char Action::NAMES_CATEGORY[] = "ANAM";
const char Action::INLINE_MARK[] = "IACT";
const unsigned short Action::INSTRUCTION_SIZE = 0xE;

void Action::ReadFromFile(FILE *file, range range, vector<Zone*> zones)
{
    fseek(file, range.first, SEEK_SET);

    uint16_t zoneIdx;
    while(fread(&zoneIdx, sizeof(uint16_t), 1, file) && zoneIdx != 0xFFFF)
    {
        uint16_t number_of_actions;
        fread(&number_of_actions, sizeof(uint16_t), 1, file);
        for(int i=0; i < number_of_actions; i++) {
            Zone *zone = zones[zoneIdx];
            zone->_actions.push_back(new Action(file, zone, i));
        }
    }
}

Action::Action(FILE *file, Zone* zone, unsigned actionIdx)
{
    _state = 0;
    _isAtEnd = false;

    char category_identifier[GameObject::CATEGORY_LENGTH];
    fread(category_identifier, sizeof(char), GameObject::CATEGORY_LENGTH, file);
    if(memcmp(category_identifier, Action::INLINE_MARK, GameObject::CATEGORY_LENGTH) != 0)
    {
        printf("Did not find expected inline category '%s' at 0x%08lx\n", Action::INLINE_MARK, ftell(file) - GameObject::CATEGORY_LENGTH);
    }

    uint32_t size;
    fread(&size, sizeof(uint32_t), 1, file);

    if(size == 0)
    {
        // indy does not store size :/
        size = GetIndyActionSize((unsigned int)zone->index, actionIdx);
        size -= 8; // fake size contains HEADER and SIZE
    }

    uint16_t conditionCount = 0;
    fread(&conditionCount, sizeof(uint16_t), 1, file);
    conditions.clear();
    conditions.resize(conditionCount);

    for(int c=0; c < conditionCount; c++)
    {
        Instruction &i = conditions[c];
        fread(&i, sizeof(Instruction) - sizeof(char*), 1, file);

        i.additionalData = (char*)malloc(i.additionalDataLength);
        fread(i.additionalData, sizeof(char), i.additionalDataLength, file);
    }

    uint16_t instructionCount = 0;
    fread(&instructionCount, sizeof(uint16_t), 1, file);
    instructions.clear();
    instructions.resize(instructionCount);
    for(int c=0; c < instructionCount; c++)
    {
        Instruction &i = instructions[c];
        fread(&i, sizeof(Instruction) - sizeof(char*), 1, file);

        i.additionalData = (char*)malloc(i.additionalDataLength);
        fread(i.additionalData, sizeof(char), i.additionalDataLength, file);
    }
}

Action::~Action() {
    // _clearInstructions();
    for(Instruction i: conditions)
        if(i.additionalData) free(i.additionalData);
    conditions.clear();
    
    for(Instruction i : instructions)
        if(i.additionalData) free(i.additionalData);
    instructions.clear();
}
#include "ActionIndySizes.cpp"

void Action::ReadNamesFromFile(FILE *file, range range, vector<Zone*> zones)
{
    if(GameContext::CurrentContext()->getGameStyle() == GameStyleYoda)
        return;

    fseek(file, range.first, SEEK_SET);

    uint16_t zoneID, actionID;
    while (fread(&zoneID, sizeof(uint16_t), 1, file) && zoneID != 0xFFFF) {
        while (fread(&actionID, sizeof(uint16_t), 1, file) && actionID != 0xFFFF) {
            Zone *z = zones[zoneID];
            if(actionID >= z->_actions.size())
            {
                printf("Should not need to create additional actions when reading name %hu of zone #%hu\n", actionID, zoneID);
                z->_actions.push_back(new Action());
            }
            zones[zoneID]->_actions[actionID]->readName(file);
        }
    }
}

Action::Action()
{
    _state = 0;
    _isAtEnd = false;
}

#pragma mark -
size_t Action::write(char *buffer)
{
    size_t bytesWritten = 0;

    if(GameContext::CurrentContext()->getGameStyle() != GameStyleYoda) return bytesWritten;

    if(buffer) memcpy(buffer, INLINE_MARK, MARKER_LENGTH);
    bytesWritten += MARKER_LENGTH;

    char *sizeLocation = buffer ? buffer+bytesWritten : NULL;
    // size will be written later
    bytesWritten += sizeof(uint32_t);

    if(buffer) uint16_t_pack(buffer+bytesWritten, (uint16_t)conditions.size());
    bytesWritten += sizeof(uint16_t);

    for(int c=0; c < conditions.size(); c++)
        bytesWritten += _writeInstruction(conditions[c], buffer ? buffer+bytesWritten : NULL);

    if(buffer) uint16_t_pack(buffer+bytesWritten, (uint16_t)instructions.size());
    bytesWritten += sizeof(uint16_t);

    for(int i=0; i < instructions.size(); i++)
        bytesWritten += _writeInstruction(instructions[i], buffer ? buffer+bytesWritten : NULL);

    if(sizeLocation) uint32_t_pack(sizeLocation, (uint32_t)bytesWritten-8);

    return bytesWritten;
}


size_t Action::_writeInstruction(Instruction i, char *buffer)
{
    size_t bytesWritten = 0;

    if(buffer) uint16_t_pack(buffer+bytesWritten, i.opcode);
    bytesWritten += sizeof(uint16_t);

    for(int j=0; j < 5; j++)
    {
        if(buffer) uint16_t_pack(buffer+bytesWritten, i.arguments[j]);
        bytesWritten += sizeof(uint16_t);
    }

    if(buffer) uint16_t_pack(buffer+bytesWritten, i.additionalDataLength);
    bytesWritten += sizeof(uint16_t);

    if(buffer) memcpy(buffer+bytesWritten, i.additionalData, i.additionalDataLength);
    bytesWritten += i.additionalDataLength;

    return bytesWritten;
}

#pragma mark -
void Action::_clearInstructions(void)
{
    for(int i=0; i < conditions.size(); i++)
        free(conditions[i].additionalData);
    conditions.clear();

    for(int i=0; i < instructions.size(); i++)
        free(instructions[i].additionalData);
    instructions.clear();
}

void Action::readName(FILE *file)
{
    const size_t nameLength = 0x10;
    char data[nameLength];
    fread(data, sizeof(char), nameLength, file);
    name = string(data);
}

#pragma mark -
bool Action::executeInScene(ZoneScene *scene)
{
    return true;

    /*
    Zone *zone = scene->getZone();
        Engine *engine = scene->getEngine();
        GameDataParser *data = engine->data;
        for(size_t i=_state; i < subActionCount; i++)
        {
            _state++;

            size_t offset = i * 0xE + 0x12;
            uint16_t opcode = uint16_t_read((const char*)_raw_data+offset);
            uint16_t arg1 = uint16_t_read((const char*)_raw_data+offset+2);
            uint16_t arg2 = uint16_t_read((const char*)_raw_data+offset+4);
            uint16_t arg3 = uint16_t_read((const char*)_raw_data+offset+6);
            uint16_t arg4 = uint16_t_read((const char*)_raw_data+offset+8);
            uint16_t arg5 = uint16_t_read((const char*)_raw_data+offset+10);
            uint16_t additionalData = uint16_t_read((const char*)_raw_data+offset+6);

            Tile *t;
            ZoneTransitionScene *transition;
            switch (opcode) {
                case 0x00: // printf(" - Place Tile");
                    // x,y,z, tileID
                    zone->setTile(data->getTile(arg4), arg1, arg2, arg3);
                    break;
                case 0x01: // printf(" - Remove Tile");
                    zone->setTile(NULL, arg1, arg2, arg3);
                    break;
                case 0x02: // printf(" - Move Tile");
                    t = zone->getTile(arg1, arg2, arg3);
                    zone->setTile(NULL, arg1, arg2, arg3);
                    zone->setTile(t, arg4, arg5, arg3);
                    break;
                case 0x04: // printf(" - Hero Speak");
                    break;
                case 0x05: // printf(" - NPC Speak");
                    // x = arg1, y = arg2
                    // SH = arg3
                    // OW = arg4
                    // TE = arg5
                    break;
                case 0x07: // printf(" - Flush Screen, Part I");
                    return !_isAtEnd;
                    break;
                case 0x08: // printf(" - Flush Screen, Part II");
                    //return !_isAtEnd;
                    break;
                case 0x09: // printf(" - Flush Screen, Part III");
                    //return !_isAtEnd;
                    break;
                case 0x0A: // printf(" - Play Sound");
                    engine->mixer->play(data->_sounds[arg1], SoundMixer::SoundEffect);
                    break;
                case 0x21: // printf(" - Change Zone");
                    transition = new ZoneTransitionScene(ZoneTransitionScene::RoomTransition, scene);
                    transition->setEngine(engine);
                    transition->heroStart = (GamePoint){.x=0,.y=0,.l=0};
                    transition->heroEnd   = (GamePoint){.x=arg2,.y=arg3,.l=0};
                    transition->newZone   = data->getZone(arg1);
                    engine->pushScene(transition);
                    break;
                default:
                    break;
            }

            if(_state == subActionCount)
            {
                _isAtEnd = true;
                break;
            }
        }
    }

    return !_isAtEnd;
     */
}

string& Action::getName()
{
    return name;
}
string& Action::getText()
{
    return text;
}