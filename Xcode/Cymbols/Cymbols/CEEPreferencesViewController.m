//
//  CEEPreferencesViewController.m
//  Cymbols
//
//  Created by Yu Qing Cai on 2021/4/6.
//  Copyright © 2021 Lazycatdesign. All rights reserved.
//

#import "CEEPreferencesViewController.h"
#import "CEEStyleManager.h"
#import "CEEComboBoxTableCellView.h"
#import "CEETextFieldTableCellView.h"
#import "CEEJSONReader.h"
#import "AppDelegate.h"

@interface CEEPreferencesViewController ()
@property (weak) IBOutlet CEETextField *filterInput;
@property (weak) IBOutlet CEETableView *configurationTable;
@property (strong) NSMutableArray* configurations;
@property (strong) NSMutableArray* configurationsFiltered;
@property (strong) NSString* filter;
@property BOOL activeConfigurationSetting;
@end

@implementation CEEPreferencesViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [_configurationTable setNibNameOfCellView:@"TableCellViews"];
    [_configurationTable setDataSource:self];
    [_configurationTable setDelegate:self];
    [_configurationTable setNumberOfColumns:1];
    [_configurationTable setAllowsMultipleSelection:NO];
    [_configurationTable setEnableDrawHeader:NO];
    [_configurationTable setColumnAutoresizingStyle:kCEETableViewUniformColumnAutoresizingStyle];
    [_configurationTable setSelectable:NO];
    [_filterInput setDelegate:self];
    _activeConfigurationSetting = YES;
}

- (void)viewDidAppear {
    [super viewDidAppear];
    [self.view setStyleState:kCEEViewStyleStateActived];
    
    [self laodConfigurationTemplate];
    
    _configurationsFiltered = _configurations;
    [_configurationTable reloadData];
}

- (void)laodConfigurationTemplate {
    AppDelegate* delegate = [NSApp delegate];
    CEEStyleManager* styleManager = [CEEStyleManager defaultStyleManager];
    NSString* filePath = [delegate propertyIndex:CEEConfigurationTemplateFilePathIndexer];
    NSMutableDictionary* dict = [CEEJSONReader objectFromFile:filePath];
    NSArray* keys = [dict.allKeys sortedArrayUsingComparator:^(id a, id b) {
        return [a compare:b options:NSLiteralSearch];
    }];
    
    // enumerate UIStyles To Template
    NSArray* UIStyleNames = [styleManager userInterfaceStyleNames];
    if (UIStyleNames && dict[CEEApplicationConfigurationNameUIStyle])
        [dict[CEEApplicationConfigurationNameUIStyle] setValue:UIStyleNames forKey:@"values"];
    // enumerate TextHighlightStyles To Template
    NSArray* textHighlightStyleNames = [styleManager textHighlightStyleNames];
    if (textHighlightStyleNames && dict[CEEApplicationConfigurationNameTextHighlightStyle])
        [dict[CEEApplicationConfigurationNameTextHighlightStyle] setValue:textHighlightStyleNames forKey:@"values"];
    
    NSMutableDictionary* dictSorted = [[NSMutableDictionary alloc] init];
    for (NSString* key in keys)
        [dictSorted setValue:dict[key] forKey:key];
    
    NSDictionary* configuration = [delegate configuration];
    
    // instance configuration template
    for (NSString* key in configuration.allKeys) {
        NSString* value = configuration[key];
        if (dictSorted[key]) {
            dict = dictSorted[key];
            [dict setValue:value forKey:@"value"];
        }
    }
    
    // create configuration array
    _configurations = [[NSMutableArray alloc] init];
    for (NSInteger i = 0; i < dictSorted.count; i ++) {
        NSString* key = dictSorted.allKeys[i];
        NSMutableDictionary* dict = dictSorted[key];
        NSMutableDictionary* entry = [[NSMutableDictionary alloc] init];
        [entry setValue:key forKey:@"configuration_key"];
        [entry setValue:dict forKey:@"configuration_object"];
        [_configurations addObject:entry];
    }
}

- (NSInteger)numberOfRowsInTableView:(CEETableView *)tableView {
    if (_configurationsFiltered && _configurationsFiltered.count)
        return _configurationsFiltered.count;
    return 0;
}

- (CEEView *)tableView:(CEETableView *)tableView viewForColumn:(NSInteger)column row:(NSInteger)row {
    _activeConfigurationSetting = NO;
    NSDictionary* configurationEntry = _configurationsFiltered[row];
    CEEView* cellView = nil;
    NSDictionary* object = configurationEntry[@"configuration_object"];
    if (object[@"values"]) {
        CEEComboBoxTableCellView* comboBoxTableCellView = [_configurationTable makeViewWithIdentifier:@"IDComboBoxTableCellView"];
        comboBoxTableCellView.text.stringValue = object[@"title"];
        CEEComboBox* comboBox = comboBoxTableCellView.comboBox;
        
        comboBox.tag = row;
        comboBox.delegate = self;
        [comboBox setNibNameOfCellView:@"TableCellViews"];
        [comboBox removeAllItems];
        
        NSArray* values = object[@"values"];
        NSString* value = object[@"value"];
        for (NSInteger i = 0; i < values.count; i ++)
            [comboBox addItem:values[i]];
        
        for (NSInteger i = 0; i < values.count; i ++) {
            if ([value compare:values[i] options:NSCaseInsensitiveSearch] == NSOrderedSame) {
                [comboBox setIndexOfSelectedItem:i];
                break;
            }
        }
        
        cellView = comboBoxTableCellView;
    }
    else {
        CEETextFieldTableCellView* textFieldTableCellView = [_configurationTable makeViewWithIdentifier:@"IDTextFieldTableCellView"];
        textFieldTableCellView.text.stringValue = object[@"title"];
        CEETextField* textField = textFieldTableCellView.input;
        textField.tag = row;
        textField.delegate = self;
        textField.stringValue = object[@"value"];
        cellView = textFieldTableCellView;
    }
    _activeConfigurationSetting = YES;
    return cellView;
}

- (void)textViewTextChanged:(CEETextView *)textView {
    AppDelegate* delegate = [NSApp delegate];
    if (textView == _filterInput) {
        _filter = _filterInput.stringValue;
        
        if (!_filter) {
            _configurationsFiltered = _configurations;
        }
        else {
            _filter = [_filter stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
            if ([_filter isEqual:@""]) {
                _configurationsFiltered = _configurations;
            }
            else {
                _configurationsFiltered = [[NSMutableArray alloc] init];
                for (NSDictionary* configuration in _configurations) {
                    NSDictionary* configuration_object = configuration[@"configuration_object"];
                    NSString* title = configuration_object[@"title"];
                    if ([title rangeOfString:_filter options:NSCaseInsensitiveSearch].location != NSNotFound)
                        [_configurationsFiltered addObject:configuration];
                }
            }
        }
        [_configurationTable reloadData];
    }
    else {
        if (!_activeConfigurationSetting)
            return;
        
        NSDictionary* configurationEntry = _configurationsFiltered[textView.tag];
        NSDictionary* configuration_object = configurationEntry[@"configuration_object"];
        NSString* value = textView.stringValue;
        [configuration_object setValue:value forKey:@"value"];
        [delegate setConfigurationForKey:configurationEntry[@"configuration_key"] value:value];
    }
}

- (void)selectedIndexChangedOfComboBox:(CEEComboBox *)comboBox {
    if (!_activeConfigurationSetting)
        return;
    
    AppDelegate* delegate = [NSApp delegate];
    NSDictionary* configurationEntry = _configurationsFiltered[comboBox.tag];
    NSDictionary* configuration_object = configurationEntry[@"configuration_object"];
    NSArray* values = configuration_object[@"values"];
    NSString* value = values[comboBox.indexOfSelectedItem];
    [configuration_object setValue:value forKey:@"value"];
    [delegate setConfigurationForKey:configurationEntry[@"configuration_key"] value:value];
}

@end
