# srcbsprand
Basic command-line Source engine BSP randomizer (i.e. Glitch-Life), made just for fun. Not much features in it yet.

At the moment, it randomizes textures, models, NPCs, and certain entities.
By default, only textures used in the BSP will be randomized. srcbsprand can also scan VPKs for resources from a selected gamedir, but don't expect stable results. :)

The program copies selected .bsp and performs randomization, the original .bsp will stay unaltered.

# Args
```
-out, -output : Output directory.
-v, -verbose : (Very) verbose output.
-seed : Random seed, eg. -seed 133769420
-game, -gamedir : Location for the Source game folder you wish to get materials from.
-bsptexonly : Texture randomizer will only use textures inside the BSP. If no gamedir is available this will be used by default.
-ignorestorynpcs : "Story protected" NPCs will be randomized.
-dumpentlist : Dump entity list to the same dir as your BSP.
```
