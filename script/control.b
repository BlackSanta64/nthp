DEPEND >returnRegister
DEPEND >moveLeftAction
DEPEND >moveRightAction
DEPEND >jumpAction
DEPEND >panThrowAction


CONST l_playerControl 834

MACRO f_playerControl {
        GETINDEX >returnRegister
        ADD 3 >returnRegister >returnRegister
        GOTO #l_playerControl
}



LABEL #l_playerControl

        IF >exitAction
                CORE_STOP
                RETURN >returnRegister
        END


        IF >moveLeftAction
                SUB >playerXVelocity >playerSpeed >playerXVelocity
        END

        IF >moveRightAction
                ADD >playerXVelocity >playerSpeed >playerXVelocity
        END

        IF >jumpAction
                IF >onSolidGround
                        SUB >playerYVelocity >jumpHeight >playerYVelocity
                        SET >onSolidGround 0
                END
        END



        RETURN >returnRegister



EXIT