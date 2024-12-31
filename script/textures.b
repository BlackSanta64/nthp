DEPEND #CONTEXT_INIT_PROG
/ This is a BRANCH SCRIPT. /

CONST t_player                  0
CONST t_platform                1
CONST t_pan                     2

CONST textureCount 3


CONST f0_player 0 CONST f1_player 1 CONST f2_player 2 CONST f3_player 3   / 0 - 3 /
CONST f0_platform 4     / 4 /
CONST f0_pan 5          / 5 /

CONST frameCount 6


MACRO f_setupTextures {
        SET_ACTIVE_PALETTE resource/genericPalette.pal

        TEXTURE_DEFINE #textureCount
        FRAME_DEFINE #frameCount
        
        TEXTURE_LOAD #t_player resource/player_sheet.cst
        TEXTURE_LOAD #t_platform resource/platform.cst
        TEXTURE_LOAD #t_pan resource/pan.cst


        FRAME_SET #f0_player 0 0 60 85 #t_player
        FRAME_SET #f1_player 60 0 60 85 #t_player
        FRAME_SET #f2_player 0 85 35 89 #t_player
        FRAME_SET #f3_player 35 85 35 88 #t_player


        FRAME_SET #f0_platform 0 0 80 41 #t_platform
        

        FRAME_SET #f0_pan 0 0 63 41 #t_pan
}

@f_setupTextures



CONST TEXTURES_DEFINED __DEFINED
EXIT