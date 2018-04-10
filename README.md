# File Management C++ library
#### This library provides many ways to manipulate files, making it simpler to store and use data. Lines (divided by "\r\n"), words (divided by spaces) and chars are the main things that can be edited. Some examples:
```cpp
sp::File inputFile("input.txt");                  //creates the object using one path. If provided with two paths the second one is used as temp file.
inputFile.create();                               //creates the file if it doesn't exist.
inputFile.addLine(1, "This is the input fzle!");  //adds a line in second position containing the parameter. (This is the input fzle!)
inputFile.replaceWord(1, 2, "ann");               //replaces the third word on the second line with the parameter. (This is ann input fzle!)
inputFile.deleteChar(1, 10);                      //deletes the eleventh char on the second line using the temp file. (This is an input fzle!)
for (auto letter : inputFile)                     //cycles through all the chars in the file.
    if(letter == 'z') letter = 'i';               //if the current char is a 'z' it is replaced with an 'i'. (This is an input file!)
    
std::cout << inputFile.getLine(1) << "\n----\n"   //prints all the content of the second line
    << inputFile.getNrWords(1) << "\n----\n"      //prints the number of words in the second line
    << inputFile.str();                           //prints the entire file
```
#### If the file "input.txt" was empty, this outputs:
```
This is an input file!
----
5
----

This is an input file!
```
  
###### Some commits descriptions and comments are in italian language, sorry for that, I didn't think I would have shared my code.
