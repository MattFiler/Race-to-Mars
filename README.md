<img align="right" src="https://i.imgur.com/paT8WSh.png" width="100%" alt="Race to Mars" style="padding-bottom:20px;">

&nbsp;

This was a group project to create a four player board game and digitise it to a turn-based multiplayer game for PC.

After concepting, designing, and testing our board game, we arrived at Race to Mars - a four player co-operative game where players work together to solve issues on their journey to Mars. Players win by reaching Mars, or lose by being overwhelmed by issues. This repo holds the digitised game, created following the rules of the board game in OpenGL using the engine framework ASGE.

### Creating and Joining a Game

To play, a game server instance must be running. Once a server is active, you can connect using the IP field in the game launcher. Servers work by a lobby system, so although the game is for four players only, multiple games can run simultaneously on any one server.

A tutorial is included in the game's main menu.

To get into the game itself, select "New Game" and you will be placed into a lobby. Once four players have joined and readied up, the game can begin.

### Gameplay

<img align="right" src="https://i.imgur.com/O7Nc6f8.jpg" width="50%">

Each full rotation of the game, between 1 and 3 issue cards are drawn. Each issue card is either a modifier (a positive/negative modification of the current turn) or an issue (a problem to solve, with required Action Point values of 5, 10, or 15). To solve issues, you must put Action Points towards them. Action Points are gained each turn by rolling the dice, and additionally, buying items from the Supply Bay.

To solve an issue you must be in the correct room of the ship - to move, click on the room you wish to go to. You can only move to rooms that neighbour your current one (E.G. you cannot jump from one side of the ship to the other), so to move across the board you must click to move in sequence. Moving costs 1 Action Point, except for a movement to your own room, which is free. To buy an item you must be in the supply bay. Items do not go towards movement points, they only assign to issues of their specified class.

For a full tutorial with image examples, check out the tutorial menu in-game!

### Team

This project was put together by:

- [Matt Filer](http://www.mattfiler.co.uk) (UI, networking, systems programmer)
- [Jack King](https://github.com/Jack-King1) (card programmer, networking)
