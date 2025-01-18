/       STAGE.B
        Stage structure is defined here. Utilities to write and load stages are defined here.


STAGE CACHE STRUCTURE:
        [0] Size of stage (in objects not including player and pan)

        [...] x
        [...] y
        [...] w
        [...] h

/
DEPEND #core_context
CONST l_initStageObjects        25
CONST l_addObject               26


VAR entity_counter
VAR size
VAR ptr_object

VAR r_x
VAR r_y
VAR r_w
VAR r_h


GLOBAL stageSize

MACRO loadStage {
        CACHE_CLEAR

        CACHE_OPEN #ar0
        CACHE_READ 0 >stageSize
}

/ Saves the stage as a file specified in arg0 /
MACRO saveStage {
        CACHE_SAVE #ar0
}

/ Creates a stage structure with 1 object /
MACRO createStage {
        CACHE_CREATE 1
        CACHE_WRITE 0 1
        SET >stageSize 0
}



MACRO f_initStageObjects {
        GETINDEX >returnRegister
        ADD 3 >returnRegister >returnRegister
        GOTO #l_initStageObjects
}


/ Defines new stage geometry with arg0-arg3 [x y w h] /
MACRO f_addObject {
        SET $r_x #ar0
        SET $r_y #ar1
        SET $r_w #ar2
        SET $r_h #ar3

        GETINDEX >returnRegister
        ADD 3 >returnRegister >returnRegister
        GOTO #l_addObject
}


LABEL #l_initStageObjects
        SET $ptr_object 1
        SET $entity_counter 2
        
        ADD >stageSize 2 >mathReg1
        ENT_DEFINE >mathReg1

        LABEL 625

        CACHE_READ $ptr_object $r_x
        INC $ptr_object
        CACHE_READ $ptr_object $r_y
        INC $ptr_object
        CACHE_READ $ptr_object $r_w
        INC $ptr_object
        CACHE_READ $ptr_object $r_h

        ENT_SETHITBOXSIZE $entity_counter $r_w $r_h
        ENT_SETPOS $entity_counter $r_x $r_y
        ENT_SETFRAMERANGE $entity_counter #f0_wall 1
        ENT_SETRENDERSIZE $entity_counter $r_w $r_h


        INC $ptr_object
        INC $entity_counter
        IF $entity_counter LST >stageSize
                GOTO 625
        END

        RETURN >returnRegister


LABEL #l_addObject
        INC >stageSize
        MUL >stageSize 4 $ptr_object
        INC $ptr_object
        CACHE_RESIZE $ptr_object
        CACHE_WRITE 0 >stageSize

        MUL >stageSize 4 $ptr_object
        SUB $ptr_object 3 $ptr_object
        
        CACHE_WRITE $ptr_object $r_x
                INC $ptr_object
        CACHE_WRITE $ptr_object $r_y
                INC $ptr_object
        CACHE_WRITE $ptr_object $r_w
                INC $ptr_object
        CACHE_WRITE $ptr_object $r_h


        RETURN >returnRegister



EXIT