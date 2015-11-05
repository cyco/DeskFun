//
//  ActionWindowController.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "ActionWindowController.h"

#import "AppDelegate.h"
#import "Packing.h"

#import "NSNumber+StringReading.h"

@interface ActionWindowController () <NSTableViewDataSource, NSTableViewDelegate>
{
    Action   *_action;
    GameDataParser *_GameDataParser;
}
@end

NSString * const ActionDragType = @"com.deskfun.action";

@implementation ActionWindowController

- (void)windowDidLoad {
    [super windowDidLoad];

    [[self instructions] registerForDraggedTypes:@[ActionDragType]];
}

- (NSString*)windowNibName
{
    return @"ActionWindowController";
}

- (void)setGameDataParser:(GameDataParser*)data
{
    _GameDataParser = data;
}

- (void)setAction:(Action*)action
{
    _action = action;
    if(_action == NULL)
    {
        [[self window] orderOut:nil];
        return;
    }

    [[self instructions] reloadData];
    [self _clearDetails];
    [[self window] orderFront:nil];
}
#pragma mark -
- (IBAction)removeItem:(id)sender
{
    NSInteger row = [[self instructions] selectedRow];
    if(row == -1) return;

    row -= 1;

    if(row < _action->conditions.size()) {
        _action->conditions.erase(_action->conditions.begin()+row);
    } else if(row > _action->conditions.size()) {
        row -= _action->conditions.size()+1;
        _action->instructions.erase(_action->instructions.begin()+row);
    }

    [[self instructions] reloadData];
}

- (IBAction)addItem:(id)sender
{
    NSInteger row = [[self instructions] selectedRow];
    if(row == -1) {
        if(([NSEvent modifierFlags] & NSAlternateKeyMask) == 0)
            _action->conditions.insert(_action->conditions.end(), Action::Instruction());
        else
        _action->instructions.insert(_action->instructions.end(), Action::Instruction());
        [[self instructions] reloadData];
        return;
    }

    NSInteger conditionIdx   = [self conditionForRow:row];
    NSInteger instructionIdx = [self instructionForRow:row];

    if(conditionIdx >= 0 && conditionIdx <= _action->conditions.size())
    {
        _action->conditions.insert(_action->conditions.begin()+conditionIdx, Action::Instruction());
        [[self instructions] reloadData];
        return;
    }

    if(instructionIdx >= 0 && instructionIdx <= _action->instructions.size())
    {
        _action->instructions.insert(_action->instructions.begin()+instructionIdx, Action::Instruction());
        [[self instructions] reloadData];
        return;
    }

    _action->instructions.insert(_action->instructions.end(), Action::Instruction());
    [[self instructions] reloadData];
}


- (NSInteger)rowForCondition:(NSInteger)conditionIdx
{
    return 1 + conditionIdx;
}

- (NSInteger)rowForInstruction:(NSInteger)instructionIdx
{
    return _action->conditions.size() + 2 + instructionIdx;
}

- (NSInteger)conditionForRow:(NSInteger)row
{
    return row-1;
}

- (NSInteger)instructionForRow:(NSInteger)row
{
    return row - 1 - _action->conditions.size() - 1;
}

- (IBAction)saveChanges:(id)sender
{
    NSInteger row = [[self instructions] selectedRow];
    if(row == -1) return;

    uint16_t arg1, arg2, arg3, arg4, arg5;
    NSString *additionalText = [self.additionalDataView string];
    uint16_t additionalDataLenth = [additionalText length];

    Action::Instruction *i;
    if(row < _action->conditions.size()+1) {
        i = &_action->conditions[row-1];
    } else if(row > _action->conditions.size()) {
        row -= _action->conditions.size()+2;
        i = &_action->instructions[row];
    }


    arg1 = [[NSNumber parseHexString:self.arg1.stringValue] intValue];
    arg2 = [[NSNumber parseHexString:self.arg2.stringValue] intValue];
    arg3 = [[NSNumber parseHexString:self.arg3.stringValue] intValue];
    arg4 = [[NSNumber parseHexString:self.arg4.stringValue] intValue];
    arg5 = [[NSNumber parseHexString:self.arg5.stringValue] intValue];

    i->opcode = [self.opcode indexOfSelectedItem];
    i->arguments[0] = arg1;
    i->arguments[1] = arg2;
    i->arguments[2] = arg3;
    i->arguments[3] = arg4;
    i->arguments[4] = arg5;

    i->additionalDataLength = additionalDataLenth;

    free(i->additionalData);
    i->additionalData = (char*)malloc(additionalDataLenth);
    memcpy(i->additionalData, [[additionalText dataUsingEncoding:NSISOLatin1StringEncoding] bytes], additionalDataLenth);

    [[self instructions] reloadDataForRowIndexes:[NSIndexSet indexSetWithIndex:[[self instructions] selectedRow]] columnIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [[[self instructions] tableColumns] count])]];
}

- (IBAction)resetChanges:(id)sender
{
    [self tableViewSelectionDidChange:[NSNotification notificationWithName:NSTableViewSelectionDidChangeNotification object:[self instructions]]];
}

#pragma mark -
- (void)keyDown:(NSEvent*)event
{
    if([event keyCode] == 51)
    {
        NSIndexSet *indexSet = [[self instructions] selectedRowIndexes];
        [self removeItem:nil];
        [[self instructions] selectRowIndexes:indexSet byExtendingSelection:NO];
    }
}

#pragma mark -
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    return 2 + (_action ? _action->instructions.size() + _action->conditions.size() : 0);
}

- (BOOL)tableView:(NSTableView *)tableView isGroupRow:(NSInteger)row
{
    return row == 0 || row == 1 + (_action ? _action->conditions.size() : 0);
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSTableRowView *view;
    if([self tableView:tableView isGroupRow:row])
    {
        view = [tableView makeViewWithIdentifier:@"GroupCell" owner:self];

        NSString *title = row == 0 ? @"Conditions" : @"Instructions";
        [[[view subviews] objectAtIndex:0] setStringValue:title];
    }
    else
    {
        NSString *label = @"";
        Action::Instruction instruction;
        if(row <= _action->conditions.size())
        {
            instruction = _action->conditions[row-1];
            label = [self descriptionForCondition:instruction];
        }
        else
        {
            instruction = _action->instructions[row - _action->conditions.size() -2];
            label = [self descriptionForInstruction:instruction];
        }


        view = [tableView makeViewWithIdentifier:@"InstructionCell" owner:self];
        NSTextField *field = [view viewWithTag:1];
        [field setStringValue:label];
    }

    return view;
}

- (NSString*)descriptionForCondition:(Action::Instruction)condition
{
    uint16_t *arguments = condition.arguments;
    switch (condition.opcode) {
#define f(_S_, ...) return [NSString stringWithFormat:_S_, ##__VA_ARGS__];
        case Action::Instruction::ConditionCode::ZoneNotInitalized:
            f(@"Zone not initialized yet");
        case Action::Instruction::ConditionCode::ZoneEntered:
            f(@"Zone Entered");
        case Action::Instruction::ConditionCode::EnterByPlane:
            f(@"Zone entered by plane");
        case Action::Instruction::ConditionCode::Bump:
            f(@"Bump into %@ (0x%0x) at %dx%d", [self _nameOfTile:arguments[2]], arguments[2], arguments[0], arguments[1]);
        case Action::Instruction::ConditionCode::StandingOn:
            f(@"Hero standing on %@ (0x%0x) at %dx%d", [self _nameOfTile:arguments[2]], arguments[2], arguments[0], arguments[1]);
        case Action::Instruction::ConditionCode::PlaceItem:
            f(@"Placed %@ (0x%0x) on %@ (0x%0x) at %dx%dx%d", [self _nameOfTile:arguments[4]], arguments[4], [self _nameOfTile:arguments[3]], arguments[3], arguments[0], arguments[1], arguments[2]);
        case Action::Instruction::ConditionCode::HasItem:
            f(@"Inventory contains %@ (0x%0x)", [self _nameOfTile:arguments[0]], arguments[0]);
        case Action::Instruction::ConditionCode::CounterIs:
            f(@"Counter is set to %d", arguments[0]);
        case Action::Instruction::ConditionCode::CounterIsNot:
            f(@"Counter is _not_ set to %d", arguments[0]);
        case Action::Instruction::ConditionCode::RequiredItem:
            f(@"Required item is %@ (%0x)", [self _nameOfTile:arguments[0]], arguments[0]);
        case Action::Instruction::ConditionCode::Ending:
            f(@"Ending is %@ (0x%0x)", [self _nameOfTile:arguments[0]], arguments[0]);
        case Action::Instruction::ConditionCode::RandomIs:
            f(@"Random is %d", arguments[0]);
        case Action::Instruction::ConditionCode::RandomIsNot:
            f(@"Random is _not_ %d", arguments[0]);
        case Action::Instruction::ConditionCode::PlaceItemIsNot:
            f(@"Place other than %@ (0x%0x) on %@ (0x%0x) at %dx%dx%d", [self _nameOfTile:arguments[4]], arguments[4], [self _nameOfTile:arguments[3]], arguments[3], arguments[0], arguments[1], arguments[2]);

        case Action::Instruction::ConditionCode::HeroIsAt:
            f(@"Hero is at %0dx%d", arguments[0], arguments[1]);
            
        case Action::Instruction::ConditionCode::HealthIsLessThan:
            f(@"Health < %d", arguments[0]);
        case Action::Instruction::ConditionCode::HealthIsMoreThan:
            f(@"Health > %d", arguments[0]);

        case Action::Instruction::ConditionCode::GamesWonBiggerThan:
            f(@"GamesWon > %d", arguments[0]);
        case Action::Instruction::ConditionCode::GamesWonIsExactly:
            f(@"GamesWon == %d", arguments[0]);

        case Action::Instruction::ConditionCode::TileAtIs:
            f(@"Tile at (3) %0dx%0dx%0d is %@ (0x%0x)", arguments[1], arguments[2], arguments[0], [self _nameOfTile:arguments[0]], arguments[0])
        case Action::Instruction::ConditionCode::TileAt:
            f(@"Tile at (2) %0dx%0dx%0d is %@ (0x%0x)", arguments[1], arguments[2], arguments[3], [self _nameOfTile:arguments[0]], arguments[0]);
        case Action::Instruction::ConditionCode::TileIs:
            f(@"Tile at (1) %0dx%0dx%0d is %@ (0x%0x)", arguments[1], arguments[2], arguments[3], [self _nameOfTile:arguments[0]], arguments[0]);

#undef f
        default: return [self genericDescription:condition];
    }
}

- (NSString*)descriptionForInstruction:(Action::Instruction)instruction
{
#define f(_S_, ...) return [NSString stringWithFormat:_S_, ##__VA_ARGS__];
    uint16_t *arguments = instruction.arguments;
    switch (instruction.opcode) {
        case Action::Instruction::Opcode::PlaceTile:
            f(@"placeTile(x:%d, y:%d, l:%d, 0x%04x)", arguments[0], arguments[1], arguments[2], arguments[3])
        case Action::Instruction::Opcode::RemoveTile:
            f(@"removeTile(x:%d, y:%d, l:%d)", arguments[0], arguments[1], arguments[2])
        case Action::Instruction::Opcode::MoveTile:
            f(@"moveTile(x:%d, y:%d, l:%d, toX:%d, toY:%d)", arguments[0], arguments[1], arguments[2], arguments[3], arguments[4])
        case Action::Instruction::Opcode::RollDice:
            f(@"rollDice(%d, %d)", arguments[0], arguments[1]);
        case Action::Instruction::Opcode::SetRandom:
            f(@"setRandom(%d)", arguments[0]);
        case Action::Instruction::Opcode::SpeakHero:
            f(@"speakHero(%d, %d)",arguments[0],arguments[1])
        case Action::Instruction::Opcode::DrawTile:
            f(@"drawTile(x: %d, y: %d, 0x%04x)", arguments[0], arguments[1], arguments[2]);
        case Action::Instruction::Opcode::SpeakNPC:
            f(@"speakNPC(x:%d, y:%d)", arguments[0],arguments[1]);
        case Action::Instruction::Opcode::SetTileNeedsDisplay:
            f(@"tileNeedsDisplay(x:%d, y:%d)", arguments[0], arguments[1]);
        case Action::Instruction::Opcode::ShowHero:
            f(@"showHero()");
        case Action::Instruction::Opcode::HideHero:
            f(@"hideHero()");
        case Action::Instruction::Opcode::Wait:
            f(@"Wait()");
        case Action::Instruction::Opcode::Redraw:
            f(@"Redraw()");
        case Action::Instruction::Opcode::SetRectNeedsDisplay:
            f(@"rectNeedsDisplay(minX:%d, minY:%d, maxX:%d, maxY:%d)", arguments[0], arguments[1], arguments[2], arguments[3]);
        case Action::Instruction::Opcode::SetHero:
            f(@"setHer(x:%d, y:%d)", arguments[0], arguments[1])
        case Action::Instruction::Opcode::AddHealth:
            f(@"addHealth(%d, %d)", (int16_t)arguments[0], (int16_t)arguments[1]);
        case Action::Instruction::Opcode::DropItem:
            f(@"dropItem(%@ (0x%0x), x:%d, y:%d)", [self _nameOfTile:arguments[0]], arguments[0], arguments[1], arguments[2])
        case Action::Instruction::Opcode::AddItem:
            f(@"addItem(%@ (0x%0x))", [self _nameOfTile:arguments[0]], arguments[0])
        case Action::Instruction::Opcode::RemoveItem:
            f(@"removeItem(%@ (0x%0x))", [self _nameOfTile:arguments[0]], arguments[0])
        case Action::Instruction::Opcode::PlaySound:
            f(@"playSound(%s)", _GameDataParser->_sounds[arguments[0]]->getFileName().c_str())
        case Action::Instruction::Opcode::SetCounter:
            f(@"setCounter(value: %d)", arguments[0]);
        case Action::Instruction::Opcode::ChangeZone:
            f(@"changeZone(0x%03x, x:%d, y:%d)", arguments[0], arguments[1], arguments[2])
        case Action::Instruction::Opcode::EnableHotspot:
            f(@"enableHotspot(%d)", arguments[0]);
        case Action::Instruction::Opcode::DisableHotspot:
            f(@"disableHotspot(%d)", arguments[0]);
        case Action::Instruction::Opcode::DisableAction:
            f(@"disableAction()");
#undef f
        default: return [self genericDescriptionForInstruction:instruction];
    }
}

- (NSString*)_nameOfTile:(uint16_t)tileID
{
    if(tileID == 0xFFFF) return @"Puzzle Item";
    if(tileID >= _GameDataParser->_tiles.size()) return @"invalid";
    
    return [NSString stringWithFormat:@"%s", _GameDataParser->_tiles[tileID]->name.c_str()];
}

- (NSString*)genericDescriptionForInstruction:(Action::Instruction)i
{
    return [NSString stringWithFormat:@"%02x %s", i.opcode, Action::Instruction::instructionName((Action::Instruction::Opcode)i.opcode).c_str()];
}

- (NSString*)genericDescription:(Action::Instruction)instruction
{
    NSMutableString *label = [NSMutableString stringWithFormat:@"0x%02x", instruction.opcode];
    [label appendString:@"("];
    for(int i=0; i < 5; i++)
    {
        uint16_t arg = instruction.arguments[i];
        unsigned char upper = (arg>>8) & 0xFF;
        unsigned char lower = arg & 0xFF;

        [label appendFormat:@"%c, ", (lower < 0x20 || lower > 0x7E) ? ' ' : lower];

        if(i==4) [label appendFormat:@"%c",   (upper < 0x20 || upper > 0x7E) ? ' ' : upper];
        else     [label appendFormat:@"%c, ", (upper < 0x20 || upper > 0x7E) ? ' ' : upper];
    }
    [label appendString:@")"];
    return label;
}

- (BOOL)tableView:(NSTableView *)tableView shouldSelectRow:(NSInteger)row
{
    return _action && row != 0 && row != _action->conditions.size() + 1;
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
    if([[self instructions] selectedRow] == -1)
    {
        [self _clearDetails];
        return;
    } else {
        NSInteger row = [[self instructions] selectedRow];
        if(row > _action->conditions.size())
        {
            [self.opcode removeAllItems];
            for(uint16_t i=0; i < Action::Instruction::Opcode::OPCODE_COUNT; i++)
            {
                Action::Instruction::Opcode c = (Action::Instruction::Opcode)i;
                NSString *name = [NSString stringWithFormat:@"%02x %s", i, Action::Instruction::instructionName(c).c_str()];
                [self.opcode addItemWithTitle:name];
            }

            [self _showDetailsForInstruction:row-_action->conditions.size()-2];
        } else {
            [self.opcode removeAllItems];
            for(uint16_t i=0; i < Action::Instruction::ConditionCode::CONDITION_CODE_COUNT; i++)
            {
                Action::Instruction::ConditionCode c = (Action::Instruction::ConditionCode)i;
                NSString *name = [NSString stringWithFormat:@"%02x %s", i, Action::Instruction::conditionName(c).c_str()];
                [self.opcode addItemWithTitle:name];
            }

            [self _showDetailsForCondition:row-1];
        }

    }
}

- (void)_clearDetails
{
    [self.opcode removeAllItems];
    [self.opcode setEnabled:FALSE];

    [self.arg1 setStringValue:@""];
    [self.arg2 setStringValue:@""];
    [self.arg3 setStringValue:@""];
    [self.arg4 setStringValue:@""];
    [self.arg5 setStringValue:@""];

    self.arg1.enabled = self.arg2.enabled = self.arg3.enabled = self.arg4.enabled = self.arg5.enabled = FALSE;

    [self.additionalDataView setString:@""];
    [self.additionalDataView setEditable:FALSE];
    [self.additionalDataView setSelectable:FALSE];

    [self.additionalDataLengthField setStringValue:@""];
}

- (void)_showDetailsForInstruction:(uint16_t)i
{
    if(i >= _action->instructions.size())
    {
        printf("out of bounds\n");
        return;
    }

    Action::Instruction instruction = _action->instructions[i];
    [self _showDetailsForUnknown:instruction];
}

- (void)_showDetailsForCondition:(uint16_t)c
{
    if(c >= _action->conditions.size())
    {
        printf("out of bounds\n");
        return;
    }

    Action::Instruction condition = _action->conditions[c];
    [self _showDetailsForUnknown:condition];
}

- (void)_showDetailsForUnknown:(Action::Instruction)i
{
#define ASCII(_V_, _S_) (_V_&0xFF), ((_V_>>8)&0xFF)

    [self.opcode setEnabled:YES];
    [self.opcode selectItemAtIndex:i.opcode];

    self.arg1.enabled = self.arg2.enabled = self.arg3.enabled = self.arg4.enabled = self.arg5.enabled = TRUE;
    [self.arg1 setStringValue:[NSString stringWithFormat:@"%02x", i.arguments[0]]];
    [self.arg2 setStringValue:[NSString stringWithFormat:@"%02x", i.arguments[1]]];
    [self.arg3 setStringValue:[NSString stringWithFormat:@"%02x", i.arguments[2]]];
    [self.arg4 setStringValue:[NSString stringWithFormat:@"%02x", i.arguments[3]]];
    [self.arg5 setStringValue:[NSString stringWithFormat:@"%02x", i.arguments[4]]];


    [self _setAdditionalText:i];
}

- (void)_setAdditionalText:(Action::Instruction)i
{
    [self.additionalDataLengthField setStringValue:[NSString stringWithFormat:@"0x%04x", i.additionalDataLength]];
    NSMutableString *additionalText = [NSMutableString stringWithCapacity:i.additionalDataLength];
    for(int j=0; j < i.additionalDataLength; j++)
    {
        [additionalText appendFormat:@"%c", i.additionalData[j]];
    }
    [self.additionalDataView setString:additionalText];
    [self.additionalDataView setSelectable:TRUE];
    [self.additionalDataView setEditable:TRUE];
}

#pragma mark -
- (BOOL)tableView:(NSTableView *)tableView writeRowsWithIndexes:(NSIndexSet *)rowIndexes toPasteboard:(NSPasteboard*)pboard {
    [pboard declareTypes:@[ActionDragType] owner:self];
    return YES;
}

- (NSDragOperation)tableView:(NSTableView*)tableView validateDrop:(id)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)op
{
    if(op == NSTableViewDropOn || row < 1 )
        return NSDragOperationNone;

    return NSDragOperationMove;
}

- (BOOL)tableView:(NSTableView *)aTableView acceptDrop:(id)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)op {

    if(op == NSTableViewDropOn || row < 1)
        return NO;

    NSLog(@"Drop at row: %ld", row);

    return YES;
}
@end
