/       CONTROL.B

This file is a branch of the main game runtime.
Handles player control, interaction, and collision.

/

DEPEND #core_context


MACRO f_playerControl {
        GETINDEX >returnRegister
        ADD 3 >returnRegister >returnRegister
        GOTO #l_playerControl
}

MACRO f_playerLogic {
        GETINDEX >returnRegister
        ADD 3 >returnRegister >returnRegister
        GOTO #l_playerLogic
}

MACRO f_applyPlayerMotion {
        GETINDEX >returnRegister
        ADD 3 >returnRegister >returnRegister
        GOTO #l_applyPlayerMotion
}



LABEL #l_applyPlayerMotion
        
        MUL >friction >deltatime >mathReg2

        IF >onSolidGround
                IF >playerXVelocity GRT 0
                        SUB >playerXVelocity >mathReg2 >playerXVelocity
                        IF >playerXVelocity LST 0
                                SET >playerXVelocity 0
                                SKIP
                        END
                ELSE
                        ADD >mathReg2 >playerXVelocity >playerXVelocity
                        IF >playerXVelocity GRT 0
                                SET >playerXVelocity 0
                                SKIP
                        END

                END


                SKIP_END

        END

        ENT_MOVE #e_player >playerXVelocity >playerYVelocity


        RETURN >returnRegister


LABEL #l_playerControl

        IF >moveLeft
                MUL >deltatime >playerAcceleration >mathReg0
                SUB >playerXVelocity >mathReg0 >playerXVelocity
        END
        IF >moveRight
                MUL >deltatime >playerAcceleration >mathReg0
                ADD >mathReg0 >playerXVelocity >playerXVelocity
        END

        IF >playerXVelocity GRT 0
                IF >playerXVelocity GRT #MAX_PLAYERSPEED
                        SET >playerXVelocity #MAX_PLAYERSPEED
                END
        ELSE
                IF >playerXVelocity LST #-MAX_PLAYERSPEED
                        SET >playerXVelocity #-MAX_PLAYERSPEED
                END
        END


        IF >jump
                IF >onSolidGround
                        SUB >playerYVelocity >jumpHeight >playerXVelocity
                END
        END




        RETURN >returnRegister


LABEL #l_playerLogic



        RETURN >returnRegister


EXIT