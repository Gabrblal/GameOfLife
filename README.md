# Conway's Game Of Life

A C++ and [SFML](https://www.sfml-dev.org/) implementation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life). A cell survives if that are 2 or 3 cells around it, or else it dies. A cell comes into existence with exactly
3 cells surviving around it.

- Controls:
  - `space` - Pause and unpause
  - `escape` - Exit
  - `WASD` - Pan the Camera
  - `scroll` - Zoom in and out
  - `left click` - Set a tile
  - `right click` - Remove a tile
  - `up arrow` - Increase simulation speed.
  - `down arrow` - Reduce simulation speed.

![gameoflife](https://user-images.githubusercontent.com/52615052/113376056-39ea8800-93b4-11eb-9e9e-4388e7ef3d65.gif)
