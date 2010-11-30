That module is add-on to file-field module, it allow to code video with given parameters and generated preview for video and display video with flash player.

Working principle
1)	It is necessary to create preset in menu with path admin/build/simplevideo. You should select action which should be done by him. For example the preset could compress video or make previews. ATTENTION! Preset could make only 1 action! Also you could set how it will be done. It could use crone or immediately after node updating and saving.
We recommend to compress video using crone and generate previews when node is updating.

2)	You should create player (at the same menu as preset). Based on that player formatter for Filefield field for video downloading will be created. When you creating player you should set path to it, machinery name of formatter and human friendly name of formatter. Also you should enter parameters of player and variables. At variables you should set presets for video and for previews.

EXAMPLE base on jw player.
(http://www.longtailvideo.com/support/jw-player)

1)  Set path to player  “sites\all\modules\custom\videofield\players\mediaplayer-viral\player-viral.swf”
2) Set parameters for player ( check player's documentation ) for example:
			width  - 470
			height - 320
			allowfullscreen – TRUE
3) Set variables to player (check variables in documentation) you could check presets list at the bottom of table of available tokens
    FOR EXAMPLE file [video1]
		      img [thumb1]
		     type [video]
4)  When it ready don't forget to save it!
5)	Set necessary formatter with player at display-field properties file-field field.
6)	You should set path to ffmpeg.exe for windows and other cases if it necessary
