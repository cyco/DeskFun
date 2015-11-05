//
//  Action.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef __Desktop_Adventure__Action__
#define __Desktop_Adventure__Action__

#include <stdio.h>
#include <string>
#include "GameObject.hpp"

#define packed __attribute__ ((__packed__))

using namespace std;

class Zone;
class ZoneScene;
class Action : GameObject
{
public:
    typedef struct packed {
        typedef enum : uint16_t {
            PlaceTile  = 0x00,
            RemoveTile = 0x01,
            MoveTile   = 0x02,
            DrawTile   = 0x03,
            SpeakHero  = 0x04,
            SpeakNPC   = 0x05,
            SetTileNeedsDisplay = 0x06,
            SetRectNeedsDisplay = 0x07,
            Wait   = 0x08,
            Redraw = 0x09,
            PlaySound = 0x0a,
            // = 0x0b, // update inventory?
            RollDice = 0x0c,
            SetCounter = 0x0d,
            // = 0x0e, // AddToCounter
            // = 0x0f,
            HideHero = 0x10,
            ShowHero = 0x11,
            SetHero  = 0x12,
            // = 0x13,
            DisableAction = 0x14,
            DisableHotspot = 0x15,
            EnableHotspot  = 0x16,
            // = 0x17, // spawnEnemy(id, 0x00)
            // = 0x18,
            // = 0x19,
            // = 0x1a,
            DropItem   = 0x1b,
            AddItem    = 0x1c,
            RemoveItem = 0x1d,
            // = 0x1e, // mark zone as solved?
            // = 0x1f,
            ChangeZone = 0x21,
            // = 0x22,
            // = 0x23,
            SetRandom = 0x24,
            AddHealth = 0x25,

            OPCODE_COUNT
        } Opcode;

        typedef enum : uint16_t {
            ZoneNotInitalized = 0x00,
            ZoneEntered = 0x01, // Once per zone entered
            Bump = 0x02,
            PlaceItem = 0x03,
            StandingOn = 0x04,
            CounterIs = 0x05,
            RandomIs = 0x06,
            RandomIsNot = 0x07, // guessed
            TileIs = 0x08,
            EnterByPlane = 0x09,
            TileAtIs = 0x0a,
            TileAt = 0x0b,
            // = 0x0c,
            HasItem      = 0x0d,
            RequiredItem = 0x0e,
            Ending       = 0x0f,
            // = 0x10,
            // = 0x12,
            HealthIsLessThan = 0x13,
            HealthIsMoreThan = 0x14,
            // = 0x16,
            PlaceItemIsNot = 0x17,
            HeroIsAt = 0x18,
            // = 0x19,
            // = 0x1a,
            // = 0x1b,
            GamesWonIsExactly = 0x1c,
            // = 0x1d,
            // = 0x1e,
            CounterIsNot = 0x1f,
            // = 0x20,
            // = 0x21,
            // = 0x22,
            GamesWonBiggerThan = 0x23,

            CONDITION_CODE_COUNT,
        } ConditionCode;

        uint16_t opcode;
        uint16_t arguments[5];
        uint16_t additionalDataLength;
        char* additionalData;

        static std::string instructionName(Opcode op)
        {
            switch(op) {
                case PlaceTile: return "PlaceTile";
                case RemoveTile: return "RemoveTile";
                case MoveTile: return "MoveTile";
                case SpeakHero: return "SpeakHero";
                case DrawTile: return "DrawTile";
                case SpeakNPC: return "SpeakNPC";
                case SetTileNeedsDisplay: return "SetTileNeedsDisplay";
                case SetRectNeedsDisplay: return "SetRectNeedsDisplay";
                case Wait: return "Wait";
                case Redraw: return "Redraw";
                case PlaySound: return "PlaySound";
                case RollDice: return "RollDice";
                case SetCounter: return "SetCounter";
                case DisableAction: return "DisableAction";
                case DisableHotspot: return "DisableHotspot";
                case EnableHotspot: return "EnableHotspot";
                case ShowHero: return "ShowHero";
                case HideHero: return "HideHero";
                case SetHero: return "SetHero";
                case DropItem: return "DropItem";
                case AddItem: return "AddItem";
                case RemoveItem: return "RemoveItem";
                case ChangeZone: return "ChangeZone";
                case AddHealth: return "AddHealth";
                case SetRandom: return "SetRandom";
                default: return "Unknown";
            }
        }

        static std::string conditionName(ConditionCode c)
        {
            switch(c) {
                case ZoneNotInitalized: return "ZoneNotInitalized";
                case ZoneEntered: return "ZoneEntered";
                case Bump: return "Bump";
                case PlaceItem: return "PlacedItem";
                case StandingOn: return "StandingOn";
                case CounterIs: return "CounterIs";
                case RandomIs: return "RandomIs";
                case RandomIsNot: return "RandomIsNot";
                case TileIs: return "TileIs";
                case EnterByPlane: return "EnterByPlane";
                case TileAtIs: return "TileAtIs";
                case TileAt: return "TileAt";
                case HasItem: return "HasItem";
                case RequiredItem: return "RequiredItem";
                case Ending: return "Ending";
                case HealthIsLessThan: return "HealthIsLessThan";
                case HealthIsMoreThan: return "HealthIsMoreThan";
                case PlaceItemIsNot: return "PlaceItemIsNot";
                case HeroIsAt: return "HeroIsAt";
                case GamesWonIsExactly: return "GamesWonIsExactly";
                case CounterIsNot: return "CounterIsNot";
                case GamesWonBiggerThan: return "GamesWonBiggerThan";
                default: return "Unknown";
            }
        }
    } Instruction;

    const static char CATEGORY[];
    const static char NAMES_CATEGORY[];
    const static char INLINE_MARK[];

    const static unsigned short INSTRUCTION_SIZE;

    static void ReadFromFile(FILE *file, range range, vector<Zone*> zones);
    static void ReadNamesFromFile(FILE *file, range range, vector<Zone*> zones);

    Action(FILE *file, Zone* z, unsigned actionIndex);
    static uint32_t GetIndyActionSize(unsigned int zoneIdx, unsigned int actionIdx);

    void readName(FILE *file);

    size_t write(char *buffer);

    string& getName();
    string& getText();

    bool executeInScene(ZoneScene *scene);
private:
    string name;
    string text;

    size_t _state;
    bool _isAtEnd;

    size_t _writeInstruction(Instruction i, char *buffer);

    void _clearInstructions(void);

public:
    Action();
    ~Action();

    vector<Instruction> conditions;
    vector<Instruction> instructions;

};

#endif /* defined(__Desktop_Adventure__Action__) */
