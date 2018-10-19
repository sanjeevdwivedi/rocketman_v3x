# rocketman_v3x
Rocketman Game Ported to v3.x and with Win10 support

The Rocketman_v3x is a Cocos2d-x v3.x port of the previous game that I used in an MVA (https://github.com/sanjeevdwivedi/rocket-man-game)
The game has been updated to:

- Work against v3 APIs
- support v3 style event models
- Remove batch nodes. They are not needed as of v3 and recommended against by the Cocos2d-x community
- All sprites have been merged into a single spritesheet using TexturePacker. 
- Loading sprites is done using SpriteFrameCache and spriteWithSpriteFrameName.
- Rocketman has a flame coming out of the jetpack which is animated 
