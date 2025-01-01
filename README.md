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
