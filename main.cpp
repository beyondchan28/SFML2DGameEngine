//#include "Game.h"
#include "GameEngine.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Vec2.h"

#include <memory>
#include <vector>

class Object
{
    int a = 5;

public:
    Object(int num) : a(num) {};

    int & getNumber()
    {
        return a;
    }
};


int main()
{
    /**
    Object a = {6};             // constructing/creating object 'a'
    Object a(6);                // also constructing/creating object 'a'
    Object * b = new Object(6); // storing new object to pointer
    Object * c = &a;            // pointing to address 'a'
    Object d = a;               // copying object 'a'
    Object * e = b;             // pointer to pointer
    **/
    /**
    for the sake of cheap computation. because pointer is just an "aliases"
    for A that u want to accessing. despite put 'all the thing' A had
    inside a new bucket, u just pointing to the bucket that stored A


    **/
    //const int b = a.getNumber() + 1;

    //Game game;
    //game.run();

    std::shared_ptr<GameEngine> game = std::make_shared<GameEngine>("assetsConfig.txt");
    game->run();


    return 0;

}
