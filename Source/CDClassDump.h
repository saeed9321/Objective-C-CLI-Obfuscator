// -*- mode: ObjC -*-

//  This file is part of class-dump, a utility for examining the Objective-C segment of Mach-O files.
//  Copyright (C) 1997-1998, 2000-2001, 2004-2015 Steve Nygard.

#import "CDFile.h" // For CDArch

#define CLASS_DUMP_VERSION "1.0.0"

@class CDFile;
@class CDTypeController;
@class CDVisitor;
@class CDSearchPathState;

@interface CDClassDump : NSObject

@property (readonly) CDSearchPathState *searchPathState;

@property (assign) BOOL shouldProcessRecursively;
@property (assign) BOOL shouldSortClasses;
@property (assign) BOOL shouldSortClassesByInheritance;
@property (assign) BOOL shouldSortMethods;
@property (assign) BOOL shouldShowIvarOffsets;
@property (assign) BOOL shouldShowMethodAddresses;
@property (assign) BOOL shouldShowHeader;
@property (assign) int maxRecursiveDepth;
@property (assign) BOOL shouldIterateInReverse;
@property BOOL shouldOnlyAnalyze;
@property (copy, nonatomic) NSArray *forceRecursiveAnalyze;

@property (strong) NSRegularExpression *regularExpression;
- (BOOL)shouldShowName:(NSString *)name;

@property (strong) NSString *sdkRoot;

@property (readonly) NSArray *machOFiles;
@property (readonly) NSArray *objcProcessors;

@property (assign) CDArch targetArch;

@property (nonatomic, readonly) BOOL containsObjectiveCData;
@property (nonatomic, readonly) BOOL hasEncryptedFiles;
@property (nonatomic, readonly) BOOL hasObjectiveCRuntimeInfo;

@property (readonly) CDTypeController *typeController;

- (BOOL)loadFile:(CDFile *)file error:(NSError **)error depth:(int)depth;
- (void)processObjectiveCData;

- (void)recursivelyVisit:(CDVisitor *)visitor;

- (void)appendHeaderToString:(NSMutableString *)resultString;

- (void)registerTypes;

- (void)showHeader;
- (void)showLoadCommands;

@end

extern NSString *CDErrorDomain_ClassDump;
extern NSString *CDErrorKey_Exception;


