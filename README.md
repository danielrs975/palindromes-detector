# Detector of palindromes in the file system
This program detects palindromes in the names of the folders and files inside the file system. Written in C.

## Description of the project
A palindrome is a special word that can be read in the same way if you write it backwards. The program objective is to explore the file system
tree to search if a palindrome exists. The words are form with the concatenation of the names of the items. These words can be formed with any subset
of the items inside the file system tree. The file system tree is limited to the files inside a given root.

          b
         / \  
       |a|  e
       / \   \
     |s|  c   p
     / \       \
    o  |a|      x

         asa

Explanation: in this example we begin in a folder called 'b' then we explore the inside this folder. We do this until we do not have anything else to
explore. The leaf of the tree are the files. In here we found a palindrome 'asa'.

## How to use the program?
To use the program you only have to compile it ```gcc main.c -o main``` and then run the executable ```./main```

## Options available
Different flags exists to run the program
- ```-d <folder>``` establishes the folder where the program will start running
- ```-m <height>``` establishes the height of the directory tree the program will explore to find the palindromes
- ```-f``` establishes that the name of the files are taken in consideration