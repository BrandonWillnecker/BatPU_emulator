Here is an assembler and emmulator for the CPU designed by MattBatWings (https://github.com/mattbatwings, https://www.youtube.com/c/Mattbatwings).
I made it in C++ while following along with his YouTube videos on the CPU design and building. The videos are very well made!!
I have followed his specifications sheet on his GitHib so the emmulator is CPU accurate
As of 1 Jan 2025, he hasnt uploaded the videos about the screen and input control so that hasn't been included yet but the definitions are there
The cpp file contains the emmulator and the assembler that produces the .mc (machine code) text files given the assembly files. It also produces .bin (bianry) files with bytes of the machine code
Currently, there are very few comments and no optimizations have been made at all. I just uploaded the file once I got it working.

My goal is to create a simplified C compiler that takes a C program, translates it into *MattBatWings Assembly* and then into *MattBatWings Machine Code* that will run on the in-minecraft CPU that he made

I'll start with flow structure and functions then get to arrays and all that. I am somewhat familar with tokenizers and interpreters so we will see how this goes :)

Some resources: (I'll add more as I figure this out)
Javidx9 How does assembly work - https://www.youtube.com/watch?v=1FXhjErUz58
MIT Assembly language and computer architecture - https://www.youtube.com/watch?v=L1ung0wil9Y

Please share your thoughts

11/01 - I think the tokenizer is done now. It's been tested a bit and seems to be working fine. I would like to add more info in the token struct like the line number on which the token occurs for error returning. The preprocessing comes before tokenizing so things will be added/removed thus messing up the line numbering. Not an urgent issue.
      - I have started making a parsing system. Online resources gave tutorials on parsing numerical expressions which is a bit too basic. I haven't looked at any compiler textbooks yet. I want to try figure it out (even if its not a great solution) before looking it up. This is not a serious project. My current idea is to try and pattern match lists of consecutive tokens to variable declaration patterns, function definition patterns, etc like a regular expression. The time complexity is terrible. I had another idea where you have an initial guess of what the statement means and then using the next token, update what it could be. Eg:
        int func(int a, int b);  compared to int func(int a, int b){...
                             /\                                    /\     At this point the statement is either a function declaration or definition. Before this it's ambiguous
                             
