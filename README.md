# Hawaii - remake

How to compile the project on windows with the original workflow:

- install dev-c++ 4.9.9.2
- install allegro and alfont devpaks
- rename collect2.exe to collect2.exe.old otherwise the very old compiler doesn't work
- add -std=c99 flag to compiler options

## Todos after refactoring effort is somewhat done:

- menu buttons (play menu buttons and play button itself) are a bit wonky, change the bboxes.
- options don't work, don't save or read from a file and don't apply.
- you can select the same color for both boats. (that is, if the options would actually work.)
- no need to select stuff like color depth
- volume should be a slider
- resolutions won't work when we draw everything to a specific pixel value (and I mean everything)
- resolutions should include some 16:9
- main music glitches out at the end and doesn't loop (even though it should)
- you should be able to reverse slowly (optional? find out if motorboat can do it first)

## What to work on next:

- find out how game_loop works
- make some sort of game init function so that everything gets reinited and the game can be played again
  - but first just the install int
- player xv and yv are always the same, the abstraction is wrong - should just be one variable for velocity
