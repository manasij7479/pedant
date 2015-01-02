# pedant  
Pedant is a clang tool for catching inconsistent identifier names in your code.

## Build  
1. Clone the repo in `clang/tools/extra`  
2. Add `add_subdirectory(pedant)` to `clang/tools/extra/CMakeLists.txt`  
   or `pedant` to the DIRS variable in `clang/tools/extra/Makefile`  
3. Build  
  
## Example Invocation:  
`pedant ~/b.cpp -member=prefix-m_ --`  
This checks whether the class members in b.cpp have `m_` as a prefix.  

## Available Options:  
### Match Targets:
* class
* function
* member
* method  
   
### Preset Matchers:
* all
* none
* fooprefixed
* CamelCase
* camelCaseLite
* usep_lcase
  
### Dynamic Matchers:
* prefix
* regex
  
## TODO:
* More Match targets.
* Test cases.
* Use proper diagnostics instead of printing out results.
* Add `analysis` mode in which statistical data is generated for different Match targets instead of matching against specific requirements.
