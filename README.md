# DraughtsGame
Simple Draughts Game - 10x10 board with international draughts rules. 

 The gameboard comprises 10×10 squares in alternating dark and light colours, of which only the 50 dark squares are used. Each player has 20 pieces, light for one player and dark for the other, at opposite sides of the board.

## UI
- Main.qml - Serves as an entry point & holds the main application window 
- MainBoard.qml - The 2D checkerboard layout & provides an interface for coin movements
- Theme.qml - Single point of entity which holds all the colour codings & scaling factor values
- Coin.qml - Visual representation of the draught's piece
- CButton.qml - Custom qml button with background fill and text
- CTextBold.qml - Custom qml text to maintain uniformity
- CPopup.qml - Blocking dialog - used for winner announcement
- ScoreCardItem.qml - Shows the scorecard for each player
- PlayerHighlighter.qml - Active player highlighter
- HeaderItem.qml - Topbar item which is used to show active player and warning messages

## Backend
- Main.cpp/h - Qt app eventloop entry
- Board.cpp/h - Tracks the coin movement and does move validations
  - Used a QVector-based container to track the board state and player movements
  - Property to hold score, current player and number of moves, ....
  - void move() is the main function to validate the player's movements
- PropertyHelper.h - Utility macro which reduces the efforts required for Q_PROPERTY declarations
- Enums.h - Holds the app-wide Enums and provides a qml access to do type checks & validations
- Piece.h - Template which holds the property(active, type and player) of each cell in the checkerboard

## Build
- Project configured and developed using Qt 5.15.2
- Recommended to build and run using the same version to avoid compilation issues

## Improvements
- Board::dataChanged - This signal acts as a notifier to qml to update the board state. Implementing the specific index updation or role-based approach will increase the performance further
- QAbstractListModel can be used to hold data that has built-in mechanisms to notify about the qml state changes
- Drag and drop of coins can be done for easier operation

## Other approaches tried
- Tableview and QAbstractTableModel method - faced some disadvantages with the default row and column numbering
