DEPEND #CONTEXT_INIT_PROG

/ 
Utility macros and constants for making stages.
Descriptions are included above symbol definitions. 
This is a BRANCH SCRIPT, do NOT INCLUDE IN BUILDSYSTEM. Interface with IMPORT.
/

VAR ___stage_size

/ INTERFACE CONSTANTS
        #STAGE_SIZE
        #HAZARD_COUNT
/

MACRO f_setStageSize {
        CONST STAGE_SIZE
}
MACRO f_setHazardCount {
        CONST HAZARD_COUNT
}


MACRO f_stageInit {
        SET $___stage_size #STAGE_SIZE
        ADD $___stage_size 2 $___stage_size

        ENT_DEFINE $___stage_size
        

}


EXIT