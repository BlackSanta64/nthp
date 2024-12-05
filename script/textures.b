DEPEND #CONTEXT_INIT_PROG
/ This is a BRANCH SCRIPT. /

CONST t_player
CONST t_horizontalPlatform
CONST t_verticalPlatform

CONST textureCount 3


CONST f0_player 0 CONST f1_player 1 CONST f2_player 2 CONST f3_player 3   / 0 - 3 /
CONST f0_horizontal 4   / 4 /
CONST f0_vertical 5     / 5 /

CONST frameCount 6


MACRO f_setupTextures {
        SET_ACTIVE_PALETTE resource/genericPalette.pal

        TEXTURE_DEFINE #textureCount
        FRAME_DEFINE #frameCount
        
        TEXTURE_LOAD #t_player resource/player_sheet.cst
        TEXTURE_LOAD #t_horizontalPlatform resource/platform.cst
        TEXTURE_LOAD #t_verticalPlatform resource/wall.cst


        FRAME_SET #f0_player 0 0 80 85 #t_player
        FRAME_SET #f1_player 80 0 80 85 #t_player
        FRAME_SET #f2_player 0 85 80 85 #t_player
        FRAME_SET #f3_player 80 85 80 85 #t_player


        FRAME_SET #f0_horizontal 0 0 80 41 #t_horizontalPlatform
        FRAME_SET #f0_vertical 0 0 41 188 #t_horizontalPlatform

}

@f_setupTextures

EXIT