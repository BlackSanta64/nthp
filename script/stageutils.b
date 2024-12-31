DEPEND #CONTEXT_INIT_PROG
DEPEND #TEXTURES_DEFINED
/ 
Utility macros and constants for making stages.
Descriptions are included above symbol definitions. 
This is a BRANCH SCRIPT, do NOT INCLUDE IN BUILDSYSTEM. Interface with IMPORT.
/

VAR counter



MACRO f_setStageSize {
        CONST STAGE_SIZE
}
MACRO f_setHazardCount {
        CONST HAZARD_COUNT
}


MACRO f_stageInit {
        SET >entity_count #STAGE_SIZE
        ADD >entity_count 2 >entity_count

        ENT_DEFINE >entity_count
        
        / PLAYER SETUP /
        ENT_SETFRAMERANGE #e_player #f0_player 4
        ENT_SETRENDERSIZE #e_player 30 10
        ENT_SETHITBOXSIZE #e_player 30 10
        ENT_SETHITBOXOFFSET #e_player 0 0
        
        / PAN SETUP /
        ENT_SETFRAMERANGE #e_pan #f0_pan 1
        ENT_SETRENDERSIZE #e_pan 5 5
        ENT_SETHITBOXSIZE #e_pan 5 5
        ENT_SETHITBOXOFFSET #e_pan 0 0


        SET $counter 2

        LABEL 100
        

        ENT_SETFRAMERANGE $counter #f0_platform 1
        ENT_SETRENDERSIZE $counter 100 76
        ENT_SETHITBOXSIZE $counter 100 76


        INC $counter
        IF $counter LST >entity_count
                GOTO 100
        END

}


EXIT