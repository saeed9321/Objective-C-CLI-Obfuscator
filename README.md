PreEmptive Protection for iOS - Class Guard
===========================================
*PreEmptive Protection for iOS - Class Guard*, or *PPiOS-CG* for short, is a command-line utility for obfuscating Objective-C class, protocol, property, and methods names, in iOS apps. It is based on [iOS-Class-Guard](https://github.com/Polidea/ios-class-guard) from [Polidea](https://www.polidea.com/), with extensive improvements and modifications.

*PPiOS-CG* can be used on its own or alongside [PreEmptive Protection for iOS](https://www.preemptive.com/products/ppios), [PreEmptive Solutions](https://www.preemptive.com/)' product for control-flow obfuscation of iOS apps.

*PPiOS-CG* works by generating a special set of `#define` statements (e.g. `#define createArray y09FYzLXv7T`) that automatically rename symbols during compilation. It includes a number of features:

* Analyze a Mach-O binary to identify symbols to be renamed
* Apply the renaming rules to the project source code
* Translate an obfuscated crash dump back to un-obfuscated names
* Generate unobfuscated dSYM files for upload to analytics tools, for automatic stack trace mapping

*PPiOS-CG* works with more than just your project's code. It also automatically finds symbols to exclude from renaming by looking at all external/dependent frameworks and in Core Data (xcdatamodel) files. The renamed symbols will also be applied to your XIB/Storyboard files, and to any open-source CocoaPods libraries in your project.

*PPiOS-CG* is licensed under the GNU GPL v2, but commercial support is also available from [PreEmptive Solutions](https://www.preemptive.com/contact/contactus) via a commercial support agreement. Please see LICENSE.txt for details.


How It Works
------------
*PPiOS-CG* is designed to be used in two phases of your build and release process. In the first phase, *PPiOS-CG* analyzes an unobfuscated compiled build of your app, to determine which symbols should be renamed and which should be excluded from renaming. In the second phase, *PPiOS-CG* applies those renaming rules to the **source code** of your app, so that the next build made from that source will be obfuscated. These two phases can be integrated into your build and release processes in a number of ways, including back-to-back.

### Phase 1: Analyze
`ios-class-guard --analyze [-m symbols.json] <Mach-O binary>`

In this phase, *PPiOS-CG* analyzes the unobfuscated compiled build of your app to determine which symbols should be renamed. It parses all classes, properties, methods and i-vars defined in that file adding all symbols to a "rename list". Then it builds up an "excludes list" with standard reserved words, symbols from dependent frameworks, symbols in Core Data (xcdatamodel) files, and any symbols that have been explicitly excluded via command-line arguments. It them combines those lists to generate a final list of symbols that will be renamed.

For each such symbol, it generates a random identifier, and writes a "map file" (`symbols.json`, by default) with the original names mapped to the new random names. That map file is the final output of the Analyze phase, and is a required input for the next phase, Obfuscate Sources.

> Note: Usually at this point, you should archive the `symbols.json` file. You will need it to be able to un-obfuscate any stack traces generated by builds that were obfuscated based on it.

### Phase 2: Obfuscate Sources
`ios-class-guard --obfuscate-sources [-m <symbols.json>]`

In this phase, *PPiOS-CG* reads in the map file and generates a header file (`symbols.h`, by default) that has `#define`s for each symbol to be renamed. It then finds the appropriate Precompiled Header (`.pch`) files in your source code and adds a `#import` with the path to the header file. Finally, it finds all XIBs/Storyboards in your source tree and directly updates the names inside those files.

Now, with the source modifications in place, you can build your app as usual. It will be compiled with the obfuscated symbols. (And any open-source CocoaPods will also have their symbols obfuscated.)

> Note: The Obfuscate Sources phase modifies the **source code** of your app, but you should not check in the changes it makes. If you do so, it will cause errors the next time you need to perform the Analyze phase, and will cause issues with Storyboards in the IDE. We recommend only using the Obfuscate Sources phase in your release (or automated) build process, and you should always clean/reset your source tree after the build, before doing any further development.


Supported Platforms
-------------------
*PPiOS-CG* supports apps developed for:

* iOS 9
* iPhone, iPod Touch, and iPad
* ARM 32-bit, 64-bit, and x86 simulator

Using:

* Xcode 7
* OSX El Capitan
* Objective-C


Installation
------------

We suggest downloading one of the binary releases from the [Releases](https://github.com/preemptive/ios-class-guard/releases) page. The archive contains a standalone binary that you can copy to an appropriate place on your system, e.g. `/usr/local/bin`. We suggest ensuring that the location is on your PATH. The release archive also includes other files such as this README, a changelog, and our license.

Project Setup
-------------
The basic process is:

1. Ensure all local source code changes have been committed
2. Build the program
3. Analyze the program
4. Obfuscate the sources
5. Build the program again

For your first time using *PPiOS-CG,* the following script (adjusted for your project) will perform the Analyze step:

    ios-class-guard --analyze /path/to/program.app/program

The analyze process generates `symbols.json`, the file containing a symbol mapping that can be used to decode stack traces in the event of a crash. The symbols file created during a build that is released should always be archived for subsequent use.

Then the Obfuscate Sources step can be accomplished with the following:

    ios-class-guard --obfuscate-sources

> Note: The Obfuscate Sources phase modifies the **source code** of your app, but you should not check in the changes it makes. If you do so, it will cause errors the next time you need to perform the Analyze phase, and will cause issues with Storyboards in the IDE. We recommend only using the Obfuscate Sources phase in your release (or automated) build process, and you should always clean/reset your source tree after the build, before doing any further development.

Once you are comfortable using *PPiOS-CG,* it can be easier to use if you integrate it into your Xcode project as part of the build process. This can be set up with the following process:

1. Open the project in Xcode.

2. Go to the Project Navigator, and select the project.

3. Select the icon to open the "Show project and targets list" (near the upper left corner of the main pane).

4. Select the target to obfuscate, right-click, and select Duplicate (Command-D).

5. Select the duplicated target and rename it to "Build and Analyze <original-target-name>".

6. Select Build Phases.

7. Add a script phase by selecting the "+" and then selecting New Run Script Phase (it should run as the last phase, and will by default).

8. Rename the phase from "Run Script" to "Analyze Binary".

9. Where it says "Type a script or ...", paste the following script, adjusting for the correct path:

        PATH="${PATH}:${HOME}/Downloads/PPiOS-ClassGuard-v1.0.0"
        [[ "${SDKROOT}" == *iPhoneSimulator*.sdk* ]] && sdk="${SDKROOT}" || sdk="${CORRESPONDING_SIMULATOR_SDK_DIR}"
        ios-class-guard --analyze --sdk-root "${sdk}" "${BUILT_PRODUCTS_DIR}/${EXECUTABLE_PATH}"

10. Go to Product | Scheme | Manage Schemes.

11. If Autocreate Schemes is enabled, a new scheme for the obfuscate target will have already been created, rename it to "Build and Analyze <original-scheme-name>".

12. Otherwise, create a new scheme for the Build and Analyze target.

13. Duplicate the original target again, and rename it to "Obfuscate Sources".

14. Delete all of the steps in this target.

15. If there are any target dependencies, delete them as well.

16. Add a script phase, and rename it to "Obfuscate Sources" (this should be the only real action for this target).

17. Paste the following script, again adjusting for the correct path:

        Path="${PATH}:${HOME}/Downloads/PPiOS-ClassGuard-v1.0.0"
        ios-class-guard --obfuscate-sources

18. Edit the scheme (or add one) for this new target, renaming the scheme to "Obfuscate Sources".

19. These changes should be committed to source control at this point, since building the Obfuscate Sources target will change the sources in ways that shouldn't generally be committed.


When ready to start testing an obfuscated build:

1. Ensure all local source code changes have been committed.

2. Build using the Build and Analyze scheme, producing the symbols file, `symbols.json`.

3. Commit or otherwise preserve the `symbols.json` file.

4. Build using the Obfuscate Sources scheme, which applies the renaming to the sources.

5. Build using the original scheme.

6. Revert changes to the sources before continuing development.

Once the sources are obfuscated, the process of building and testing for different destinations can be repeated using the original scheme (step #4), as long as you haven't reverted the sources yet (step #5).

If you modify the original build target or scheme, be sure to delete and recreate the Build and Analyze target as above. Under certain conditions, the Obfuscate Sources target and scheme will need to be recreated as well.


Using PPiOS-CG with PPiOS
-------------------------
*PreEmptive Protection for iOS - Class Guard* (*PPiOS-CG)* provides the "renaming" obfuscation, which is the most-common type of obfuscation typically applied to applications to help protect them from reverse engineering, intellectual property theft, software piracy, tampering, and data loss. There are additional obfuscation techniques, however, that are critically important for serious protection of apps. [PreEmptive Solutions](https://www.preemptive.com/) offers another product, simply named [PreEmptive Protection for iOS](https://www.preemptive.com/products/ppios), that includes multiple additional obfuscation transforms. *PPiOS-CG* is meant to work alongside *PPiOS*; together they provide much better protection than either one alone can provide.

If you have both *PPiOS-CG* and *PPiOS*, no special instructions are required for using them together. Set each one up according to its documentation, and they will each perform their obfuscation without affecting the other.

Troubleshooting
---------------

### Missing `.pch` file
During the Obfuscate Sources phase, you may get an error:

    Error: could not find any *-Prefix.pch files under .

This is because *PPiOS-CG* is attempting to add an `#import` to a Precompiled Header file, and it can't find a suitable file to add it to. This is typically because projects created in Xcode 6 and above don't contain a `.pch` file by default.

To fix this, add a `.pch` file as follows:

1. In Xcode go to *File -> New -> File -> iOS -> Other -> PCH File*.

2. Name the file e.g. `MyProject-Prefix.pch`. *PPiOS-CG* looks for a file matching `*-Prefix.pch`.

3. At the target's *Build Settings*, in *Apple LLVM - Language* section, set **Prefix Header** to your PCH file name.

4. At the target's *Build Settings*, in *Apple LLVM - Language* section, set **Precompile Prefix Header** to *YES*.


### Undefined symbols / exclusions

During the build, after the Obfuscate Source phase, you may see errors like this:

    Undefined symbols for architecture i386:
      "_OBJC_CLASS_$_n9z", referenced from:
          objc-class-ref in GRAppDelegate.o

You might also see `unresolved external` linker errors, e.g. if you used a C function and named an Objective-C method using the same name.

These errors usually mean that *PPiOS-CG* obfuscated a symbol that needs to be excluded for some reason. You can find the symbol by searching `symbols.json` or `symbols.h` for the referenced symbol (`n9z`, in this example) to see what the original name was. Then you can exclude the symbol via command-line arguments to the Analyze phase, via one of the two mechanisms below.

In this example, if `n9z` had mapped to `PSSomeClass`, you would add `-F '!PSSomeClass'` to your arguments when running `--analyze`.

#### Class / Protocol filter
You can exclude an entire class or protocol by using the `-F` option in the Analyze phase. For example:

    -F '!APH*' -F '!MC*' -F '!F?Box'

This will filter out any class in namespaces `APH` and `MC`. It will also filter out every class in namespaces that start with `F` and have a class name of `Box`.

`?` matches any single character, while `*` matches any number of characters.

#### Symbol filter
You can exclude specific symbols by using the `-i` argument in the Analyze phase. For example:

    -i 'deflate' -i 'curl_*'

This will filter out symbols named *deflate* and symbols that start with *curl_*.

`?` matches any single character, while `*` matches any number of characters.

#### Exclusion propagation
When excluding items using either `-i` or `-F`, if the excluded item matches a class or protocol name, then this can lead to a cascade effect of exclusions. If a class is excluded, then it will also exclude all of the class's methods and properties. This can extend to names of protocol methods/properties.

For example, if a class name is excluded, then the following is also excluded (assuming ClassName is the class name):

1. ClassNameProtocol
2. ClassNameDelegate
3. All of the methods and properties defined within the class

When excluding properties, the following names are also excluded (assuming the propery name is `propertyName`):

1. _propertyName
2. setPropertyName
3. isPropertyName


### XIB and Storyboards limitations
If you're using external libraries which provide Interface Builder files, be sure to ignore those symbols as they won't work when you launch the app and try to use them. You can do that using the `-F` option to the Analyze phase.

### Key-Value Observing (KVO)
It is possible that during obfuscation KVO will stop working. Most developers use hardcoded strings to specify *KeyPath*.

``` objc
- (void)registerObserver {
    [self.otherObject addObserver:self
                       forKeyPath:@"isFinished"
                          options:NSKeyValueObservingOptionNew
                          context:nil];
}

- (void)unregisterObserver {
    [otherObject removeObserver:self
                     forKeyPath:@"isFinished"
                        context:nil];
}

- (void)observeValueForKeyPath:(NSString *)keyPath
              ofObject:(id)object
                change:(NSDictionary *)change
               context:(void *)context
{
  if ([keyPath isEqualToString:@"isFinished"]) {
    // ...
  }
}
```

This will not work. The property *isFinished* will get a new name and the hardcoded string will not reflect the change.

Remove any *keyPath* and change it to `NSStringFromSelector(@selector(keyPath))`.

**The fixed code should look like this:**

``` objc
- (void)registerObserver {
    [self.otherObject addObserver:self
                       forKeyPath:NSStringFromSelector(@selector(isFinished))
                          options:NSKeyValueObservingOptionNew
                          context:nil];
}

- (void)unregisterObserver {
    [otherObject removeObserver:self
                     forKeyPath:NSStringFromSelector(@selector(isFinished))
                        context:nil];
}

- (void)observeValueForKeyPath:(NSString *)keyPath
              ofObject:(id)object
                change:(NSDictionary *)change
               context:(void *)context
{
  if ([keyPath isEqualToString:NSStringFromSelector(@selector(isFinished))]) {
    // ...
  }
}
```

### Serialization
If you use serialization (e.g. `NSCoding` or `NSUserDefaults`), affected classes will have to be excluded from obfuscation. If you don't, then you won't be able to generate new symbols (i.e. the Analyze phase) without breaking deserialization of existing data.


Advanced Topics
---------------

### Verifying obfuscation

To verify that your app has been obfuscated, use the `nm` utility, which is included in the Xcode Developer Tools. Run:

    nm path/to/your/binary | less

to see the symbols from your app. If you do this with an unobfuscated build, you will see the orginal symbols. If you do this with an obfuscated build, you will see obfuscated symbols. Note that nm will not work properly after stripping symbols from your binary. You can use the `otool` utility if you need to check for the Objective-C symbols after stripping. 

`otool` will print out unneeded information, but it can be filtered using `grep` and `awk` to only print out symbols:

    otool -o /path/to/your/binary | grep 'name 0x' | awk '{print $3}' | sort | uniq

Note that nm will not work properly after stripping symbols from your binary. You can use the `otool` utility if you need to check for the Objective-C symbols after stripping. `otool` will print out unneeded information, but it can be filtered using `grep` and `awk` to only print out symbols:

    otool -o /path/to/your/binary | grep 'name 0x' | awk '{print $3}' | sort | uniq


#### Reversing obfuscation in crash dumps
*PPiOS-CG* lets you reverse the process of obfuscation for crash dump files. This is important so you can find the original classes and methods involved in a crash. It does this by using the information from a map file (e.g. `symbols.json`) to modify the crash dump file, replacing the obfuscated symbols with the original names. For example:

    ios-class-guard --translate-crashdump -m path/to/symbols_1.0.0.json path/to/crashdump

> Note: **The crash dump file will be modified**

#### Reversing obfuscation in dSYMs
*PPiOS-CG* lets you reverse the process of obfuscation for automatic crash reporting tools such as HockeyApp, Crashlytics, Fabric, BugSense/Splunk Mint, or Crittercism. It does this by using the information from a map file (e.g. `symbols.json`) to generate a "reverse dSYM" file that has the non-obfuscated symbol names in it. For example:

    ios-class-guard --translate-dsym -m path/to/symbols_1.0.0.json path/to/input.dSYM path/to/output.dSYM

The resulting dSYM file can be uploaded to e.g. HockeyApp.


Command Line Argument Reference
-------------------------------
```
ios-class-guard --analyze [options] <mach-o-file>
  Analyze a Mach-O binary and generate a symbol map

  Options:
    -m <symbols.json>              Path to symbol map file
    -F <pattern>                   Specify filter for a class or protocol
    -i <pattern>                   Ignore obfuscation of specific symbol
    --arch <arch>                  Choose specific architecture from universal binary:
                                   ppc|ppc64|i386|x86_64|armv6|armv7|armv7s|arm64
    --sdk-root <path>              Specify full SDK root path
    --sdk-ios <version>            Specify iOS SDK by version, searching for:
                                   /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator<version>.sdk
    --list-excluded-symbols <file> Emit computed list of excluded symbols

ios-class-guard --obfuscate-sources [options]
  Alter source code (relative to current working directory), renaming based on the symbol map

  Options:
    -m <symbols.json>     Path to symbol map file
    -X <directory>        Path for XIBs and storyboards (searched recursively) (default: .)
    -O <symbols.h>        Path for the obfuscated symbols header

ios-class-guard --translate-crashdump [options] <crash dump file>
  Translate symbolicated crash dump

  Options:
    -m <symbols.json>     Path to symbol map file

ios-class-guard --translate-dsym [options] <input dir> <output dir>
  Translates dsym with obfuscated symbols to dsym with unobfuscated symbols

  Options:
    -m <symbols.json>     Path to symbol map file

ios-class-guard --list-arches <mach-o-file>
  List architectures available in a fat binary

ios-class-guard --version
  Print out version information

ios-class-guard --help
  Print out usage information
```

