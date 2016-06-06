//
//  ZoneScene.cpp
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#include "ZoneScene.hpp"
#include "Zone.hpp"
#include "Packing.h"

#include "Engine.hpp"
#include "Direction.hpp"
#include "Hero.hpp"
#include "Action.hpp"

#include "PauseScene.hpp"
#include "MapScene.hpp"
#include "ZoneTransitionScene.hpp"
#include "Sound.hpp"

ZoneScene::ZoneScene(void)
    : _camera(this)
{}

void ZoneScene::init(void)
{
    _action = NULL;
}

void ZoneScene::cleanup(void)
{}


bool ZoneScene::update(double delta)
{
    if(!_zone) return false;

    _ticks++;

    Hero& hero = _engine->world->hero;
    hero.walking = false;

    if(_action != NULL)
    {
        executeAction(_action);
    }
    else
    {
        bool handleInput = true;
        for(int i=0; i < _zone->_actions.size(); i++)
        {
            handleInput &= executeAction(_zone->_actions[i]);
            if(_action) break;
        }

        locationChanged = FALSE;
        if(handleInput){
            _parseInput();
            hero.tick(1);

            _camera.update(delta);
        }
    }

    _zone->enteredByPlane = false;
    _zone->justEntered = false;
    _zone->initialized = true;
    
    return false;
}

void ZoneScene::prepareCamera(void)
{
    _camera.update(INFINITY);
}

void ZoneScene::_parseInput(void)
{
    InputManager *InputManager = _engine->InputManager;
    if(!InputManager) return;

    if(!_handleSpecialKeys())
    {
        _handleKeyboard();
        _handleMouse();
    }
}

bool ZoneScene::_handleSpecialKeys(void)
{
    InputManager *InputManager = _engine->InputManager;

    KeyStates states  = InputManager->getKeyStates();
    KeyStates changes = InputManager->getKeyStateChanges();

    if((states & InputManager::Pause) != 0 && (changes & InputManager::Pause))
    {
        printf("push pause scene\n");
        _engine->pushScene(new PauseScene());
        return true;
    }
    else if((states & InputManager::Map) != 0 && (changes & InputManager::Map))
    {
        printf("push map scene\n");
        _engine->pushScene(new MapScene());
        return true;
    }

    return false;
}

bool ZoneScene::_handleKeyboard(void)
{
    World *world = _engine->world;
    Hero &hero = world->hero;
    InputManager *InputManager = _engine->InputManager;

    KeyStates states  = InputManager->getKeyStates();

    hero.isDragging = (states&InputManager::Drag) != 0;

    return true;
}


bool ZoneScene::_handleMouse(void)
{
    World *world = _engine->world;
    InputManager *InputManager = _engine->InputManager;
    const Pointf mouseLocationInView = InputManager->getMouseLocation();

    const Pointf offset = _camera.getOffset();
    const Sizef  size   = _camera.getSize();

    const Pointf mouseLocationOnZone = (Pointf){
        .x = mouseLocationInView.x * size.width  - offset.x,
        .y = mouseLocationInView.y * size.height - offset.y,
    };
    const Pointf relativeLocation = (Pointf){
        .x = mouseLocationOnZone.x - world->hero.location.x - 0.5f,
        .y = mouseLocationOnZone.y - world->hero.location.y - 0.5f,
    };

    // printf("mouseOnView: %2.2fx%2.2f\n", mouseLocationInView.x, mouseLocationInView.y);
    // printf("mouseOnZone: %2.2fx%2.2f\n", mouseLocationOnZone.x, mouseLocationOnZone.y);
    // printf("relative: %2.2fx%2.2f\n", relativeLocation.x, relativeLocation.y);

    if(fabsf(relativeLocation.x) < 0.5f && fabsf(relativeLocation.y) < 0.5f)
    {        if(_engine->hostCallbacks.setCursor)

        _engine->hostCallbacks.setCursor(Direction::TooNear, _engine->hostCallbacks.host);
    }
    else
    {
        float direction = Direction::CalculateFromRelativePoint(relativeLocation);
        if(_engine->hostCallbacks.setCursor)
            _engine->hostCallbacks.setCursor(direction, _engine->hostCallbacks.host);

        world->hero.face(direction);
        if(InputManager->getLeftMouseDown())
            _moveHero(direction);
    }

    return false;
}

void ZoneScene::_moveHero(float direction)
{
    World *world = _engine->world;
    Hero &hero = world->hero;

    int diri = Direction::Confine(direction);
    Pointf point = Direction::RelativeCoordinates(diri, 1);
    GamePoint p = (GamePoint){.x=(int)point.x, .y=(int)point.y, .l=0};

    hero.walking = false;
    if(!hero.move(p, *_zone))
        _transition(p);
    else locationChanged = true;
}

bool ZoneScene::render(double delta)
{
    if(_zone != NULL)
    {
#ifdef glColor4f
        glColor4f(1.0, 1.0, 1.0, 1.0);
#endif
        Pointf effectiveOffset = _camera.getOffset();
        renderZone(effectiveOffset);
    }

    return false;
}

Pointf ZoneScene::getCurrentOffset()
{
    return _camera.getOffset();
}
#pragma mark -
void ZoneScene::setEngine(Engine *engine)
{
    _engine = engine;
    _camera.setEngine(engine);
}

void ZoneScene::setZone(Zone* zone)
{
    _zone = zone;
    _zone->justEntered  = true;
    _updateLocation();
}

Zone* ZoneScene::getZone()
{
    return _zone;
}

#pragma mark -
void ZoneScene::renderZone(Pointf offset)
{
    const Sizei zoneSize = _zone->getSize();

    Tile *t = NULL;
    for(int l = 0; l < 3; l++) {
        for(int y=0; y < zoneSize.height; y++)
            for(int x=0; x < zoneSize.width; x++) {
                t = _zone->getTile(x, y, l);
                if(t) {
                    _engine->renderer->renderTile(t->getTexture(_ticks), x+offset.x, y+offset.y, l);
                }
            }

        if(l == 1) _engine->world->hero.render(offset, _engine->renderer);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

#pragma mark -
void ZoneScene::_transition(GamePoint relativeMovement)
{
    const Sizei zoneSize = _zone->getSize();

    World *world = _engine->world;
    Hero   &hero = world->hero;

    GamePoint targetLocation = GamePointAdd(hero.location, relativeMovement);
    if(GamePointInBounds(targetLocation, zoneSize.width, zoneSize.height))
        return;

    GamePoint targetZone = world->zoneLocation;

    if(targetLocation.x < 0) targetZone.x -= 1;
    if(targetLocation.x >= zoneSize.width) targetZone.x += 1;
    if(targetLocation.y < 0) targetZone.y -= 1;
    if(targetLocation.y >= zoneSize.height) targetZone.y += 1;

    Zone *z = world->getZone(targetZone.x, targetZone.y);
    if(!z || z == _zone) return;

    GamePoint targetOnNewZone = targetLocation;
    if(targetLocation.x < 0) targetOnNewZone.x += z->getSize().width;
    else if(targetLocation.x >= zoneSize.width) targetOnNewZone.x -= zoneSize.width;
    if(targetLocation.y < 0) targetOnNewZone.y += z->getSize().height;
    else if(targetLocation.y >= zoneSize.height) targetOnNewZone.y -= zoneSize.height;

    if(z && z->tileWalkableAt(targetOnNewZone))
    {
        ZoneTransitionScene *scene = new ZoneTransitionScene(ZoneTransitionScene::ZoneTransition, this);

        scene->oldZone = _zone;
        scene->newZone = world->getZone(targetZone.x, targetZone.y);

        scene->heroStart = hero.location;
        scene->heroEnd   = targetOnNewZone;

        scene->zoneStart = world->zoneLocation;
        scene->zoneEnd   = targetZone;

        _engine->pushScene(scene);
    }
}

void ZoneScene::_updateLocation(void){
    if(!_engine || !_engine->world) return;

    unsigned short location = 0;
    World *world = _engine->world;

    int x = world->zoneLocation.x;
    int y = world->zoneLocation.y;

    if(world->getZone(x+1, y) != NULL)
        location |= DirectionEast;
    if(world->getZone(x-1, y) != NULL)
        location |= DirectionWest;
    if(world->getZone(x, y-1) != NULL)
        location |= DirectionNorth;
    if(world->getZone(x, y+1) != NULL)
        location |= DirectionSouth;

    if(_engine->hostCallbacks.updateLocation)
        _engine->hostCallbacks.updateLocation(location, _engine->hostCallbacks.host);
}


#pragma mark - Actions
bool ZoneScene::executeAction(Action *action)
{
    if(!action) return false;

    if(action != this->_action)
    {
        instructionPointer = 0;
        _action = NULL;

        bool conditionsApply = true;
        for(int c=0; conditionsApply && c < action->conditions.size(); c++)
            if(!evaulateCondition(action->conditions[c])) return true;
    }

    _action = NULL;

    for(int j=instructionPointer; j < action->instructions.size(); j++)
    {
        return false;
        bool wait = executeInstruction(action->instructions[j]);
        if(wait)
        {
            _action = action;
            instructionPointer = j+1;
            return false;
        }
    }

    return true;
}

bool ZoneScene::evaulateCondition(Action::Instruction condition)
{
    using C = Action::Instruction::ConditionCode;

    switch (condition.opcode) {
        case C::ZoneNotInitalized:
            return !_zone->initialized;
        case C::ZoneEntered:
            return _zone->justEntered;
        case C::GamesWonBiggerThan:
            return _engine->state.getGamesWon() > condition.arguments[0];
        case C::GamesWonIsExactly:
            return _engine->state.getGamesWon() == condition.arguments[0];
        case C::StandingOn:
            return locationChanged && _engine->world->hero.location.x == condition.arguments[0]
            && _engine->world->hero.location.y == condition.arguments[1]
            && _zone->getTile(_engine->world->hero.location.x, _engine->world->hero.location.y, 0)->index == condition.arguments[2];
        case C::EnterByPlane:
            return _zone->enteredByPlane;
        case C::CounterIs:
            return _zone->getCounter() == condition.arguments[0];
        case C::CounterIsNot:
            return _zone->getCounter() != condition.arguments[0];
        case C::HealthIsLessThan:
            return _engine->world->hero.getHealth() <= condition.arguments[0];
        case C::HealthIsMoreThan:
            return _engine->world->hero.getHealth() >= condition.arguments[0];
        case C::HeroIsAt:
            return _engine->world->hero.location.x == condition.arguments[0] && _engine->world->hero.location.y == condition.arguments[1];

        case C::RandomIs:
            return _random == condition.arguments[0];
        case C::RandomIsNot:
            return _random != condition.arguments[0];
        case C::TileIs:
        {
            Tile *t = _zone->getTile(condition.arguments[1], condition.arguments[2], condition.arguments[4]);
            return (t == NULL && condition.arguments[0] == 0xFFFF) || (t != NULL && t->index == condition.arguments[0]);
        }
        case C::TileAt:
        {
            Tile *t = _zone->getTile(condition.arguments[1], condition.arguments[2], condition.arguments[4]);
            return (t == NULL && condition.arguments[0] == 0xFFFF) || (t != NULL && t->index == condition.arguments[0]);
        }
        case C::TileAtIs:
        {
            // TODO: check all tiles
            Tile *t = _zone->getTile(condition.arguments[1], condition.arguments[2], condition.arguments[4]);
            return (t == NULL && condition.arguments[0] == 0xFFFF) || (t != NULL && t->index == condition.arguments[0]);
        }

        default:
        //    printf("Unknown Condition\n");
            return false;
            break;
    }

    return true;
}

bool ZoneScene::executeInstruction(Action::Instruction instruction)
{
    using I = Action::Instruction::Opcode;
    switch (instruction.opcode) {
        case I::AddHealth:
            _engine->world->hero.addHealth(instruction.arguments[0]);
            break;

        case I::SetCounter:
            _zone->setCounter(instruction.arguments[0]);
        case I::PlaceTile:
        {
            uint16_t tileID = instruction.arguments[3];
            Tile *t = tileID < _engine->data->_tiles.size() ? _engine->data->_tiles.at(tileID) : NULL;
            _zone->setTile(t, instruction.arguments[0], instruction.arguments[1], instruction.arguments[2]);
            break;
        }
        case I::RemoveTile:
            _zone->setTile(NULL, instruction.arguments[0], instruction.arguments[1], instruction.arguments[2]);
            break;
        case I::MoveTile:
        {
            Tile *t = _zone->getTile(instruction.arguments[0], instruction.arguments[1], instruction.arguments[2]);
            _zone->setTile(NULL, instruction.arguments[0], instruction.arguments[1], instruction.arguments[2]);
            _zone->setTile(t, instruction.arguments[3], instruction.arguments[4], instruction.arguments[2]);
            break;
        }
        case I::SetHero:
            break;
        case I::ShowHero:
            _engine->world->hero.visible = true;
            break;
        case I::HideHero:
            _engine->world->hero.visible = false;
            break;
        case I::SetRectNeedsDisplay:
        case I::SetTileNeedsDisplay: // NOP
            break;
        case I::Redraw:
            return true;
            break;
        case I::ChangeZone:
        {
            ZoneTransitionScene *scene = new ZoneTransitionScene(ZoneTransitionScene::RoomTransition, this);

            unsigned short zoneX, zoneY;

            scene->oldZone = _zone;
            scene->newZone = _engine->world->getZone(instruction.arguments[0], &zoneX, &zoneY);
            scene->newZone->enteredByPlane = true;

            scene->heroStart = _engine->world->hero.location;
            scene->heroEnd   = (GamePoint){.x = instruction.arguments[1], .y = instruction.arguments[2],.l = 1 };

            scene->zoneStart = _engine->world->zoneLocation;
            scene->zoneEnd   = (GamePoint){.x = zoneX, .y=zoneY, .l=0};

            _engine->world->hero.location = (GamePoint){.x = instruction.arguments[1], .y = instruction.arguments[2],.l = 1 };

            _engine->pushScene(scene);

            _action = NULL;
            instructionPointer = 0;
            break;
        }
        case I::PlaySound:
        {
            Sound *sound = _engine->data->_sounds.at(instruction.arguments[0]);
            _engine->mixer->play(sound, SoundMixer::SoundEffect);
            break;
        }
        case I::StopSound:

            break;
        case I::AddItem:
            _engine->world->hero.addItem(_engine->data->_tiles.at(instruction.arguments[0]));
            if(_engine->hostCallbacks.updateInventory)
                _engine->hostCallbacks.updateInventory(_engine->hostCallbacks.host);
            break;
        case I::RemoveItem:
            _engine->world->hero.removeItem(_engine->data->_tiles.at(instruction.arguments[0]));
            if(_engine->hostCallbacks.updateInventory)
                _engine->hostCallbacks.updateInventory(_engine->hostCallbacks.host);
            break;
        case I::RollDice:
            _random = random() % instruction.arguments[0];
            break;
        case I::SetRandom:
            _random = instruction.arguments[0];
            break;

        default:
       //     printf("unknown action: %lx\n", instruction.opcode);
            break;
    }

    return false;
}

/*
 int16_t *arguments = instruction.arguments;
 case Action::Instruction::Opcode::SpeakHero:
 f(@"speakHero(%d, %d)",arguments[0],arguments[1])
 case Action::Instruction::Opcode::SpeakNPC:
 f(@"speakNPC(x:%d, y:%d)", arguments[0],arguments[1])
 case Action::Instruction::Opcode::SetHero:

 f(@"setHer(x:%d, y:%d)", arguments[0], arguments[1])
 case Action::Instruction::Opcode::DropItem:
 f(@"dropItem(%@ (0x%0x), x:%d, y:%d)", [self _nameOfTile:arguments[0]], arguments[0], arguments[1], arguments[2])
 case Action::Instruction::Opcode::AddItem:
 f(@"addItem(%@ (0x%0x))", [self _nameOfTile:arguments[0]], arguments[0])
 case Action::Instruction::Opcode::RemoveItem:
 f(@"removeItem(%@ (0x%0x))", [self _nameOfTile:arguments[0]], arguments[0])

 case Action::Instruction::Opcode::ChangeZone:
 f(@"changeZone(0x%03x, x:%d, y:%d)", arguments[0], arguments[1], arguments[2])
 #undef f
*/