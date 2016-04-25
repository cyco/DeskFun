//
//  ZoneWindowController.m
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#import "ZoneWindowController.h"

#import "GameDataParser.hpp"
#import "AppDelegate.h"

#import "ZoneView.h"

@interface ZoneWindowController () <NSTableViewDataSource, NSTableViewDelegate>
{
    GameDataParser *data;
    Zone *currentZone;
    std::vector<Zone*> zones;

    NSUInteger planetFilter;
    NSUInteger puzzleFilter;
}
@end

@implementation ZoneWindowController

- (NSString*)windowNibName
{
    return @"ZoneWindowController";
}

- (void)windowDidLoad {
    [super windowDidLoad];

    currentZone = NULL;

    AppDelegate *dele = (AppDelegate*)[NSApp delegate];
    data = [dele GameDataParser];

    [[self actionController] setGameDataParser:data];

    [self _setupPuzzleFilter];

    [[self tableView] setDataSource:self];
    [[self tableView] setDelegate:self];

    [[self actionsView] setDataSource:self];
    [[self actionsView] setDelegate:self];
    [[self actionsView] reloadData];
    
    [[self itemsTableView1] setDataSource:self];
    [[self itemsTableView1] setDelegate:self];
    [[self itemsTableView1] reloadData];
    
    [[self itemsTableView2] setDataSource:self];
    [[self itemsTableView2] setDelegate:self];
    [[self itemsTableView2] reloadData];
    
    [[self itemsTableView3] setDataSource:self];
    [[self itemsTableView3] setDelegate:self];
    [[self itemsTableView3] reloadData];
    
    [[self itemsTableView4] setDataSource:self];
    [[self itemsTableView4] setDelegate:self];
    [[self itemsTableView4] reloadData];

    planetFilter = 0, puzzleFilter = 0;
    [self filterZones];
}
#pragma mark -
- (IBAction)addAction:(id)sender
{
    NSInteger row = [[self tableView] selectedRow];
    if(row == -1 || currentZone == NULL) return;

    currentZone->_actions.insert(currentZone->_actions.begin()+row, new Action());
    [[self actionsView] reloadData];
    [[self itemsTableView1] reloadData];
    [[self itemsTableView2] reloadData];
    [[self itemsTableView3] reloadData];
    [[self itemsTableView4] reloadData];
}

- (IBAction)removeAction:(id)sender
{
    NSInteger row = [[self actionsView] selectedRow];
    if(row == -1 || currentZone == NULL) return;

    currentZone->_actions.erase(currentZone->_actions.begin() + row);
    [[self actionsView] reloadData];
    [[self itemsTableView1] reloadData];
    [[self itemsTableView2] reloadData];
    [[self itemsTableView3] reloadData];
    [[self itemsTableView4] reloadData];
}

#pragma mark - Stuff
- (void)_setupPuzzleFilter
{
    [[self puzzleFilterButton] removeAllItems];
    [[self puzzleFilterButton] addItemsWithTitles:@[@"Any",
                                                    @"Empty",
                                                    @"Blockade North",
                                                    @"Blockade South",
                                                    @"Blockade East",
                                                    @"Blockade West",
                                                    @"Travel Start",
                                                    @"Travel End",
                                                    @"Room",
                                                    @"Load",
                                                    @"Goal",
                                                    @"Town",
                                                    @"Unused",
                                                    @"Win",
                                                    @"Lose",
                                                    @"Trade",
                                                    @"Use",
                                                    @"Find",
                                                    @"FindTheForce"]];
}

- (IBAction)planetTypeFilterChanged:(id)sender
{
    planetFilter = [sender selectedSegment];
    [self filterZones];
}

- (IBAction)puzzleFilterChanged:(id)sender
{
    puzzleFilter = [sender indexOfSelectedItem];
    [self filterZones];
}

- (void)filterZones
{
    zones.clear();

    copy(data->_zones.begin(), data->_zones.end(), std::back_inserter(zones));

    if(planetFilter != 0)
    {
        PlanetType planet;
        switch (planetFilter) {
            case 1: planet = PlanetTypeDagobah; break;
            case 2: planet = PlanetTypeHoth; break;
            case 3: planet = PlanetTypeTatooine; break;
            case 4: planet = PlanetTypeEndor; break;
            default: planet = PlanetTypeNone; break;
        }
        zones = [self _filterZones:zones byPlanet:planet];
    }

    if(puzzleFilter != 0)
    {
        Zone::Type puzzleZype = (Zone::Type)puzzleFilter;
        zones = [self _filterZones:zones byPuzzle:puzzleZype];
    }

    [[self tableView] reloadData];

}

- (std::vector<Zone*>)_filterZones:(std::vector<Zone*>) zonesToFilter byPuzzle:(Zone::Type)p
{
    std::vector<Zone*> result;
    std::copy_if(zonesToFilter.begin(), zonesToFilter.end(), std::back_inserter(result),[p](Zone *z) {
        return z->getType() == p;
    });
    return result;
}

- (std::vector<Zone*>)_filterZones:(std::vector<Zone*>) zonesToFilter byPlanet:(PlanetType)p {
    std::vector<Zone*> result;
    std::copy_if(zonesToFilter.begin(), zonesToFilter.end(), std::back_inserter(result),[p](Zone *z) {
        return z->getPlanet() == p;
    });
    return result;
}

#pragma mark -
- (IBAction)switchActionTextView:(id)sender
{
    [self _updateActionPanel];
}

- (void)_updateActionPanel
{
    NSInteger selectedAction = [[self actionsView] selectedRow];

    if(selectedAction == -1)
    {
        [[self actionController] setAction:NULL];
        return;
    }

    if(currentZone)
    {
        Action *currentAction = currentZone->_actions.at(selectedAction);
        [[self actionController] setAction:currentAction];
    }
}

#pragma mark - Table View DataSource
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    if(tableView == self.tableView)
        return zones.size();
    
    if(currentZone != NULL){
        if(tableView == self.itemsTableView1) {
            return currentZone->providedItemIDs.size();
        } else if(tableView == self.itemsTableView2) {
            return currentZone->requiredItemIDs.size();
        } else if(tableView == self.itemsTableView3) {
            return currentZone->puzzleNPCTileIDs.size();
        } else if(tableView == self.itemsTableView4) {
            return currentZone->assignedItemIDs.size();
        }
        
        return currentZone->_actions.size();
    }

    return 0;
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    if(tableView == self.tableView)
    {
        NSTableCellView *cellView = [tableView makeViewWithIdentifier:@"zone_cell" owner:self];

        TileView    *view = [[cellView subviews] objectAtIndex:0];
        NSTextField *nameLabel       = [[cellView subviews] objectAtIndex:1];
        NSTextField *additionalLabel = [[cellView subviews] objectAtIndex:2];
        [additionalLabel setStringValue:@""];

        Zone *z = zones.at(row);

        Sizei size = z->getSize();

        NSString *planetName = nil;
        switch(z->getPlanet())
        {
            case PlanetTypeDagobah:
                planetName = @"Dagobah";
                break;
            case PlanetTypeEndor:
                planetName = @"Endor";
                break;
            case PlanetTypeHoth:
                planetName = @"Hoth";
                break;
            case PlanetTypeTatooine:
                planetName = @"Tatooine";
                break;
            case PlanetTypeNone:
                planetName = @"";
                break;
            default:
                break;
        }

        NSString *Type = nil;
        int mapIcon = -1;
        BOOL isYoda = data->_isYodaVersion;
        switch(z->getType())
        {
            case Zone::TypeBlockadeEast:
                Type = @"Blockade E";
                mapIcon =  isYoda ? 827 : 369;
                break;
            case Zone::TypeBlockadeNorth:
                Type = @"Blockade N";
                mapIcon = isYoda ? 821 : 367;
                break;
            case Zone::TypeBlockadeSouth:
                Type = @"Blockade S";
                mapIcon = isYoda ? 825 : 371;
                break;
            case Zone::TypeBlockadeWest:
                Type = @"Blockade W";
                mapIcon = isYoda ? 823 : 373;
                break;
            case Zone::TypeFind:
                Type = @"Find Something";
                mapIcon = isYoda ? 817 : 363;
                break;
            case Zone::TypeFindTheForce:
                Type = @"Find the Force";
                mapIcon = isYoda ? 817 : 363;
                break;
            case Zone::TypeGoal:
                Type = @"Goal";
                mapIcon = isYoda ? 831 : 376;
                break;
            case Zone::TypeEmpty:
                Type = @"Empty";
                mapIcon = isYoda ? 832 : 377;
                break;
            case Zone::TypeLoad:
                Type = @"Load";
                mapIcon = isYoda ? 835 : 377;
                break;
            case Zone::TypeLose:
                Type = @"Lose";
                mapIcon = isYoda ? 835 : 377;
                break;
            case Zone::TypeRoom:
                Type = @"Room";
                mapIcon = isYoda ? 835 : 377;
                break;
            case Zone::TypeTown:
                Type = @"Spaceport";
                mapIcon = isYoda ? 829 : 375;
                break;
            case Zone::TypeTrade:
                Type = @"Trade";
                mapIcon = isYoda ? 817 : 363;
                break;
            case Zone::TypeTravelEnd:
                Type = @"Travel End";
                mapIcon = isYoda ? 819 : 365;
                break;
            case Zone::TypeTravelStart:
                Type = @"Travel Start";
                mapIcon = isYoda ? 819 : 365;
                break;
            case Zone::TypeUse:
                Type = @"Use";
                mapIcon = isYoda ? 817 : 363;
                break;
            case Zone::TypeWin:
                Type = @"Win";
                mapIcon = isYoda ? 835 : 377;
                break;
        }

        [view setTileID:mapIcon];

        [nameLabel setStringValue: isYoda ? [planetName stringByAppendingFormat:@", %@", Type] :
         [NSString stringWithFormat:@"%s, %@", z->getName().c_str(), Type]];

        NSString *sizeString = [NSString stringWithFormat:@"%02dx%02d", size.width, size.height];
        NSString *idString   = [NSString stringWithFormat:@"0x%03lx", z->index];
        
        
        NSMutableString *items = [NSMutableString stringWithFormat:@", "];
        [items appendFormat:@"%lu %lu %lu %lu", z->providedItemIDs.size(), z->requiredItemIDs.size(), z->puzzleNPCTileIDs.size(), z->assignedItemIDs.size()];
            
        
        [additionalLabel setStringValue:[NSString stringWithFormat:@"%@, %@%@", idString, sizeString, items]];

        return cellView;
    }
    else if(tableView == self.actionsView)
    {
        NSTableCellView *cellView = [tableView makeViewWithIdentifier:@"action_cell" owner:self];
        NSTextField *label = [[cellView subviews] objectAtIndex:0];

        Action *action = currentZone->_actions[row];
        NSString *actionName = nil;
        if(action->getName().length())
            actionName = [NSString stringWithFormat:@"%s", action->getName().c_str()];
        else
            actionName = [NSString stringWithFormat:@"Action %ld", row];

        [label setStringValue:actionName];
        return cellView;
    } else {
        int p = 0;
        if(tableView == self.itemsTableView1) p = 1;
        if(tableView == self.itemsTableView2) p = 2;
        if(tableView == self.itemsTableView3) p = 3;
        if(tableView == self.itemsTableView4) p = 4;
        
        NSTableCellView *cellView = [tableView makeViewWithIdentifier:@"item_cell" owner:self];

        TileView    *view = [[cellView subviews] objectAtIndex:0];
        
        NSInteger tileID = [self itemIdForRow:row inPuzzle:p];
        if(tileID < 0) tileID = -1;
        if(tileID > data->_tiles.size()) tileID = -1;
        view.tileID = tileID;

        return cellView;
    }

    return nil;
}

- (NSInteger)itemIdForRow:(NSInteger)row inPuzzle:(int)p{
    switch (p) {
        case 1: return currentZone->providedItemIDs[row];
        case 2: return currentZone->requiredItemIDs[row];
        case 3: return currentZone->puzzleNPCTileIDs[row];
        case 4: return currentZone->assignedItemIDs[row];
    }
    return 0;
}

#pragma mark - TableView Delegate
- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
    NSTableView *view     = [notification object];
    NSInteger selectedRow = [view selectedRow];

    if(view == self.tableView)
    {
        if(selectedRow == -1 || selectedRow == -1)
        {
            [[self zoneView] clearZone];
            currentZone = NULL;
        }
        else
        {
            currentZone = zones.at(selectedRow);
            [[self zoneView] setZoneID:currentZone->index];
        }
        
        [[self actionsView] reloadData];
        [[self itemsTableView1] reloadData];
        [[self itemsTableView2] reloadData];
        [[self itemsTableView3] reloadData];
        [[self itemsTableView4] reloadData];
        [self _updateActionPanel];

    } else if(view == self.actionsView){
        [self _updateActionPanel];
    } else if(view == self.itemsTableView1) {
        if(selectedRow == -1 || selectedRow == -1)
            return;
        
        NSInteger tileID = [self itemIdForRow:selectedRow inPuzzle:1];
        Tile *item = data->getTile((int)tileID);
        printf("Tile: %s\n", item->name.c_str());
    } else if(view == self.itemsTableView2) {
        if(selectedRow == -1 || selectedRow == -1)
            return;
        
        NSInteger tileID = [self itemIdForRow:selectedRow inPuzzle:2];
        Tile *item = data->getTile((int)tileID);
        printf("Tile: %s\n", item->name.c_str());
    } else if(view == self.itemsTableView3) {
        if(selectedRow == -1 || selectedRow == -1)
            return;
        
        NSInteger tileID = [self itemIdForRow:selectedRow inPuzzle:3];
        Tile *item = data->getTile((int)tileID);
        printf("Tile: %s\n", item->name.c_str());
    } else if(view == self.itemsTableView4) {
        if(selectedRow == -1 || selectedRow == -1)
            return;
        
        NSInteger tileID = [self itemIdForRow:selectedRow inPuzzle:4];
        Tile *item = data->getTile((int)tileID);
        printf("Tile: %s\n", item->name.c_str());
    }
}

@end
