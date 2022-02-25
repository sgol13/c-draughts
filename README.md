# Draughts
This is my implementation of a draughts game in C which allows two users to play against each other. The programs has no GUI and displays a game board in terminal. I've used [libxml2](http://xmlsoft.org/) for parsing XML files and CMake for project compilation. In the video below you can see an exemplary gameplay.

<a href="https://youtu.be/hERW3JtGsnE">
         <img alt="Draughts video" src="https://i.postimg.cc/cJQb2Pny/play-screen.png" width="500">
</a>

## Requirements
* CMake
* Make
* libxml2 ([download](http://xmlsoft.org/downloads.html))

## Compilation
First, clone the repository:

```
git clone https://github.com/sgol13/c-draughts.git
cd c-draughts
```

Then you can compile and run the game by using a prepared bash script:
```
./run
```

## License
This project is under MIT [license](LICENSE).
