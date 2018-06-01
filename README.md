# File Management C++ library
#### This library provides many ways to manipulate both unix and windows encoded files, making it simpler to store and use data. Lines (divided by '\n' on unix and "\r\n" on Windows), words (divided by spaces) and chars are the basic items that can be managed. Some examples:
```cpp
sp::File inputFile("input.txt", sp::Newline::CRLF); //creates the object using the provided path. The sp::Newline parameter is the newline mode (in this case \r\n), if not provided it's automatically determined based on the operating system.
inputFile.create();                                 //creates the file if it doesn't exist.
inputFile.addLine(1, "This is the nice fzle!");     //adds a line between the first and the second line containing the parameter. (This is the nice fzle!)
inputFile.replaceWord(1, 2, "an");                  //replaces the third word on the second line with the parameter. (This is an nice fzle!)
inputFile.deleteChar(1, 2, 1);                      //deletes the second char of the third word of the second line. (This is a nice fzle!)
for (auto letter : inputFile.chars())               //cycles through all the chars in the file.
    if(letter == 'z') letter = 'i';                 //if the current char is a 'z' it is replaced with an 'i'. (This is a nice file!)
inputFile[0] = 589.7;                               //replaces the first line with "589.7"
     
std::cout << inputFile.getLine(1) << "\n----\n"     //prints all the content of the second line.
    << inputFile.getNrChars(0) << "\n----\n"        //prints the number of chars in the first line.
    << inputFile.getWords(1, 4, 2) << "\n----\n"    //prints the third, the fourth and the fifth word in the second line, but in reverse order since the second parameter is bigger than the third one. 
    << inputFile.str();                             //prints the entire file.
```
#### This outputs:
```
This is a nice file!
----
5
----
file! nice a
----
589.7
This is a nice file!
[  ...   (all the other lines)]
```
  
###### Some commits descriptions and comments are in italian language. Sorry for that, I didn't think I would have shared my code.
