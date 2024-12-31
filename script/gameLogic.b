CONST l_gameLogic 20
CONST l_renderStage 30
CONST l_checkCollision 40
VAR counter
VAR b_collide_count

GLOBAL pl_hx
GLOBAL pl_hy
GLOBAL pl_hw
GLOBAL pl_hh


MACRO f_gameLogic {
        GETINDEX >returnRegister
        ADD 3 >returnRegister >returnRegister
        GOTO #l_gameLogic
}


MACRO f_renderStage {
        GETINDEX >returnRegister
        ADD 3 >returnRegister >returnRegister
        GOTO #l_renderStage
}


MACRO f_checkCollision {
        GETINDEX >returnRegister
        ADD 3 >returnRegister >returnRegister
        GOTO #l_checkCollision
}


LABEL #l_gameLogic

        IF >playerXVelocity GRT 0.5
                SET >playerXVelocity 0.5

                GOTO 11
        END

        IF >playerXVelocity LST -0.5
                SET >playerXVelocity -0.5

                GOTO 11
        END


        LABEL 11

        ENT_MOVE #e_player >playerXVelocity >playerYVelocity


        IF >playerXVelocity GRT 0
                SUB >playerXVelocity >airResistance >playerXVelocity
                IF >playerXVelocity LST 0
                        SET >playerXVelocity 0
                        GOTO 12
                END
                GOTO 12
        END

        IF >playerXVelocity LST 0
                ADD >playerXVelocity >airResistance >playerXVelocity
                IF >playerXVelocity GRT 0
                        SET >playerXVelocity 0
                        GOTO 12
                END
        END

        LABEL 12

        IF >onSolidGround EQU 0
                ADD >playerYVelocity >gravity >playerYVelocity
                IF >playerYVelocity GRT #MAX_AIRSPEED
                        SET >playerYVelocity #MAX_AIRSPEED
                        GOTO 13
                END
        END
        
        LABEL 13


        RETURN >returnRegister




LABEL #l_renderStage
        SET $counter 0

        CORE_CLEAR
        LABEL 1000

                CORE_QRENDER $counter

                INC $counter
                IF $counter LST >entity_count
                        GOTO 1000
                END
        
        CORE_DISPLAY


        RETURN >returnRegister


CONST obj_hx >r_poll1
CONST obj_hy >r_poll2
CONST obj_hw >r_poll3
CONST obj_hh >r_poll4


LABEL #l_checkCollision

        SET $counter 2


        LABEL 554

        ENT_CHECKCOLLISION #e_player $counter $b_collide_count
        IF $b_collide_count
                POLL #e_player HITBOX
                COPY >r_poll1 >pl_hx
                COPY >r_poll2 >pl_hy
                COPY >r_poll3 >pl_hw
                COPY >r_poll4 >pl_hh

                POLL $counter HITBOX / Now the POLL registers are the colliding object's hitbox data /
                IF >pl_hy LSTE #obj_hy / Must be TOP-DOWN or corners /
                        IF >pl_hx LSTE #obj_hx / LEFT-CORNER collision /
                                
                        END

                END

                END 





        END
        RETURN >returnRegister



EXIT